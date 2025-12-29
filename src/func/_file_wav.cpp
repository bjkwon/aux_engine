// wav_parse.h/.c in one file for simplicity.
// Build:  gcc -std=c11 -Wall -Wextra -O2 wav_parse.c -o wav_parse
// Usage:  ./wav_parse input.wav

#include <stdio.h>
#include <string.h>
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
    int expected_end;
    int actual_end;
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
            headersize = actual_end;
            out.data_offset = tell_pos(fp);
            out.data_size = chunk_size;
            // Skip audio data for scanning; caller may not want this, but
            // we stop after finding data anyway.
            if (!skip_bytes(fp, chunk_size)) return 0;
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
    return headersize + 8; // adding 8 to the position where the data chunk is found
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
    uint64_t id=0;
    size_t res;
    switch (info.audio_format) {
        case 1:
            if (info.bits_per_sample == 8) {
                //8bit unsigned
                std::vector<uint8_t> temp_out(frames2read * info.num_channels, 0);
                res = fread(temp_out.data(), info.block_align, frames2read, fp);
                for (auto &v : out) {v = (float)temp_out[id++] / 128; v -= 1.f;}
            } else if (info.bits_per_sample == 16) {
                std::vector<int16_t> temp_out(frames2read* info.num_channels, 0);
                res = fread(temp_out.data(), info.block_align, frames2read, fp);
                for (auto &v : out) v = (float)temp_out[id++] / 32768;
            } else if (info.bits_per_sample == 24) { 
            } else {
                estr = "Unknown PCM 1.";
                return 0;                
            }
            break;
        case 0xFFFE:
            if (is_pcm_guid(info.subformat_guid) && info.bits_per_sample == 32) {
                    std::vector<int32_t> temp_out(frames2read * info.num_channels, 0);
                    res = fread(temp_out.data(), info.block_align, frames2read, fp);
                    for (auto &v : out) v = (float)temp_out[id++] / 2147483648;
            } else if (is_float_guid(info.subformat_guid)) {
                res = fread(out.data(), info.block_align, frames2read, fp);
            } else {
                estr = "Not supported.";
                return 0;                
            }
        break;
        case 3:
            // this part was not tested
            res = fread(out.data(), info.block_align, frames2read, fp);
        break;
    }
    return res;
}

void make_wav_header(char* buffer, const WavInfo& info, size_t nSamples)
{
    buffer[0] = 'R';
    buffer[1] = 'I';
    buffer[2] = 'F';
    buffer[3] = 'F';
    // from 4 to 7: riff size (file_size_minus_8)
    *(uint32_t*)(buffer + 4) = info.riff_size;
    buffer[8] = 'W';
    buffer[9] = 'A';
    buffer[10] = 'V';
    buffer[11] = 'E';
    buffer[12] = 'f';
    buffer[13] = 'm';
    buffer[14] = 't';
    buffer[15] = ' ';
    // from 16 to 19: fmt_chunk_size (16 in most cases)
    *(uint32_t*)(buffer + 16) = 16;
    *(uint16_t*)(buffer + 20) = info.audio_format;
    *(uint16_t*)(buffer + 22) = info.num_channels;
    *(uint32_t*)(buffer + 24) = info.sample_rate;
    *(uint32_t*)(buffer + 28) = info.byte_rate;
    *(uint16_t*)(buffer + 32) = info.block_align;
    *(uint16_t*)(buffer + 34) = info.bits_per_sample;
    buffer[36] = 'd';
    buffer[37] = 'a';
    buffer[38] = 't';
    buffer[39] = 'a';
    *(uint32_t*)(buffer + 40) = nSamples * info.block_align * info.num_channels;
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
