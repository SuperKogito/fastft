import os 
import sys
import librosa
import argparse
import numpy as np

# Add the parent directory to sys.path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from tensorflow import keras
from tensorflow.keras import Model, layers
from tensorflow.keras.layers import Dense, Dropout, Conv2D
from tensorflow.keras.layers import TimeDistributed


class CNN(object):
    """
    copied from MOSNet original implementation.
    """
    def __init__(self):
        print('CNN init')
        
    def build(self):
        _input = keras.Input(shape=(None, 257))
        
        re_input = layers.Reshape((-1, 257, 1), input_shape=(-1, 257))(_input)
        
        # CNN
        conv1 = (Conv2D(16, (3,3), strides=(1, 1), activation='relu', padding='same'))(re_input)
        conv1 = (Conv2D(16, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv1)
        conv1 = (Conv2D(16, (3,3), strides=(1, 3), activation='relu', padding='same'))(conv1)
        
        conv2 = (Conv2D(32, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv1)
        conv2 = (Conv2D(32, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv2)
        conv2 = (Conv2D(32, (3,3), strides=(1, 3), activation='relu', padding='same'))(conv2)
        
        conv3 = (Conv2D(64, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv2)
        conv3 = (Conv2D(64, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv3)
        conv3 = (Conv2D(64, (3,3), strides=(1, 3), activation='relu', padding='same'))(conv3)
        
        conv4 = (Conv2D(128, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv3)
        conv4 = (Conv2D(128, (3,3), strides=(1, 1), activation='relu', padding='same'))(conv4)
        conv4 = (Conv2D(128, (3,3), strides=(1, 3), activation='relu', padding='same'))(conv4)
        
        # DNN
        flatten = TimeDistributed(layers.Flatten())(conv4)
        dense1=TimeDistributed(Dense(64, activation='relu'))(flatten)
        dense1=Dropout(0.3)(dense1)

        frame_score=TimeDistributed(Dense(1), name='frame')(dense1)

        average_score=layers.GlobalAveragePooling1D(name='avg')(frame_score)
        
        model = Model(outputs=[average_score, frame_score], inputs=_input)
        
        return model

if __name__ == "__main__":
    # Init verbosity
    verbose = 0

    # set up parser
    parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter)
    parser.add_argument("--fpath", help = "wave file path.",         type = str)

    # init varsm, instances and fnames
    args = parser.parse_args()
    # debug: print(args)

    if len(sys.argv) < 3:
        print("Input file arguments are missing.")
        sys.exit(0)

    wav_fpath  = args.fpath or ''
    file_fs    = 16000

    # Init STFT vars
    FFT_SIZE = 512 
    HOP_LENGTH = 256
    WIN_LENGTH = 512

    # Initialize dictionary to store average MOS values
    avg_mos_values = {}

    # Load Keras model
    MOSNet_model = CNN()
    keras_model = MOSNet_model.build()
    keras_model.load_weights('./cnn.h5')

    # Load audio and compute magnitude spectrogram
    sig, fs = librosa.load(wav_fpath, sr=file_fs)
    
    # Compute STFT magnitude
    mag = np.abs(librosa.stft(y=np.asfortranarray(sig), 
                              n_fft=FFT_SIZE, 
                              hop_length=HOP_LENGTH, 
                              win_length=WIN_LENGTH, 
                              window="hamm"))
    mag = np.transpose(mag.astype(np.float32))
    mag = mag[None, ...]

    # Perform prediction using Keras model
    keras_res = keras_model.predict_on_batch(mag)
    print("MOS average: %.3f\n" % keras_res[0][0][0])

    # Print results
    if verbose:
        for frame_mos in keras_res[1].flatten():
            print("{:<4.3f}".format(frame_mos))
