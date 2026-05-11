import ctypes
import os
import sys
from enum import Enum, auto


class DEVICE_TYPE(Enum):
    SPEAKER = 0
    MICROPHONE = auto()


class Spectrum(ctypes.Structure):
    _fields_ = [
        ("normalized_frequencies", ctypes.POINTER(ctypes.c_float)),
        ("frequency_bin_count", ctypes.c_size_t),
    ]


class Device(ctypes.Union):
    _fields_ = [
        ("speaker", ctypes.c_void_p),
        ("microphone", ctypes.c_void_p),
    ]


class AudioDecoder(ctypes.Structure):
    _fields_ = [
        ("sample_rate", ctypes.c_size_t),
        ("device", Device),
    ]


def init_audio():
    spectrum_analyzer = ctypes.CDLL(find_audio_library("libspectrum_analyzer.dylib"))
    audio_decoder = ctypes.CDLL(find_audio_library("libaudio_decoder.dylib"))

    spectrum_analyzer.spectrum_analyzer_init.restype = None
    spectrum_analyzer.spectrum_analyzer_init.argtypes = []
    spectrum_analyzer.spectrum_samples_append.restype = None
    spectrum_analyzer.spectrum_samples_append.argtypes = [
        ctypes.POINTER(ctypes.c_float),
        ctypes.c_size_t,
    ]
    spectrum_analyzer.spectrum_create.restype = Spectrum
    spectrum_analyzer.spectrum_create.argtypes = [ctypes.c_size_t]
    spectrum_analyzer.spectrum_destroy.restype = None
    spectrum_analyzer.spectrum_destroy.argtypes = [ctypes.POINTER(Spectrum)]

    OnSamplesProcessed = ctypes.CFUNCTYPE(
        None, ctypes.POINTER(ctypes.c_float), ctypes.c_size_t
    )
    audio_decoder.audio_decoder_start.restype = AudioDecoder
    audio_decoder.audio_decoder_start.argtypes = [ctypes.c_int, OnSamplesProcessed]
    audio_decoder.audio_decoder_stop.restype = None
    audio_decoder.audio_decoder_stop.argtypes = [ctypes.c_int, Device]

    return spectrum_analyzer, audio_decoder, OnSamplesProcessed


def find_audio_library(library_name):
    bundle = os.path.dirname(os.path.dirname(os.path.abspath(sys.executable)))

    library_path = os.path.join(bundle, "Frameworks", library_name)
    if not os.path.exists(library_path):
        raise FileNotFoundError(f"Cannot find audio library: {library_name}")

    return library_path
