#include <iostream>
#include <sstream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "portaudio.h"
#include <auxe/auxe.h>

int fs;

using namespace std;

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define FRAMES_PER_BUFFER 1024 // With fs=44100 Hz, tone(500,1000).ramp(100) sounds OK with 256. If this was 4096, it sounds choppy at the end

class audioPlay {
public:
	deque<unique_ptr<audioPlay>> ondeck;
	auxtype* buf1;
	auxtype* buf2;
	uint64_t nSamples;
	double currenttime; // in sec; constantly changing
	uint64_t currentID; // buffer index offset from the beginning; constantly changing
	int repeatCount;
	PaStream* stream;
	int deckID;
	void fill_buffer(unsigned long bufSize, float* playBuffer, uint64_t offset)
	{
		auto lastindex = min(offset + bufSize, nSamples);
		for (unsigned long k = offset; k < lastindex; k++) {
			*playBuffer++ = (float)buf1[k];  /* left */
			if (buf2)
				*playBuffer++ = (float)buf2[k];  /* right */
		}
	}
	// In the constructor, AuxObj object and the framebuffer size are specified by the user
	// timeblock is derived.
	audioPlay(int _fs, auxtype* _buf1, auxtype* _buf2, uint64_t _nSamples, int _lenblock, int _repCount) {
		fs = _fs;
		buf1 = _buf1;
		buf2 = _buf2;
		nSamples = _nSamples;
		lenblock = _lenblock;
		timeblock = (double)lenblock / fs;
		currenttime = 0;
		currentID = 0;
		repeatCount = _repCount;
		deckID = 0;
	};
	virtual ~audioPlay() {};
	int get_lenblock() { return lenblock; };
private:
	int lenblock; // not changing after the constructor
	double timeblock; // in sec; determined in the constructor and constant
};

map<PaStream*, bool> playdone;
map<uintptr_t, PaStream*> streams;
map<PaStream*, audioPlay*> pmods; // play modules

static int playCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	audioPlay* data = (audioPlay*)userData;
	auto deckID = data->deckID;
	if (deckID > 0)
		data = data->ondeck.front().get();

	pmods[data->stream] = data;

	data->fill_buffer(framesPerBuffer, (float*)outputBuffer, data->currentID);
	if (data->currentID > data->nSamples) {
		data->repeatCount--;
		if (data->repeatCount > 0) {
			data->currentID = 0;
			data->currenttime = 0.;
			data->fill_buffer(framesPerBuffer, (float*)outputBuffer, data->currentID);
			data->currentID += data->get_lenblock();
			data->currenttime = (double)data->currentID / fs;
			return paContinue;
		}
		else
		{
			if (deckID > 0)
			{
				((audioPlay*)userData)->ondeck.pop_front();
				((audioPlay*)userData)->deckID--;
			}
			if (((audioPlay*)userData)->deckID < ((audioPlay*)userData)->ondeck.size()) {
				((audioPlay*)userData)->deckID++;
				return paContinue;
			}
			else {
				playdone[data->stream] = true;
				auto it = pmods.find(data);
				if (it != pmods.end())
					pmods.erase(it);
				return paComplete;
			};
		}
	}
	else {
		data->currentID += data->get_lenblock();
		data->currenttime = (double)data->currentID / fs;
	}
	return paContinue;
}

void playfinishcb(void* p)
{
	audioPlay* data = (audioPlay*)p;
}

void playthread(auxContext* ctx, const AuxObj& obj, uintptr_t handle, int rep, void* petc, const PaStreamParameters& outputParameters)
{
	PaStream* stream;
	PaError err;
	auxtype* buf1;
	auxtype* buf2 = NULL;
	AuxSignal seg;
	aux_get_segment(obj, 0, 0, seg);
	buf1 = seg.buf;
	if (outputParameters.channelCount > 1) {
		aux_get_segment(obj, 1, 0, seg);
		buf2 = seg.buf;
	}

	audioPlay pm(fs, buf1, buf2, seg.nSamples, FRAMES_PER_BUFFER, rep);
	mutex p;
	mutex mtx;
	unique_lock<mutex> lock1(mtx);
	err = Pa_OpenStream(&stream, NULL /*no input*/, &outputParameters, fs, FRAMES_PER_BUFFER, paClipOff, playCallback, &pm);
	if (err == paNoError) {
		streams[handle] = stream;
		playdone[stream] = false;
		pm.stream = stream;
		Pa_SetStreamFinishedCallback(stream, playfinishcb);
		err = Pa_StartStream(stream);
		while (!playdone[stream]) {
			Pa_Sleep(100);
		};
		err = Pa_StopStream(stream);
		err = Pa_CloseStream(stream);
	}
	else {
		strcpy((char*)petc, "error");
	}
}

int play_audio(auxContext* ctx, const AuxObj& obj)
{
	fs = aux_get_fs(ctx);
	PaStreamParameters param;
	param.device = Pa_GetDefaultOutputDevice();
	if (param.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return 1;
	}
	param.channelCount = aux_num_channels(obj);
	param.sampleFormat = paFloat32; /* 32 bit floating point output */
	param.suggestedLatency = Pa_GetDeviceInfo(param.device)->defaultLowOutputLatency;
	param.hostApiSpecificStreamInfo = NULL;
	auto repeat = 1;
	auto devid = 1;
	char etc[256] = {};
	thread t1(playthread, ctx, obj, (uintptr_t)0 /*phandle->value()*/, (int)repeat, etc, param);
	t1.detach();
	return 0;
}
 