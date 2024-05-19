
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_window.h:

Program Listing for File window.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_window.h>` (``fastft/include/window.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef WINDOW_H
   #define WINDOW_H
   
   #include <math.h>
   
   #define LIBROSA_PI   3.14159265358979323846
   
   void compute_hanning_window(int n_fft, float *window);
   
   
   #endif
