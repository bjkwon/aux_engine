// MP3 decoding backed by dr_mp3 (vendored single-header decoder,
// public domain / MIT-0, see src/third_party/dr_mp3.h). MP3's core
// patents expired in 2017, so there is no royalty/licensing concern
// in decoding it; dr_mp3 was chosen over alternatives specifically
// for its permissive license and lack of external dependencies.
#define DR_MP3_IMPLEMENTATION
#include "../third_party/dr_mp3.h"
#include "_file_mp3.h"
#include "audio_file_reader.h"
#include <stdexcept>

using namespace std;

AudioFileReader open_mp3_reader(const string& path, double beginMs, bool countFrames)
{
	auto initialized = std::make_unique<drmp3>();
	if (!drmp3_init_file(initialized.get(), path.c_str(), nullptr))
		throw runtime_error("Unable to open/parse MP3 file: " + path);
	auto decoder = shared_ptr<drmp3>(initialized.release(), [](drmp3* p) {
		drmp3_uninit(p);
		delete p;
	});
	AudioFileReader reader;
	reader.sampleRate = decoder->sampleRate;
	reader.channels = decoder->channels;
	if (countFrames) reader.frames = drmp3_get_pcm_frame_count(decoder.get());
	uint64_t start = static_cast<uint64_t>(beginMs / 1000.0 * reader.sampleRate + 0.5);
	if (start && !drmp3_seek_to_pcm_frame(decoder.get(), start))
		throw runtime_error("Error seeking in MP3 file: " + path);
	reader.read = [decoder](size_t frames, vector<float>& out) {
		out.resize(frames * decoder->channels);
		size_t got = drmp3_read_pcm_frames_f32(decoder.get(), frames, out.data());
		out.resize(got * decoder->channels);
		return got;
	};
	return reader;
}

uint64_t mp3_read_float32(const string& path, double beginMs, double durMs,
	Mp3Info& info, vector<float>& out, string& estr)
{
	estr.clear();
	try {
		auto reader = open_mp3_reader(path, beginMs, false);
		info.sample_rate = reader.sampleRate;
		info.num_channels = reader.channels;
		uint64_t limit = durMs < 0 ? UINT64_MAX : uint64_t(durMs / 1000.0 * info.sample_rate + 0.5);
		uint64_t done = 0;
		vector<float> chunk;
		while (done < limit) {
			size_t want = std::min<uint64_t>(4096, limit - done);
			size_t got = reader.read(want, chunk);
			out.insert(out.end(), chunk.begin(), chunk.end());
			done += got;
			if (got < want) break;
		}
		return done;
	} catch (const std::exception& e) {
		estr = e.what();
		return 0;
	}
}
