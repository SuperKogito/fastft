
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_trafo_istft.h:

Program Listing for File trafo_istft.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_trafo_istft.h>` (``fastft/include/trafo_istft.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef TRAFO_ISTFT_H
   #define TRAFO_ISTFT_H
   
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
       fftwf_complex *istft_in;        
       float *istft_out;               
       float *wnd;                     
       float *window_sum;              
   } IstftStruct;
   
   void istft_init(IstftStruct *istft_struct, int n_fft, int reconstructed_signal_length, int win_length, int hop_length);
   
   void istft_compute(IstftStruct* istft_struct, fftwf_complex *stft_values, float *reconstructed_signal, int reconstructed_signal_length, int num_frames, int num_bins);
   
   void istft_clean(IstftStruct *istft_struct);
   
   
   
   // Function to normalize an array based on its maximum absolute value
   void normalize_array(float *arr, int length, float max_value);
   
   // Function to find the maximum absolute value in an array
   float get_max(const float *arr, int length);
   
   #endif
