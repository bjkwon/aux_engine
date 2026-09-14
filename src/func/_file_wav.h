#include <stdint.h>
#include <string>


typedef struct {
    // From fmt chunk (canonical)
    uint16_t audio_format;        // 1=PCM, 3=IEEE float, 0xFFFE=extensible, others possible
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;

    // Extensible extras (valid only if audio_format==0xFFFE and present)
    uint16_t valid_bits_per_sample;
    uint32_t channel_mask;
    uint8_t  subformat_guid[16];  // GUID bytes as stored in file

    // Data chunk location
    uint64_t data_offset;         // file offset where audio data starts
    uint32_t data_size;           // bytes of audio data (can be 0 for streaming/unknown in some files)

    // RIFF size (file_size_minus_8)
    uint32_t riff_size;
} WavInfo;

// Sample formats the wav writer can produce. 8-bit is unsigned (as the WAV spec
// requires at that depth); every other integer depth is signed little-endian.
typedef enum {
    WAVFMT_UNKNOWN = 0,
    WAVFMT_INT8,
    WAVFMT_INT16,
    WAVFMT_INT24,
    WAVFMT_INT32,
    WAVFMT_FLOAT32,
} WavSampleFormat;

int wav_read_header(const std::string& fname, WavInfo& out, std::string& estr);
uint64_t wav_read_float32(FILE* fp, uint64_t frames2read, const WavInfo& info, std::vector<float>& out, std::string& estr);
// Largest header make_wav_header can emit (58 bytes today, rounded up).
#define WAV_HEADER_MAX_BYTES 64
// Fills buffer (at least WAV_HEADER_MAX_BYTES) and returns the header size:
// 44 for PCM, more for non-PCM formats, which carry a fact chunk.
size_t make_wav_header(char* buffer, const WavInfo& info, size_t nSamples);

// Map one option token ("16", "int16", "float", ...) to a format.
// Case-insensitive; returns WAVFMT_UNKNOWN for anything unrecognized.
WavSampleFormat wav_format_from_token(const std::string& token);
// Human-readable list of the tokens above, for error messages.
const char* wav_format_tokens();
uint16_t wav_format_bits(WavSampleFormat fmt);
uint16_t wav_format_code(WavSampleFormat fmt); // fmt chunk audio_format: 1=PCM, 3=IEEE float
// Quantize count interleaved samples (nominally in [-1,1]) to fmt and write them.
// Integer formats clamp out-of-range samples; float32 stores them as they are.
// Returns the number of samples written (== count on success).
size_t wav_write_samples(FILE* fp, WavSampleFormat fmt, const double* samples, size_t count);
