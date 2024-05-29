
.. _program_listing_file__home_am_fftw_stft_external_fastft_include_pad.h:

Program Listing for File pad.h
==============================

|exhale_lsh| :ref:`Return to documentation for file <file__home_am_fftw_stft_external_fastft_include_pad.h>` (``fastft/include/pad.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef PAD_H
   #define PAD_H
   
   #include <string.h>
   #include <stdio.h>
   #include <stdlib.h>
   
   typedef enum {
       REFLECT,    
       SYMMETRIC,  
       EDGE,       
       CONSTANT    
   } PaddingMode;
   
   typedef struct {
       int width;                  
       float* padded_signal;       
       int padded_signal_length;   
       int unpadded_signal_length; 
       PaddingMode mode;           
       int padded_num_frames;      
       int unpadded_num_frames;    
   } Padding;
   
   
   
   void init_padding(Padding* padding, PaddingMode mode, int width, int unpadded_signal_length, int padded_signal_length, int unpadded_num_frames, int padded_num_frames);
   
   void apply_padding(float* signal, int signal_length, Padding* padding);
   
   #endif
