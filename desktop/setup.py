from setuptools import setup

APP = ["main.py"]
OPTIONS = {
    "argv_emulation": False,
    "frameworks": [
        "../build/spectrum_analyzer/libspectrum_analyzer.dylib",
        "../build/audio_decoder/libaudio_decoder.dylib",
    ],
    "plist": "../Info.plist",
}

setup(
    app=APP,
    options={"py2app": OPTIONS},
    setup_requires=["py2app"],
)
