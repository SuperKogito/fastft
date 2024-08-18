#ifndef FASTFT_PADDING_H
#define FASTFT_PADDING_H

#ifdef __cplusplus
extern "C" {
#endif

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
} fastft_padding_mode_t;

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
    fastft_padding_mode_t mode;           /**< Padding mode used. */
    int padded_num_frames;      /**< Number of frames in the padded signal. */
    int unpadded_num_frames;    /**< Number of frames in the original (unpadded) signal. */
} fastft_padding_t;

void fastft_padding_init(fastft_padding_t* padding, fastft_padding_mode_t mode, int width, int unpadded_signal_length, int padded_signal_length, int unpadded_num_frames, int padded_num_frames);

/**
 * @brief Pad a signal with the specified padding mode.
 * 
 * This function pads the input signal with the specified padding mode and stores the result
 * in the provided fastft_padding_t structure.
 * 
 * @param signal Pointer to the input signal array.
 * @param signal_length Length of the input signal array.
 * @param padding Pointer to the fastft_padding_t structure where the padding information will be stored.
 */
void fastft_padding_apply(float* signal, int signal_length, fastft_padding_t* padding);

#ifdef __cplusplus
}
#endif

#endif
