#pragma once
#include <cstdint>
#include <string>
#include <vector>

typedef struct {
    uint32_t sample_rate;
    uint32_t num_channels;
} Mp3Info;

// Decodes an MP3 file to interleaved float32 PCM, starting at beginMs and
// covering durMs (durMs < 0 reads to end-of-file). Returns the number of
// frames decoded; on error, returns 0 and estr is set to a non-empty message.
uint64_t mp3_read_float32(const std::string& fname, double beginMs, double durMs,
	Mp3Info& info, std::vector<float>& out, std::string& estr);
