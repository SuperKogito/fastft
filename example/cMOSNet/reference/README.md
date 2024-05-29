# MOSNet

Reference Python implementation of [MOSNet: Deep Learning based Objective Assessment for Voice Conversion](https://arxiv.org/abs/1904.08352).

## Dependency
You will need the following requirements to be installed to use this MOSNet version.

- tensorflow==2.11.0
- librosa==0.10.1

### Environment set-up
For example,
```
conda create -n mosnet python=3.9
conda activate mosnet
pip install -r requirements.txt
```

## Usage

```
usage: keras_model.py [-h] [--fpath FPATH]

optional arguments:
  -h, --help     show this help message and exit
  --fpath FPATH  wave file path.

```

> cMOSNet only works with audio wave files with sample rat of 16000 Hz (like the original).

- Example:  `python keras_model.py --fpath ../test.wav` to run the prediction using the original keras model. 
 
## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE](https://github.com/SuperKogito/fastft/blob/master/LICENSE) file for details.
