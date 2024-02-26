#include "pad.h"


void pad(float* signal, int signal_length, Padding* padding) {
    int i, left_reflected_index, right_reflected_index;
    // Calculate the left and right reflected indices based on padding mode
    printf("%d\n", padding->mode);

    switch (padding->mode) {
        case REFLECT:
            // Pad the left side of the output array
            for (i = 0; i < padding->width; i++) {
                left_reflected_index = padding->width - i - 1;
                if (left_reflected_index >= 0) {
                    padding->padded_signal[i] = signal[left_reflected_index];
                }
            }

            // Copy samples to the middle
            for (i = 0; i < signal_length; i++) {
                    padding->padded_signal[i + padding->width] = signal[i];
            }
            
            // Pad the right side of the output array
            for (i = 0; i < padding->width; i++) {
                right_reflected_index = signal_length - i - 1;
                if (right_reflected_index >= 0) {
                    padding->padded_signal[padding->width + signal_length + i] = signal[right_reflected_index];
                }
            }
            break;


        case SYMMETRIC:
            left_reflected_index  = (padding->width > 0) ? (padding->width - 1) : 0;
            right_reflected_index = (padding->width > 0) ? (signal_length - 1 - padding->width) : 0;
            break;
        case EDGE:
            left_reflected_index = 0;
            right_reflected_index = (signal_length > 0) ? (signal_length - 1) : 0;
            break;

        default:
            memcpy(padding->padded_signal + padding->width, signal, signal_length * sizeof(float));
            break;
    }
}
