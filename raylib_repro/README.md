# Raylib Repro

A C++ implementation using Raylib that ports an ESP32 TFT display implementation showing animated Markov chain state transitions.

## Raylib Implementation

- Window size: 160x128 pixels (matching original landscape orientation)
- 10 FPS animation matching Arduino ~100ms delay
- Same Markov chain logic and probability matrices

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Requirements
- Raylib library
- CMake 3.15+
- C++17 compiler
