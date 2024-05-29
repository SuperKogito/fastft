![](extras/logo.png)


**Fastft**: **Fa**st **S**hort **T**ime **F**ourrier **T**ransform implementation based on **F**astest **F**ourier **T**ransform in the **W**est (**FFTW**).


# Motivation

The Short Time Fourier Transform (STFT) is a common tool in audio AI tasks. However, there is currently no standard implementation in C that facilitates fast and efficient features extraction for the purpose of inference. **Fastft** aims to address this gap by offering an implementation based on the Fastest Fourier Transform in the West (FFTW). This implementation is suitable for Spectrogram/STFT-based inference (e.g., models like [Spleeter](https://github.com/deezer/spleeter), [MOSnet](https://github.com/lochenchou/MOSNet), etc.), and it can also be extended to cover feature extraction algorithms such as MFCC. The implementation aims to be as efficient as possible in order to target CPU-based low-latency inference solutions. 

While some deep learning libraries offer the option of incorporating STFT into the model, these implementations **often differ** and **may restrict developer flexibility**; two critical considerations when targeting embedded hardware.


# Project structure

The following is an illustration of the different components of the project.

```
├── CMakeLists.txt
├── comparison                                # compare Librosa.stft to Fastft
│   ├── benchmark.py
│   ├── CMakeLists.txt
│   ├── fastft_librosa_mse.png
│   ├── fastft_vs_librosa.png
│   ├── main.cpp
│   ├── README.md
│   └── requirements.txt
├── docs                                      # project documentation
│   ├── Doxygen
│   │   ├── Doxyfile.cfg
│   │   └── xml
│   └── Sphinx
│       ├── make.bat
│       ├── Makefile
│       └── source
├── example                                  # usecase/ example
│   └── cMOSNet
│       ├── CMakeLists.txt
│       ├── main.c
│       ├── mosnet_cnn.onnx
│       ├── README.md
│       ├── reference
│       └── test.wav
├── extras                                    
│   ├── logo.png
│   └── small_logo.png
├── include
│   ├── pad.h
│   ├── signal.h
│   ├── spectral.h
│   ├── trafo_istft.h
│   ├── trafo_stft.h
│   └── window.h
├── README.md
├── resources
│   ├── chirp.wav
│   └── test.wav
├── src
│   ├── pad.c
│   ├── signal.c
│   ├── spectral.c
│   ├── trafo_istft.c
│   ├── trafo_stft.c
│   └── window.c
└── test                                     # gtests folder
    ├── CMakeLists.txt 
    ├── main.cpp
    └── test_stft.cpp
```

# Build

To build fastft you will need to install the following two libraries: 

- [FFTW](https://www.fftw.org/)
- [Libsndfile](https://libsndfile.github.io/libsndfile/)

Then, simply build the project using:

```
mkdir build
cd build
cmake ..
make
```

## Run the gtests

fastft includes some unit tests based on [gtests](http://google.github.io/googletest/). 
These can be run using `build/test/fastft_tests` after the build.

## Comparison 
A comparison betweeen librosa's python output and fastft output is present under `comparison/`.

## Demo
A demo / usecase example is available under `example/cMOSNet/`.

## Documentation 

The code documentation is available under [https://superkogito.github.io/fastft/](https://superkogito.github.io/fastft/).

## Citation

```
@misc{Malek2024,
 url    = {https://github.com/SuperKogito/fastft}, 
 year   = {2024}, 
 author = {Ayoub Malek}, 
 title  = {Fastft: Fast Short Time Fourrier Transform implementation based on Fastest Fourier Transform in the West (FFTW).}
} 
```