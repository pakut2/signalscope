from setuptools import setup

APP = ["main.py"]
OPTIONS = {
    "argv_emulation": False,
    "frameworks": [
        "../audio_decoder/build/libaudio_decoder.dylib",
        "../spectrum_analyzer/build/libspectrum_analyzer.dylib",
    ],
    "plist": "../Info.plist",
}

setup(
    app=APP,
    options={"py2app": OPTIONS},
    setup_requires=["py2app"],
)
