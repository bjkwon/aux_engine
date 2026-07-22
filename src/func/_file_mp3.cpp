// MP3 decoding backed by dr_mp3 (vendored single-header decoder,
// public domain / MIT-0, see src/third_party/dr_mp3.h). MP3's core
// patents expired in 2017, so there is no royalty/licensing concern
// in decoding it; dr_mp3 was chosen over alternatives specifically
// for its permissive license and lack of external dependencies.
#define DR_MP3_IMPLEMENTATION
#include "../third_party/dr_mp3.h"
#include "_file_mp3.h"

using namespace std;

uint64_t mp3_read_float32(const string& fname, double beginMs, double durMs,
	Mp3Info& info, vector<float>& out, string& estr)
{
	estr.clear();
	drmp3 mp3;
	if (!drmp3_init_file(&mp3, fname.c_str(), NULL)) {
		estr = "Unable to open/parse MP3 file: " + fname;
		return 0;
	}
	info.sample_rate = mp3.sampleRate;
	info.num_channels = mp3.channels;

	uint64_t beginFrame = (uint64_t)(beginMs / 1000.0 * info.sample_rate + 0.5);
	uint64_t frames2read = (durMs < 0)
		? UINT64_MAX
		: (uint64_t)(durMs / 1000.0 * info.sample_rate + 0.5);

	if (beginFrame > 0 && !drmp3_seek_to_pcm_frame(&mp3, beginFrame)) {
		estr = "Error seeking in MP3 file: " + fname;
		drmp3_uninit(&mp3);
		return 0;
	}

	const uint64_t kChunkFrames = 4096;
	vector<float> chunk(kChunkFrames * info.num_channels);
	uint64_t framesRead = 0;
	for (;;) {
		uint64_t want = kChunkFrames;
		if (frames2read != UINT64_MAX) {
			uint64_t remaining = frames2read - framesRead;
			if (remaining == 0) break;
			want = remaining < kChunkFrames ? remaining : kChunkFrames;
		}
		uint64_t got = drmp3_read_pcm_frames_f32(&mp3, want, chunk.data());
		if (got == 0) break;
		out.insert(out.end(), chunk.begin(), chunk.begin() + got * info.num_channels);
		framesRead += got;
		if (got < want) break; // EOF
	}
	drmp3_uninit(&mp3);
	return framesRead;
}
