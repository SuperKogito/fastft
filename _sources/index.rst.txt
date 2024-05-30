.. Fastft documentation master file, created by
   sphinx-quickstart on Fri Apr 26 18:56:22 2024.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to fastft's documentation!
==================================

.. image:: _static/logo.png
   :scale: 30 %
   :align: center


Motivation
==========

The Short Time Fourier Transform (STFT) is a common tool in audio AI tasks. 
However, there is currently no standard implementation in C that facilitates fast and efficient features extraction for the purpose of inference. 
Fastft aims to address this gap by offering an implementation based on the Fastest Fourier Transform in the West (FFTW). 
This implementation is suitable for Spectrogram/STFT-based inference (e.g., models like Spleeter, MOSnet, 
etc.), and it can also be extended to cover feature extraction algorithms such as MFCC. 
The implementation aims to be as efficient as possible in order to target CPU-based low-latency inference solutions.

While some deep learning libraries offer the option of incorporating STFT into the model, these implementations 
often differ and may restrict developer flexibility; two critical considerations when targeting embedded hardware.


Documentation
============

.. toctree::
   :maxdepth: 6
   :caption: Contents   

   api/fastft.rst


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
