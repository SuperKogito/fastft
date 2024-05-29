
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_signal.h:

Program Listing for File signal.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_signal.h>` (``fastft/include/signal.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef SIGNAL_H
   #define SIGNAL_H
   
   #include <fftw3.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include <string.h>
   #include <sndfile.h>
   #include <math.h>
   
   typedef struct {
       int sample_rate;          
       unsigned int num_channels; 
       int num_samples;          
       float *data;              
   } Signal;
   
   typedef struct {
       int num_samples_per_channel; 
       Signal *left;                
       Signal *right;               
   } StereoSignal;
   
   
   int compute_num_frames(int num_samples, int win, int hop);
   float* read_wav_file(const char *filename, unsigned int *channels, int *total_samples_count);
   void write_wav_file(const char* filename, const float *audio_buffer, size_t total_samples_count, unsigned int sample_rate, unsigned int channels);
   void write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins);
   
   #endif
