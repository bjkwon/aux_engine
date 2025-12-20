#ifndef CSIGNALS_MP3_AIFF
typedef struct {
	int fs;
	size_t length;
	uint8_t nChans;
	uint8_t floatbuf; // 0 for double; 1 for float
	union
	{
		double* buf_l;
		float* fbuf_l;
	};
	union
	{
		double* buf_r;
		float* fbuf_r;
	};
} csignals_mp3_aiff;
#define CSIGNALS_MP3_AIFF
#endif