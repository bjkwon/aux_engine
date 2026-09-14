// wav_parse.h/.c in one file for simplicity.
// Build:  gcc -std=c11 -Wall -Wextra -O2 wav_parse.c -o wav_parse
// Usage:  ./wav_parse input.wav

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <vector>
#include "_file_wav.h"

static int read_u16_le(FILE* f, uint16_t* out)
{
    uint8_t b[2];
    if (fread(b, 1, 2, f) != 2) return 0;
    *out = (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
    return 1;
}

static int read_u32_le(FILE* f, uint32_t* out)
{
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4) return 0;
    *out = (uint32_t)(b[0] |
                      ((uint32_t)b[1] << 8) |
                      ((uint32_t)b[2] << 16) |
                      ((uint32_t)b[3] << 24));
    return 1;
}

static int read_exact(FILE* f, void* dst, size_t n)
{
    return fread(dst, 1, n, f) == n;
}

static int skip_bytes(FILE* f, uint64_t n)
{
    // fseek takes long; for portability, loop in chunks if needed.
    // Most WAV files are <2GB so fseek is fine, but we'll be safe-ish.
#if defined(_WIN32)
    // _fseeki64 is safer on Windows for large files.
    return _fseeki64(f, (int64_t)n, SEEK_CUR) == 0;
#else
    // fseeko uses off_t (often 64-bit).
    return fseeko(f, (off_t)n, SEEK_CUR) == 0;
#endif
}

static uint64_t tell_pos(FILE* f)
{
#if defined(_WIN32)
    return (uint64_t)_ftelli64(f);
#else
    return (uint64_t)ftello(f);
#endif
}

static uint64_t file_size_of(FILE* f)
{
    uint64_t here = tell_pos(f);
#if defined(_WIN32)
    if (_fseeki64(f, 0, SEEK_END) != 0) return 0;
#else
    if (fseeko(f, 0, SEEK_END) != 0) return 0;
#endif
    uint64_t size = tell_pos(f);
#if defined(_WIN32)
    _fseeki64(f, (int64_t)here, SEEK_SET);
#else
    fseeko(f, (off_t)here, SEEK_SET);
#endif
    return size;
}

static int is_pcm_guid(const uint8_t g[16])
{
    // {00000001-0000-0010-8000-00AA00389B71}
    static const uint8_t pcm_guid[16] = {
        0x01,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00,
        0x80,0x00, 0x00,0xAA,0x00,0x38,0x9B,0x71
    };
    return memcmp(g, pcm_guid, 16) == 0;
}

static int is_float_guid(const uint8_t g[16])
{
    // {00000003-0000-0010-8000-00AA00389B71}
    static const uint8_t float_guid[16] = {
        0x03,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00,
        0x80,0x00, 0x00,0xAA,0x00,0x38,0x9B,0x71
    };
    return memcmp(g, float_guid, 16) == 0;
}

#define FREAD_CHECK_CHAR(FID,BUFFER,COUNT,NAME) { \
 if (fread(BUFFER, 1, COUNT, FID) != COUNT) { estr= fname + "--Error in fread " + NAME; return 0; }}
#define FREAD_CHECK_U32(FID,VAR,NAME) { \
 if (!read_u32_le(fp, &VAR)) { estr= fname + "--Error in fread " + NAME; return 0; }}
#define FREAD_CHECK_U16(FID,VAR,NAME) { \
 if (!read_u16_le(fp, &VAR)) { estr= fname + "--Error in fread " + NAME; return 0; }}

static int parse_fmt_chunk(FILE* fp, uint32_t chunk_size, WavInfo& info)
{
    // Minimum WAVEFORMAT (PCM) is 16 bytes.
    if (chunk_size < 16) return 0;

    uint64_t start = tell_pos(fp);

    if (!read_u16_le(fp, &info.audio_format)) return 0;
    if (!read_u16_le(fp, &info.num_channels)) return 0;
    if (!read_u32_le(fp, &info.sample_rate)) return 0;
    if (!read_u32_le(fp, &info.byte_rate)) return 0;
    if (!read_u16_le(fp, &info.block_align)) return 0;
    if (!read_u16_le(fp, &info.bits_per_sample)) return 0;

    info.valid_bits_per_sample = 0;
    info.channel_mask = 0;
    memset(info.subformat_guid, 0, sizeof(info.subformat_guid));

    uint32_t consumed = 16;

    // If extended fmt exists, next field is cbSize (2 bytes) for WAVEFORMATEX.
    if (chunk_size >= 18) {
        uint16_t cbSize = 0;
        if (!read_u16_le(fp, &cbSize)) return 0;
        consumed += 2;

        // WAVE_FORMAT_EXTENSIBLE (0xFFFE) typically has cbSize==22
        // and then: validBitsPerSample (2), channelMask(4), subFormat GUID(16)
        if (info.audio_format == 0xFFFE) {
            if (chunk_size < 18 + 22) {
                // Extensible but not enough bytes.
                return 0;
            }
            if (!read_u16_le(fp, &info.valid_bits_per_sample)) return 0;
            if (!read_u32_le(fp, &info.channel_mask)) return 0;
            if (!read_exact(fp, info.subformat_guid, 16)) return 0;
            consumed += 2 + 4 + 16;
        } else {
            // Non-extensible; skip any extra fmt bytes beyond what we've read.
            // Some compressed formats include extra bytes.
            (void)cbSize;
        }
    }

    // Skip any remaining bytes in fmt chunk we didn't consume.
    uint64_t now = tell_pos(fp);
    uint64_t read_bytes = now - start;
    if (read_bytes > chunk_size) return 0;
    uint64_t remaining = (uint64_t)chunk_size - read_bytes;
    if (remaining > 0) {
        if (!skip_bytes(fp, remaining)) return 0;
    }
    return 1;
}


// Returns the number of bytes read on success. 0 for all errors.
int wav_read_header(const std::string& fname, WavInfo& out, std::string& estr)
{
    int headersize = 0;
    FILE* fp = fopen(fname.c_str(), "rb");
    if (!fp) {
        estr = "File not found or cannot be opened: ";
        estr += fname;
        return 0;
    }
    memset(&out, 0, sizeof(out));
    uint64_t start = tell_pos(fp);

    // RIFF header: "RIFF" <u32 size> "WAVE"
    char riff_id[4];
    FREAD_CHECK_CHAR(fp,riff_id,4,"RIFF");
    if (memcmp(riff_id, "RIFF", 4) != 0) {
        estr = fname + "--Error: Not starting with RIFF.";
        return 0;
    }
    FREAD_CHECK_U32(fp, out.riff_size, "out.riff_size");

    char wave_id[4];
    FREAD_CHECK_CHAR(fp,wave_id,4,"WAVE");
    if (memcmp(wave_id, "WAVE", 4) != 0) {
        estr = fname + "--Error: WAVE expected after RIFF.";
        return 0;
    }
    int found_fmt = 0;
    int found_data = 0;
    // Scan chunks until we find fmt and data (or EOF).
    uint64_t expected_end = 0;
    uint64_t actual_end = 0;
    const uint64_t file_size = file_size_of(fp);
    while (!found_data) {
        char chunk_id[4];
        uint32_t chunk_size;
        FREAD_CHECK_CHAR(fp,chunk_id, 4, "chunk_id"); // EOF
        FREAD_CHECK_U32(fp, chunk_size, "chunk_size"); // malformed

        uint64_t chunk_data_pos = tell_pos(fp);

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            if (!parse_fmt_chunk(fp, chunk_size, out)) {
                estr = fname + "--Error while reading fmt chunk.";
                return 0;
            }
            found_fmt = 1;
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            out.data_offset = chunk_data_pos;
            headersize = (int)(chunk_data_pos - 8); // where the data chunk id begins
            // A corrupt, truncated or still-being-written file can declare far more
            // data than it actually holds. Trust the file size, not the header;
            // otherwise the caller allocates a buffer for data that isn't there.
            uint64_t avail = file_size > chunk_data_pos ? file_size - chunk_data_pos : 0;
            out.data_size = (uint32_t)(chunk_size < avail ? chunk_size : avail);
            // Skip audio data for scanning; caller may not want this, but
            // we stop after finding data anyway.
            if (!skip_bytes(fp, out.data_size)) return 0;
            found_data = 1;
        } else {
            // Skip unknown chunk payload
            if (!skip_bytes(fp, chunk_size)) return 0;
        }

        // RIFF chunks are word-aligned. If chunk_size is odd, there is 1 pad byte.
        if (chunk_size & 1) {
            if (!skip_bytes(fp, 1)) return 0;
        }

        // Safety: ensure we advanced at least the chunk size (avoid infinite loops)
        expected_end = chunk_data_pos + chunk_size + (chunk_size & 1);
        actual_end = tell_pos(fp);
        if (actual_end < expected_end) {
            // Some parsing function didn't move correctly; try to resync.
            if (!skip_bytes(fp, expected_end - actual_end)) return 0;
        }
    }
    int res = fclose(fp);
    if (res) {
        estr = fname + "--Error: Cannot close file.";
        return 0;
    }
    // Without a usable fmt chunk every downstream size computation is garbage.
    if (!found_fmt || out.block_align == 0 || out.num_channels == 0 ||
        out.bits_per_sample == 0 || out.sample_rate == 0) {
        estr = fname + "--Error: Missing or invalid fmt chunk.";
        return 0;
    }
    return headersize + 8; // adding 8 to the position where the data chunk is found
}

// Decode integer PCM (8/16/24/32 bit, little-endian) into out, which must be
// sized frames2read*num_channels beforehand. Returns the number of frames read.
static size_t read_pcm_int(FILE* fp, uint64_t frames2read, const WavInfo& info, std::vector<float>& out, std::string& estr)
{
    size_t res = 0;
    uint64_t id = 0;
    switch (info.bits_per_sample) {
    case 8: {
        //8bit unsigned
        std::vector<uint8_t> temp_out(frames2read * info.num_channels, 0);
        res = fread(temp_out.data(), info.block_align, frames2read, fp);
        for (auto& v : out) { v = (float)temp_out[id++] / 128; v -= 1.f; }
        break;
    }
    case 16: {
        std::vector<int16_t> temp_out(frames2read * info.num_channels, 0);
        res = fread(temp_out.data(), info.block_align, frames2read, fp);
        for (auto& v : out) v = (float)temp_out[id++] / 32768;
        break;
    }
    case 24: {
        // 24-bit samples are packed three bytes each, little-endian, no padding.
        // Sign-extend into int32 before scaling.
        std::vector<uint8_t> temp_out((size_t)frames2read * info.block_align, 0);
        res = fread(temp_out.data(), info.block_align, frames2read, fp);
        for (auto& v : out) {
            const uint8_t* p = temp_out.data() + id * 3;
            int32_t s = (int32_t)((uint32_t)p[0] << 8 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 24);
            s >>= 8; // arithmetic shift back down, sign preserved
            v = (float)s / 8388608;
            ++id;
        }
        break;
    }
    case 32: {
        std::vector<int32_t> temp_out(frames2read * info.num_channels, 0);
        res = fread(temp_out.data(), info.block_align, frames2read, fp);
        for (auto& v : out) v = (float)temp_out[id++] / 2147483648;
        break;
    }
    default:
        estr = "Unsupported PCM bit depth.";
        return 0;
    }
    return res;
}

// Decode IEEE float samples (32 or 64 bit) into out, which must be sized
// frames2read*num_channels beforehand. Returns the number of frames read.
static size_t read_pcm_float(FILE* fp, uint64_t frames2read, const WavInfo& info, std::vector<float>& out, std::string& estr)
{
    switch (info.bits_per_sample) {
    case 32:
        // float32 is the container type already; read straight into out.
        return fread(out.data(), info.block_align, frames2read, fp);
    case 64: {
        // Never read doubles into out directly: it holds half the bytes per sample.
        std::vector<double> temp_out(frames2read * info.num_channels, 0.0);
        size_t res = fread(temp_out.data(), info.block_align, frames2read, fp);
        uint64_t id = 0;
        for (auto& v : out) v = (float)temp_out[id++];
        return res;
    }
    default:
        estr = "Unsupported IEEE float bit depth.";
        return 0;
    }
}

// Read from the data block of a wave file and put them to a float container output
uint64_t wav_read_float32(FILE* fp, uint64_t frames2read, const WavInfo& info, std::vector<float>& out, std::string& estr)
{
    estr = "";
    // check integrity of the wav data format
    auto bytes_per_sample = info.bits_per_sample / 8;
    auto expected_block_align = info.num_channels * bytes_per_sample;
    if (info.bits_per_sample % 8 != 0 || info.block_align != info.num_channels * bytes_per_sample) {
        estr = "Invalid PCM or Header inconsistency.";
        return 0;
    }
    out.resize(frames2read*info.num_channels);
    size_t res = 0; // must be initialized: callers size buffers with the return value
    switch (info.audio_format) {
        case 1:
            res = read_pcm_int(fp, frames2read, info, out, estr);
            if (!estr.empty()) return 0;
            break;
        case 0xFFFE:
            if (is_pcm_guid(info.subformat_guid)) {
                res = read_pcm_int(fp, frames2read, info, out, estr);
                if (!estr.empty()) return 0;
            } else if (is_float_guid(info.subformat_guid)) {
                res = read_pcm_float(fp, frames2read, info, out, estr);
                if (!estr.empty()) return 0;
            } else {
                estr = "Not supported.";
                return 0;
            }
        break;
        case 3:
            res = read_pcm_float(fp, frames2read, info, out, estr);
            if (!estr.empty()) return 0;
        break;
        default:
            estr = "Unsupported wav audio format.";
            return 0;
    }
    if (res < frames2read)
        out.resize(res * info.num_channels); // short read; don't hand back uninitialized frames
    return res;
}

// Little-endian field writers. memcpy rather than a cast through the buffer:
// the fact chunk pushes later fields off their natural alignment.
static inline void put_id(char* buffer, size_t off, const char* id)
{
    memcpy(buffer + off, id, 4);
}

static inline void put_u16(char* buffer, size_t off, uint16_t v)
{
    uint8_t b[2] = { (uint8_t)(v & 0xFF), (uint8_t)(v >> 8) };
    memcpy(buffer + off, b, 2);
}

static inline void put_u32(char* buffer, size_t off, uint32_t v)
{
    uint8_t b[4];
    for (int k = 0; k < 4; k++) b[k] = (uint8_t)((v >> (8 * k)) & 0xFF);
    memcpy(buffer + off, b, 4);
}

// Writes the header into buffer (at least WAV_HEADER_MAX_BYTES) and returns its
// size, which depends on the format: canonical 44 bytes for PCM, and for
// non-PCM the WAVEFORMATEX cbSize field plus the fact chunk the spec calls for.
size_t make_wav_header(char* buffer, const WavInfo& info, size_t nSamples)
{
    const bool pcm = info.audio_format == 1;
    const uint32_t fmt_size = pcm ? 16u : 18u;
    // block_align already covers every channel of one frame; don't multiply by
    // num_channels again or stereo files claim twice the data they hold.
    const uint32_t data_size = (uint32_t)(nSamples * info.block_align);
    const size_t header_size = pcm ? 44 : 58;

    put_id(buffer, 0, "RIFF");
    put_u32(buffer, 4, (uint32_t)(header_size - 8 + data_size)); // file_size_minus_8
    put_id(buffer, 8, "WAVE");
    put_id(buffer, 12, "fmt ");
    put_u32(buffer, 16, fmt_size);
    put_u16(buffer, 20, info.audio_format);
    put_u16(buffer, 22, info.num_channels);
    put_u32(buffer, 24, info.sample_rate);
    put_u32(buffer, 28, info.byte_rate);
    put_u16(buffer, 32, info.block_align);
    put_u16(buffer, 34, info.bits_per_sample);
    size_t off = 36;
    if (!pcm) {
        put_u16(buffer, off, 0); // cbSize: no extra fmt bytes follow
        off += 2;
        // fact holds the sample count per channel. Readers of non-PCM data use
        // it instead of deriving a frame count from the data size.
        put_id(buffer, off, "fact");
        put_u32(buffer, off + 4, 4);
        put_u32(buffer, off + 8, (uint32_t)nSamples);
        off += 12;
    }
    put_id(buffer, off, "data");
    put_u32(buffer, off + 4, data_size);
    return off + 8;
}

WavSampleFormat wav_format_from_token(const std::string& token)
{
    std::string t;
    for (char c : token) t += (char)tolower((unsigned char)c);
    if (t == "8" || t == "int8" || t == "uint8") return WAVFMT_INT8;
    if (t == "16" || t == "int16") return WAVFMT_INT16;
    if (t == "24" || t == "int24") return WAVFMT_INT24;
    if (t == "32" || t == "int32") return WAVFMT_INT32;
    if (t == "float" || t == "float32") return WAVFMT_FLOAT32;
    return WAVFMT_UNKNOWN;
}

const char* wav_format_tokens()
{
    return "8 (int8), 16 (int16), 24 (int24), 32 (int32), float (float32)";
}

uint16_t wav_format_bits(WavSampleFormat fmt)
{
    switch (fmt) {
    case WAVFMT_INT8: return 8;
    case WAVFMT_INT16: return 16;
    case WAVFMT_INT24: return 24;
    case WAVFMT_INT32: return 32;
    case WAVFMT_FLOAT32: return 32;
    default: return 0;
    }
}

uint16_t wav_format_code(WavSampleFormat fmt)
{
    return fmt == WAVFMT_FLOAT32 ? 3 : 1;
}

// Scale and round one sample into an integer range, clamping instead of
// wrapping around: a sample at or past +1.0 must come out as full scale, not as
// the most negative value.
static inline int64_t quantize(double v, double scale, int64_t lo, int64_t hi)
{
    double s = v * scale;
    if (s != s) return 0; // NaN: write silence rather than a full-scale click
    s = (s >= 0.0) ? floor(s + 0.5) : ceil(s - 0.5);
    if (s < (double)lo) return lo;
    if (s > (double)hi) return hi;
    return (int64_t)s;
}

// The scale factors mirror the divisors in read_pcm_int(), so a value that came
// from a file of the same depth writes back to the same bytes.
size_t wav_write_samples(FILE* fp, WavSampleFormat fmt, const double* samples, size_t count)
{
    if (count == 0) return 0;
    std::vector<uint8_t> out;
    switch (fmt) {
    case WAVFMT_INT8:
        out.resize(count);
        for (size_t k = 0; k < count; k++)
            out[k] = (uint8_t)(quantize(samples[k], 128.0, -128, 127) + 128);
        break;
    case WAVFMT_INT16:
        out.resize(count * 2);
        for (size_t k = 0; k < count; k++) {
            int32_t s = (int32_t)quantize(samples[k], 32768.0, -32768, 32767);
            out[2 * k] = (uint8_t)(s & 0xFF);
            out[2 * k + 1] = (uint8_t)((s >> 8) & 0xFF);
        }
        break;
    case WAVFMT_INT24:
        out.resize(count * 3);
        for (size_t k = 0; k < count; k++) {
            int32_t s = (int32_t)quantize(samples[k], 8388608.0, -8388608, 8388607);
            out[3 * k] = (uint8_t)(s & 0xFF);
            out[3 * k + 1] = (uint8_t)((s >> 8) & 0xFF);
            out[3 * k + 2] = (uint8_t)((s >> 16) & 0xFF);
        }
        break;
    case WAVFMT_INT32:
        out.resize(count * 4);
        for (size_t k = 0; k < count; k++) {
            int32_t s = (int32_t)quantize(samples[k], 2147483648.0, -2147483648LL, 2147483647LL);
            for (int b = 0; b < 4; b++) out[4 * k + b] = (uint8_t)((s >> (8 * b)) & 0xFF);
        }
        break;
    case WAVFMT_FLOAT32: {
        out.resize(count * 4);
        for (size_t k = 0; k < count; k++) {
            float f = (float)samples[k];
            uint32_t bits;
            memcpy(&bits, &f, 4);
            for (int b = 0; b < 4; b++) out[4 * k + b] = (uint8_t)((bits >> (8 * b)) & 0xFF);
        }
        break;
    }
    default:
        return 0;
    }
    const size_t bytes_per_sample = out.size() / count;
    return fwrite(out.data(), bytes_per_sample, count, fp);
}

// Helper: pretty print some common format codes
static const char* wav_format_name(uint16_t fmt)
{
    switch (fmt) {
        case 1:      return "PCM";
        case 3:      return "IEEE_FLOAT";
        case 0xFFFE: return "WAVE_FORMAT_EXTENSIBLE";
        default:     return "OTHER";
    }
}

static void print_guid(const uint8_t g[16])
{
    // GUID in WAVEFORMATEXTENSIBLE is stored little-endian for first 3 fields.
    // We'll print raw bytes in canonical GUID order by applying the endian swaps.
    uint32_t d1 = (uint32_t)g[0] | ((uint32_t)g[1] << 8) | ((uint32_t)g[2] << 16) | ((uint32_t)g[3] << 24);
    uint16_t d2 = (uint16_t)(g[4] | (g[5] << 8));
    uint16_t d3 = (uint16_t)(g[6] | (g[7] << 8));
    printf("%08" PRIx32 "-%04" PRIx16 "-%04" PRIx16 "-", d1, d2, d3);
    for (int i = 8; i < 10; i++) printf("%02x", g[i]);
    printf("-");
    for (int i = 10; i < 16; i++) printf("%02x", g[i]);
}

#ifdef WAV_PARSE_MAIN
int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.wav>\n", argv[0]);
        return 2;
    }

    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 2;
    }

    WavInfo info;
    int ok = wav_read_header(f, &info);
    fclose(f);

    if (!ok) {
        fprintf(stderr, "Failed to parse WAV header.\n");
        return 1;
    }

    printf("RIFF size (file-8): %u\n", info.riff_size);
    printf("Format: %s (0x%04x)\n", wav_format_name(info.audio_format), info.audio_format);
    printf("Channels: %u\n", info.num_channels);
    printf("Sample rate: %u\n", info.sample_rate);
    printf("Byte rate: %u\n", info.byte_rate);
    printf("Block align: %u\n", info.block_align);
    printf("Bits per sample: %u\n", info.bits_per_sample);

    if (info.audio_format == 0xFFFE) {
        printf("Valid bits per sample: %u\n", info.valid_bits_per_sample);
        printf("Channel mask: 0x%08x\n", info.channel_mask);
        printf("SubFormat GUID: ");
        print_guid(info.subformat_guid);
        printf("\n");
    }

    printf("Data offset: %" PRIu64 "\n", info.data_offset);
    printf("Data size: %u\n", info.data_size);
    return 0;
}
#endif
