#include <fftw3.h>
#include <sndfile.h>


float* read_wav_file(const char *filename, unsigned int *channels, int *total_samples_count);
void write_wav_file(const char* filename, const float *audio_buffer, size_t total_samples_count, unsigned int sample_rate, unsigned int channels);
void write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins);