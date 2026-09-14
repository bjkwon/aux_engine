// Regression test for wav header handling.
//
// A wav whose "data" chunk declares far more bytes than the file holds used to
// make _wave() allocate (and zero) a buffer sized from the header, which
// segfaulted inside body::UpdateBuffer. Loading such a file must now either
// fail cleanly or return only the frames that are actually present.

#include <auxe/auxe.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  fs::path dir;
  std::string err;

  explicit Session(const std::string& name) {
    dir = fs::temp_directory_path() / "auxe_regression_wav_header" / name;
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir, ec);
    if (ec) {
      err = "Could not create temp dir: " + dir.string();
      return;
    }
    cfg.sample_rate = 22050;
    cfg.display_precision = 6;
    cfg.display_limit_x = 64;
    cfg.display_limit_y = 64;
    cfg.display_limit_bytes = 512;
    cfg.display_limit_str = 512;
    cfg.search_paths = {dir.string()};
    cfg.debug_hook = nullptr;
    ctx = aux_init(&cfg);
    if (!ctx) err = "aux_init returned null.";
  }
  ~Session() {
    if (ctx) aux_close(ctx);
  }
  bool ok() const { return err.empty() && ctx != nullptr; }
};

void put_u16(std::vector<uint8_t>& b, uint16_t v) {
  b.push_back((uint8_t)(v & 0xFF));
  b.push_back((uint8_t)(v >> 8));
}

void put_u32(std::vector<uint8_t>& b, uint32_t v) {
  for (int k = 0; k < 4; ++k) b.push_back((uint8_t)((v >> (8 * k)) & 0xFF));
}

void put_id(std::vector<uint8_t>& b, const char* id) {
  b.insert(b.end(), id, id + 4);
}

// 16-bit PCM mono wav. declared_data_size overrides the data chunk size field
// (0 means "use the real size"); block_align_override does the same for fmt.
std::vector<uint8_t> make_wav(uint16_t channels, uint32_t sample_rate, uint32_t frames,
                              uint32_t declared_data_size, uint16_t block_align_override) {
  const uint16_t bits = 16;
  const uint16_t block_align = block_align_override ? block_align_override : (uint16_t)(channels * bits / 8);
  const uint32_t real_data_size = frames * channels * (bits / 8);
  const uint32_t data_size_field = declared_data_size ? declared_data_size : real_data_size;

  std::vector<uint8_t> b;
  put_id(b, "RIFF");
  put_u32(b, 36 + real_data_size);
  put_id(b, "WAVE");
  put_id(b, "fmt ");
  put_u32(b, 16);
  put_u16(b, 1);  // PCM
  put_u16(b, channels);
  put_u32(b, sample_rate);
  put_u32(b, sample_rate * block_align);
  put_u16(b, block_align);
  put_u16(b, bits);
  put_id(b, "data");
  put_u32(b, data_size_field);
  for (uint32_t k = 0; k < frames * channels; ++k) put_u16(b, (uint16_t)(k * 16));
  return b;
}

// Audio signals report their length through the channel API, not aux_vector_length.
size_t sample_count(const AuxObj& obj) {
  const size_t n = aux_flatten_channel_length(obj, 0);
  return n ? n : aux_vector_length(obj);
}

bool write_bytes(const fs::path& p, const std::vector<uint8_t>& bytes, std::string& err) {
  FILE* fp = fopen(p.string().c_str(), "wb");
  if (!fp) {
    err = "Cannot write " + p.string();
    return false;
  }
  fwrite(bytes.data(), 1, bytes.size(), fp);
  fclose(fp);
  return true;
}

// A well-formed wav still loads, with the expected number of samples.
bool case_valid_wav_loads(std::string& err) {
  Session s("valid");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "good.wav";
  if (!write_bytes(wav, make_wav(1, 22050, 100, 0, 0), err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Loading a valid wav failed: " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) { err = "x not found."; return false; }
  if (sample_count(obj) != 100) {
    err = "Expected 100 samples, got " + std::to_string(sample_count(obj));
    return false;
  }
  return true;
}

// The crash case: the header claims ~3.8 GB of audio, the file holds 100 frames.
bool case_oversized_data_chunk(std::string& err) {
  Session s("oversized");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "oversized.wav";
  if (!write_bytes(wav, make_wav(1, 22050, 100, 0xE2000000u, 0), err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  const int rc = aux_eval(&s.ctx, cmd, s.cfg, preview);
  if (rc == (int)auxEvalStatus::AUX_EVAL_OK) {
    AuxObj obj = aux_get_var(s.ctx, "x");
    if (!obj) { err = "x not found."; return false; }
    if (sample_count(obj) > 100) {
      err = "Header size was trusted over the file size: got " +
            std::to_string(sample_count(obj)) + " samples.";
      return false;
    }
    return true;
  }
  // A clean error is acceptable too; a crash or an empty message is not.
  if (preview.empty()) {
    err = "Failed without an error message.";
    return false;
  }
  return true;
}

// block_align == 0 used to divide by zero / produce nonsense lengths.
bool case_zero_block_align(std::string& err) {
  Session s("blockalign");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "badfmt.wav";
  // block_align_override cannot be 0 through the helper's "0 means default"
  // convention, so patch the field directly after building the file.
  std::vector<uint8_t> bytes = make_wav(1, 22050, 100, 0, 0);
  bytes[32] = 0;
  bytes[33] = 0;  // block_align field
  if (!write_bytes(wav, bytes, err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  const int rc = aux_eval(&s.ctx, cmd, s.cfg, preview);
  if (rc == (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "A wav with block_align=0 should not load successfully.";
    return false;
  }
  if (preview.empty()) {
    err = "Failed without an error message.";
    return false;
  }
  return true;
}

// 24-bit PCM wav, either plain (format 1) or WAVE_FORMAT_EXTENSIBLE.
// Sample n of channel c is (n * channels + c) scaled up into the 24-bit range,
// so the decoded value is predictable.
int32_t pcm24_sample(uint32_t index) {
  // Spread the test values over the negative and positive halves of the range.
  return (int32_t)index * 4096 - 1048576;
}

std::vector<uint8_t> make_wav24(uint16_t channels, uint32_t sample_rate, uint32_t frames,
                                bool extensible) {
  const uint16_t bits = 24;
  const uint16_t block_align = (uint16_t)(channels * bits / 8);
  const uint32_t real_data_size = frames * block_align;
  const uint32_t fmt_size = extensible ? 40u : 16u;

  std::vector<uint8_t> b;
  put_id(b, "RIFF");
  put_u32(b, 20 + fmt_size + real_data_size);
  put_id(b, "WAVE");
  put_id(b, "fmt ");
  put_u32(b, fmt_size);
  put_u16(b, extensible ? 0xFFFE : 1);
  put_u16(b, channels);
  put_u32(b, sample_rate);
  put_u32(b, sample_rate * block_align);
  put_u16(b, block_align);
  put_u16(b, bits);
  if (extensible) {
    put_u16(b, 22);    // cbSize
    put_u16(b, bits);  // validBitsPerSample
    put_u32(b, channels == 2 ? 0x3 : 0x4);  // channel mask
    // PCM subformat GUID {00000001-0000-0010-8000-00AA00389B71}
    const uint8_t guid[16] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                              0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
    b.insert(b.end(), guid, guid + 16);
  }
  put_id(b, "data");
  put_u32(b, real_data_size);
  for (uint32_t k = 0; k < frames * channels; ++k) {
    const int32_t s = pcm24_sample(k);
    b.push_back((uint8_t)(s & 0xFF));
    b.push_back((uint8_t)((s >> 8) & 0xFF));
    b.push_back((uint8_t)((s >> 16) & 0xFF));
  }
  return b;
}

bool check_channel(const AuxObj& obj, int channel, uint16_t channels, uint32_t frames,
                   std::string& err) {
  std::vector<auxtype> got(frames, 0);
  const size_t n = aux_flatten_channel(obj, channel, got.data(), got.size());
  if (n != frames) {
    err = "Channel " + std::to_string(channel) + ": expected " + std::to_string(frames) +
          " samples, got " + std::to_string(n);
    return false;
  }
  for (uint32_t k = 0; k < frames; ++k) {
    const double expected = (double)pcm24_sample(k * channels + (uint32_t)channel) / 8388608.0;
    if (std::abs((double)got[k] - expected) > 1e-6) {
      err = "Channel " + std::to_string(channel) + ", sample " + std::to_string(k) +
            ": expected " + std::to_string(expected) + ", got " + std::to_string((double)got[k]);
      return false;
    }
  }
  return true;
}

// 24-bit PCM used to be rejected outright; it must now decode to the right values.
bool case_pcm24_mono(std::string& err) {
  Session s("pcm24_mono");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "pcm24.wav";
  if (!write_bytes(wav, make_wav24(1, 22050, 64, false), err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Loading a 24-bit wav failed: " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) { err = "x not found."; return false; }
  if (sample_count(obj) != 64) {
    err = "Expected 64 samples, got " + std::to_string(sample_count(obj));
    return false;
  }
  return check_channel(obj, 0, 1, 64, err);
}

// 24-bit stereo carried in WAVE_FORMAT_EXTENSIBLE, the common case from DAWs.
bool case_pcm24_stereo_extensible(std::string& err) {
  Session s("pcm24_stereo");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "pcm24x.wav";
  if (!write_bytes(wav, make_wav24(2, 22050, 32, true), err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Loading a 24-bit extensible wav failed: " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) { err = "x not found."; return false; }
  if (aux_num_channels(obj) != 2) {
    err = "Expected 2 channels, got " + std::to_string(aux_num_channels(obj));
    return false;
  }
  if (!check_channel(obj, 0, 2, 32, err)) return false;
  return check_channel(obj, 1, 2, 32, err);
}

// IEEE float wav (32 or 64 bit), either plain (format 3) or WAVE_FORMAT_EXTENSIBLE.
double float_sample(uint32_t index) {
  // Values that survive a float32 round trip exactly, spanning both signs.
  return (double)((int32_t)index - 16) / 64.0;
}

std::vector<uint8_t> make_wav_float(uint16_t channels, uint32_t sample_rate, uint32_t frames,
                                    uint16_t bits, bool extensible) {
  const uint16_t block_align = (uint16_t)(channels * bits / 8);
  const uint32_t real_data_size = frames * block_align;
  const uint32_t fmt_size = extensible ? 40u : 16u;

  std::vector<uint8_t> b;
  put_id(b, "RIFF");
  put_u32(b, 20 + fmt_size + real_data_size);
  put_id(b, "WAVE");
  put_id(b, "fmt ");
  put_u32(b, fmt_size);
  put_u16(b, extensible ? 0xFFFE : 3);  // 3 = IEEE_FLOAT
  put_u16(b, channels);
  put_u32(b, sample_rate);
  put_u32(b, sample_rate * block_align);
  put_u16(b, block_align);
  put_u16(b, bits);
  if (extensible) {
    put_u16(b, 22);    // cbSize
    put_u16(b, bits);  // validBitsPerSample
    put_u32(b, channels == 2 ? 0x3 : 0x4);  // channel mask
    // IEEE float subformat GUID {00000003-0000-0010-8000-00AA00389B71}
    const uint8_t guid[16] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                              0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
    b.insert(b.end(), guid, guid + 16);
  }
  put_id(b, "data");
  put_u32(b, real_data_size);
  for (uint32_t k = 0; k < frames * channels; ++k) {
    if (bits == 32) {
      const float f = (float)float_sample(k);
      uint32_t bitsval;
      std::memcpy(&bitsval, &f, 4);
      put_u32(b, bitsval);
    } else {
      const double d = float_sample(k);
      uint64_t bitsval;
      std::memcpy(&bitsval, &d, 8);
      put_u32(b, (uint32_t)(bitsval & 0xFFFFFFFFu));
      put_u32(b, (uint32_t)(bitsval >> 32));
    }
  }
  return b;
}

bool check_float_channel(const AuxObj& obj, int channel, uint16_t channels, uint32_t frames,
                         std::string& err) {
  std::vector<auxtype> got(frames, 0);
  const size_t n = aux_flatten_channel(obj, channel, got.data(), got.size());
  if (n != frames) {
    err = "Channel " + std::to_string(channel) + ": expected " + std::to_string(frames) +
          " samples, got " + std::to_string(n);
    return false;
  }
  for (uint32_t k = 0; k < frames; ++k) {
    const double expected = float_sample(k * channels + (uint32_t)channel);
    if (std::abs((double)got[k] - expected) > 1e-6) {
      err = "Channel " + std::to_string(channel) + ", sample " + std::to_string(k) +
            ": expected " + std::to_string(expected) + ", got " + std::to_string((double)got[k]);
      return false;
    }
  }
  return true;
}

bool load_float_wav(const char* dirname, uint16_t channels, uint16_t bits, bool extensible,
                    uint32_t frames, std::string& err) {
  Session s(dirname);
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "float.wav";
  if (!write_bytes(wav, make_wav_float(channels, 22050, frames, bits, extensible), err)) return false;

  std::string preview;
  const std::string cmd = "x=file(\"" + wav.string() + "\")";
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Loading a float" + std::to_string(bits) + " wav failed: " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) { err = "x not found."; return false; }
  if (aux_num_channels(obj) != (int)channels) {
    err = "Expected " + std::to_string(channels) + " channels, got " +
          std::to_string(aux_num_channels(obj));
    return false;
  }
  for (int c = 0; c < (int)channels; ++c)
    if (!check_float_channel(obj, c, channels, frames, err)) return false;
  return true;
}

// float32, the format most DAWs and ffmpeg emit with -f wav -acodec pcm_f32le.
bool case_float32_mono(std::string& err) {
  return load_float_wav("float32_mono", 1, 32, false, 64, err);
}

bool case_float32_stereo_extensible(std::string& err) {
  return load_float_wav("float32_stereo", 2, 32, true, 32, err);
}

// float64 used to read 8 bytes per sample into a 4-byte-per-sample buffer.
bool case_float64_mono(std::string& err) {
  return load_float_wav("float64_mono", 1, 64, false, 64, err);
}

// ---------------------------------------------------------------------------
// Writer: wavwrite(x, file, option) for 8/16/24/32-bit int and float32.

std::vector<uint8_t> read_bytes(const fs::path& p, std::string& err) {
  std::vector<uint8_t> bytes;
  FILE* fp = fopen(p.string().c_str(), "rb");
  if (!fp) {
    err = "Cannot read " + p.string();
    return bytes;
  }
  uint8_t chunk[4096];
  size_t n;
  while ((n = fread(chunk, 1, sizeof(chunk), fp)) > 0) bytes.insert(bytes.end(), chunk, chunk + n);
  fclose(fp);
  return bytes;
}

uint16_t get_u16(const std::vector<uint8_t>& b, size_t off) {
  return (uint16_t)(b[off] | ((uint16_t)b[off + 1] << 8));
}

uint32_t get_u32(const std::vector<uint8_t>& b, size_t off) {
  return (uint32_t)(b[off] | ((uint32_t)b[off + 1] << 8) | ((uint32_t)b[off + 2] << 16) |
                    ((uint32_t)b[off + 3] << 24));
}

// Write a ramp through wavwrite with the given option, then read it back with
// file() and compare. tolerance is the worst-case quantization error of the
// format under test.
bool roundtrip(const char* dirname, const char* option, uint16_t expect_format,
               uint16_t expect_bits, double tolerance, bool stereo, std::string& err) {
  Session s(dirname);
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "out.wav";
  const int frames = 64;

  // A ramp from -1 to just under +1, as an audio object at the environment fs.
  // The right channel is its negation, so a channel swap would fail the check.
  std::string preview;
  const std::string ramp =
      "(0:" + std::to_string(frames - 1) + ")/" + std::to_string(frames) + "*2-1";
  const std::string mk = stereo ? "v=" + ramp + ", x=[audio(v); audio(-v)]"
                                : "v=" + ramp + ", x=audio(v)";
  if (aux_eval(&s.ctx, mk, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Could not build the test signal: " + preview;
    return false;
  }
  const std::string cmd =
      "wavwrite(x, \"" + wav.string() + "\", \"" + option + "\")";
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = std::string("wavwrite with option \"") + option + "\" failed: " + preview;
    return false;
  }

  std::vector<uint8_t> bytes = read_bytes(wav, err);
  if (bytes.size() < 44) { if (err.empty()) err = "Output file is too short."; return false; }
  const uint16_t channels = stereo ? 2 : 1;
  const uint16_t block_align = (uint16_t)(channels * expect_bits / 8);
  // PCM gets the canonical 44-byte header; non-PCM carries cbSize and a fact
  // chunk, which moves the data chunk 14 bytes later.
  const bool pcm = expect_format == 1;
  const size_t header_size = pcm ? 44 : 58;
  const size_t data_off = header_size - 8;
  if (bytes.size() < header_size) { err = "Output file is too short."; return false; }
  if (get_u32(bytes, 16) != (pcm ? 16u : 18u)) {
    err = "fmt chunk size: got " + std::to_string(get_u32(bytes, 16));
    return false;
  }
  if (!pcm) {
    if (memcmp(&bytes[38], "fact", 4) != 0) {
      err = "Non-PCM output is missing its fact chunk.";
      return false;
    }
    if (get_u32(bytes, 42) != 4 || get_u32(bytes, 46) != (uint32_t)frames) {
      err = "fact chunk should hold " + std::to_string(frames) + " frames, got " +
            std::to_string(get_u32(bytes, 46));
      return false;
    }
  }
  if (memcmp(&bytes[data_off], "data", 4) != 0) {
    err = "data chunk not at the expected offset " + std::to_string(data_off);
    return false;
  }
  if (get_u16(bytes, 20) != expect_format) {
    err = "audio_format: expected " + std::to_string(expect_format) + ", got " +
          std::to_string(get_u16(bytes, 20));
    return false;
  }
  if (get_u16(bytes, 34) != expect_bits) {
    err = "bits_per_sample: expected " + std::to_string(expect_bits) + ", got " +
          std::to_string(get_u16(bytes, 34));
    return false;
  }
  if (get_u16(bytes, 32) != block_align) {
    err = "block_align: expected " + std::to_string(block_align) + ", got " +
          std::to_string(get_u16(bytes, 32));
    return false;
  }
  // The stereo header bug: data size must match the bytes actually present.
  const uint32_t expect_data = (uint32_t)frames * block_align;
  if (get_u32(bytes, data_off + 4) != expect_data) {
    err = "data chunk size: expected " + std::to_string(expect_data) + ", got " +
          std::to_string(get_u32(bytes, data_off + 4));
    return false;
  }
  if (bytes.size() != header_size + expect_data) {
    err = "File size: expected " + std::to_string(header_size + expect_data) + ", got " +
          std::to_string(bytes.size());
    return false;
  }
  if (get_u32(bytes, 4) != (uint32_t)(bytes.size() - 8)) {
    err = "RIFF size: expected " + std::to_string(bytes.size() - 8) + ", got " +
          std::to_string(get_u32(bytes, 4));
    return false;
  }

  if (aux_eval(&s.ctx, "y=file(\"" + wav.string() + "\")", s.cfg, preview) !=
      (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Reading back the written file failed: " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "y");
  if (!obj) { err = "y not found."; return false; }
  if (aux_num_channels(obj) != (int)channels) {
    err = "Expected " + std::to_string(channels) + " channels, got " +
          std::to_string(aux_num_channels(obj));
    return false;
  }
  for (int c = 0; c < (int)channels; ++c) {
    std::vector<auxtype> got(frames, 0);
    const size_t n = aux_flatten_channel(obj, c, got.data(), got.size());
    if (n != (size_t)frames) {
      err = "Channel " + std::to_string(c) + ": expected " + std::to_string(frames) +
            " samples, got " + std::to_string(n);
      return false;
    }
    for (int k = 0; k < frames; ++k) {
      double expected = (double)k / frames * 2 - 1;
      if (c == 1) expected = -expected;
      if (std::abs((double)got[k] - expected) > tolerance) {
        err = std::string("Option \"") + option + "\", channel " + std::to_string(c) +
              ", sample " + std::to_string(k) + ": expected " + std::to_string(expected) +
              ", got " + std::to_string((double)got[k]);
        return false;
      }
    }
  }
  return true;
}

bool case_write_default_is_int16(std::string& err) {
  return roundtrip("write_default", "", 1, 16, 1.0 / 32768, false, err);
}
bool case_write_int8(std::string& err) {
  return roundtrip("write_int8", "8", 1, 8, 1.0 / 128, false, err);
}
bool case_write_int16(std::string& err) {
  return roundtrip("write_int16", "int16", 1, 16, 1.0 / 32768, false, err);
}
bool case_write_int24(std::string& err) {
  return roundtrip("write_int24", "24", 1, 24, 1.0 / 8388608, false, err);
}
bool case_write_int32(std::string& err) {
  // float32 is the engine's read-back container, so 32-bit int round trips only
  // to float32 precision.
  return roundtrip("write_int32", "int32", 1, 32, 1e-6, false, err);
}
bool case_write_float32(std::string& err) {
  return roundtrip("write_float32", "float", 3, 32, 1e-6, false, err);
}
bool case_write_stereo_int24(std::string& err) {
  return roundtrip("write_stereo24", "24", 1, 24, 1.0 / 8388608, true, err);
}
bool case_write_option_is_case_insensitive(std::string& err) {
  return roundtrip("write_case", "Float32", 3, 32, 1e-6, false, err);
}

// Samples beyond full scale must clamp, not wrap around to the opposite sign.
bool case_write_clips_instead_of_wrapping(std::string& err) {
  Session s("write_clip");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "clip.wav";
  std::string preview;
  const std::string mk = "x=audio([1.5 -1.5 0.5])";
  if (aux_eval(&s.ctx, mk, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Could not build the test signal: " + preview;
    return false;
  }
  if (aux_eval(&s.ctx, "wavwrite(x, \"" + wav.string() + "\", \"16\")", s.cfg, preview) !=
      (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "wavwrite failed: " + preview;
    return false;
  }
  std::vector<uint8_t> bytes = read_bytes(wav, err);
  if (bytes.size() != 44 + 6) {
    if (err.empty()) err = "Unexpected file size " + std::to_string(bytes.size());
    return false;
  }
  const int16_t s0 = (int16_t)get_u16(bytes, 44);
  const int16_t s1 = (int16_t)get_u16(bytes, 46);
  if (s0 != 32767) {
    err = "+1.5 should clamp to 32767, got " + std::to_string(s0);
    return false;
  }
  if (s1 != -32768) {
    err = "-1.5 should clamp to -32768, got " + std::to_string(s1);
    return false;
  }
  return true;
}

// A typo in the option must be an error, not a silent fallback to 16-bit.
bool case_write_rejects_unknown_option(std::string& err) {
  Session s("write_bad_option");
  if (!s.ok()) { err = s.err; return false; }
  const fs::path wav = s.dir / "bad.wav";
  std::string preview;
  const std::string mk = "x=audio((0:63)/64)";
  if (aux_eval(&s.ctx, mk, s.cfg, preview) != (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Could not build the test signal: " + preview;
    return false;
  }
  const int rc =
      aux_eval(&s.ctx, "wavwrite(x, \"" + wav.string() + "\", \"20\")", s.cfg, preview);
  if (rc == (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "An unknown format option was accepted.";
    return false;
  }
  if (preview.empty()) {
    err = "Failed without an error message.";
    return false;
  }
  // Two formats in one option string is a typo, not "last one wins".
  if (aux_eval(&s.ctx, "wavwrite(x, \"" + wav.string() + "\", \"float 16\")", s.cfg, preview) ==
      (int)auxEvalStatus::AUX_EVAL_OK) {
    err = "Conflicting format options were accepted.";
    return false;
  }
  return true;
}

}  // namespace

int main() {
  struct TestCase {
    const char* name;
    bool (*fn)(std::string&);
  };
  const TestCase tests[] = {
      {"case_valid_wav_loads", case_valid_wav_loads},
      {"case_oversized_data_chunk", case_oversized_data_chunk},
      {"case_zero_block_align", case_zero_block_align},
      {"case_pcm24_mono", case_pcm24_mono},
      {"case_pcm24_stereo_extensible", case_pcm24_stereo_extensible},
      {"case_float32_mono", case_float32_mono},
      {"case_float32_stereo_extensible", case_float32_stereo_extensible},
      {"case_float64_mono", case_float64_mono},
      {"case_write_default_is_int16", case_write_default_is_int16},
      {"case_write_int8", case_write_int8},
      {"case_write_int16", case_write_int16},
      {"case_write_int24", case_write_int24},
      {"case_write_int32", case_write_int32},
      {"case_write_float32", case_write_float32},
      {"case_write_stereo_int24", case_write_stereo_int24},
      {"case_write_option_is_case_insensitive", case_write_option_is_case_insensitive},
      {"case_write_clips_instead_of_wrapping", case_write_clips_instead_of_wrapping},
      {"case_write_rejects_unknown_option", case_write_rejects_unknown_option},
  };

  bool ok = true;
  for (const auto& tc : tests) {
    std::string err;
    if (tc.fn(err)) {
      std::cout << "[PASS] " << tc.name << "\n";
    } else {
      ok = false;
      std::cerr << "[FAIL] " << tc.name << ": " << err << "\n";
    }
  }
  return ok ? 0 : 1;
}
