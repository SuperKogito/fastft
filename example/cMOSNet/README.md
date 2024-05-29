# cMOSNet

This project provides a usage example of [fastft](https://github.com/SuperKogito/fastft). The example is a pure C implementation of the original Python [MOSNet](https://github.com/lochenchou/MOSNet) implementation of  [MOSNet: Deep Learning based Objective Assessment for Voice Conversion](https://arxiv.org/abs/1904.08352).
***MOSNet (Multi-Objective Subjective Assessment of Video Quality) model. It allows users to evaluate the quality of an audio file using the MOSNet model.***

> A C++ MOSNet implementation using fastft is available under [cppMOSnet](https://github.com/SuperKogito/cppMOSnet).

## Project Structure

The project structure is as follows (when the depedencies are installed):

```
.
├── CMakeLists.txt          # CMake build script
├── external                # External dependencies
│   ├── fastft              # Fast STFT library
│   └── onnxruntime         # ONNX Runtime library
├── main.c                  # Main source code file
├── mosnet_cnn.onnx         # Pre-trained MOSNet CNN model
├── README.md               # Project README file (you're reading it now!)
└── test.wav                # Sample WAV file for testing
```
# Dependencies 
Before building the project you need to download & place the following libraries under the `external/` folder: 

- [fastft](https://github.com/SuperKogito/fastft)
- [ONNX Runtime](https://github.com/microsoft/onnxruntime/releases) 

## Build
To build the project, you should do the following: 

```
mkdir build
cd build
cmake ..
make
```

## Usage 

```
Usage: ./cMOSNet <wav_fpath> <model_path> [verbose]
  <wav_fpath>  : Path to the WAV file
  <model_path> : Path to the ONNX model file
  [verbose]    : Optional. Set to 1 for verbose output
```

1. Run the CLI with the path to the WAV file and the pre-trained ONNX model:

```
./cMOSNet test.wav mosnet_cnn.onnx
```

## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE](https://github.com/SuperKogito/fastft/blob/master/LICENSE) file for details.
