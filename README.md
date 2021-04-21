# BanG Dream BGP card recognize (WIP)

## Features

- Recognize the card in cardset and print the card id
- Encode card information to bestdori's format

## Build Requirement

- A compiler that support C++ 17 Standard
- [emsdk](https://github.com/emscripten-core/emsdk.git)

## Dependency

- OpenCV
- Nomango/jsonxx

## Build

```bash
# Please build OpenCV wasm first!
# https://docs.opencv.org/3.4/d4/da1/tutorial_js_setup.html
source ${EMSDK}/emsdk_env.sh
cd ${OPENCV_PATH}
emcmake python3 ./platforms/js/build_js.py build_wasm --build_wasm
# Build this project
mkdir build && cd build
emcmake cmake .. && emmake make -j$(nproc)
# If we want to debug in local machine, replace above command with this command
cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(nproc)
```

## Architecture

See [ARCHITECTURE.md](./ARCHITECTURE.md)

## Screenshot

![Screenshot](./Screenshot.png)
