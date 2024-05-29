
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_trafo_stft.h:

Program Listing for File trafo_stft.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_trafo_stft.h>` (``fastft/include/trafo_stft.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef TRAFO_STFT_H
   #define TRAFO_STFT_H
   
   #include <fftw3.h>
   #include <math.h>
   #include <stdlib.h>
   
   #include "pad.h"
   #include "window.h"
   #include "signal.h"
   
   typedef struct {
       int win;                  
       int hop;                  
       int nfft;                 
       fftwf_plan plan;          
       float* stft_in;           
       fftwf_complex* stft_out;  
       float *wnd;               
       Padding* padding;         
   } StftStruct;
   
   void stft_init(StftStruct *stft_struct, int n_fft, int win_length, int hop_length, Padding* padding);
   
   fftwf_complex* stft_compute(StftStruct* stft_struct, Signal *channel_signal, int num_bins, int do_pad);
   
   void stft_inner(StftStruct* stft_struct, float* signal, fftwf_complex** stft_result, int num_frames, int num_bins);
   
   void stft_clean(StftStruct *stft_struct);
   
   #endif
