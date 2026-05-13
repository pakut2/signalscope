#!/bin/zsh

cmake -S audio_decoder -B audio_decoder/build && cmake --build audio_decoder/build
cmake -S spectrum_analyzer -B spectrum_analyzer/build && cmake --build spectrum_analyzer/build

cd desktop

python -m venv .venv
source .venv/bin/activate
pip install setuptools py2app

python setup.py py2app

deactivate

