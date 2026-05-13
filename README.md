# SignalScope

> [!CAUTION]
> Only supports MacOS (for now) (not really).

Visualize system-wide audio output.

## Requirements

- Clang
- Make
- CMake

### Desktop

- Python
- Tkinter

### Terminal

- ncurses

## Quickstart (Desktop)

Run `./build_desktop.sh`. Open built application under `./desktop/dist`.

## Quickstart (Terminal)

Run `./build_terminal.sh`. Application will start immediatelly in the current terminal session.

## External Usage

Core logic is separated into dynamic libraries:

- **audio_decoder**: Speaker / microphone realtime audio samples aquisition. MacOS exclusive.
- **spectrum_analyzer**: Audio samples frequency domain transformation and normalization.

Libraries can be built and used externally, as is done with Desktop and Terminal applications.

### Building Standalone Libraries

```shell
# cmake -S <library-name> -B <library-name>/build && cmake --build <library-name>/build
cmake -S spectrum_analyzer -B spectrum_analyzer/build && cmake --build spectrum_analyzer/build
```

