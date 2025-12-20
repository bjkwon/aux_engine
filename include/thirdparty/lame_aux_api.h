#include "csignals_mp3_aiff.h"

#if defined(__cplusplus)
extern "C" {
#endif
int write_mp3(csignals_mp3_aiff* px, const char* filename, char* errstr);
int read_mp3(csignals_mp3_aiff* px, const char* filename, char* errstr);
int	read_mp3_header(const char* filename, size_t* nSamples, int* nChans, int* fs, mp3data_struct* mp3info, char* errstr);
int	read_aiff_header(const char* filename, size_t* nSamples, int* nChans, int* fs, char* errstr);
#if defined(__cplusplus)
}
#endif 
