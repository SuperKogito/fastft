#ifndef PAD_H
#define PAD_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Enumeration of padding modes.
 * 
 * Defines the different modes for padding.
 */
typedef enum {
    REFLECT,    /**< Reflective padding mode. */
    SYMMETRIC,  /**< Symmetric padding mode. */
    EDGE,       /**< Edge padding mode. */
    CONSTANT    /**< Constant padding mode. */
} PaddingMode;

/**
 * @brief Structure representing padding information.
 * 
 * Contains information about the padding applied to a signal.
 */
typedef struct {
    int width;                  /**< Width of the padding. */
    float* padded_signal;       /**< Pointer to the padded signal array. */
    int padded_signal_length;   /**< Length of the padded signal array. */
    int unpadded_signal_length; /**< Length of the original (unpadded) signal. */
    PaddingMode mode;           /**< Padding mode used. */
    int padded_num_frames;      /**< Number of frames in the padded signal. */
    int unpadded_num_frames;    /**< Number of frames in the original (unpadded) signal. */
} Padding;



void init_padding(Padding* padding, PaddingMode mode, int width, int unpadded_signal_length, int padded_signal_length, int unpadded_num_frames, int padded_num_frames);

/**
 * @brief Pad a signal with the specified padding mode.
 * 
 * This function pads the input signal with the specified padding mode and stores the result
 * in the provided Padding structure.
 * 
 * @param signal Pointer to the input signal array.
 * @param signal_length Length of the input signal array.
 * @param padding Pointer to the Padding structure where the padding information will be stored.
 */
void apply_padding(float* signal, int signal_length, Padding* padding);

#endif
