#pragma once

#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// The callback advances one decoder and fills interleaved float samples.
struct AudioFileReader {
    size_t frames = 0;
    int sampleRate = 0;
    int channels = 0;
    // A downloaded file must outlive its open decoder.
    std::shared_ptr<void> temporaryFile;
    std::function<size_t(size_t, std::vector<float>&)> read;
};

AudioFileReader open_mp3_reader(const std::string& path, double beginMs, bool countFrames);
bool is_local_wave_source(const std::string& path, bool waveOnly);
