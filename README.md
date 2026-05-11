# SignalScope

## Terminal

### Initialize

```shell
cd build

cmake ../terminal -DCMAKE_BUILD_TYPE=Debug # Debug build
cmake .. # Release build
```

### Build

```shell
cd build

make
```

### Run

```shell
cd build

./signalscope.app/Contents/MacOS/signalscope
```

## Desktop

### Build Libraries

```shell
cd build

cmake ..
make
```

### Initialize

```shell
cd desktop

python -m venv .venv
source .venv/bin/activate
pip install setuptools py2app
```

### Build

```shell
cd desktop

source .venv/bin/activate
python setup.py py2app
```

### Run

Open application in `dist` directory.

