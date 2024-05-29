
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_spectral.h:

Program Listing for File spectral.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_spectral.h>` (``fastft/include/spectral.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef SPECTRAL_H
   #define SPECTRAL_H
   
   
   #include "pad.h"
   #include "window.h"
   #include "trafo_stft.h"
   #include "trafo_istft.h"
   
   
   void calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length);
   
   #endif
