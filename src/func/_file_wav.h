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

int wav_read_header(const std::string& fname, WavInfo& out, std::string& estr);
uint64_t wav_read_float32(FILE* fp, uint64_t frames2read, const WavInfo& info, std::vector<float>& out, std::string& estr);
void make_wav_header(char* buffer, const WavInfo& info, size_t nSamples);
