# Route Visualization

Wrote to be DAA's Quiz 2 Assignment Project.

|Name|NRP|
|----|---|
|Fernando|5025231091|


## Prerequisite
- C++ >=17
- CMake >=3.15
- [Raylib](https://www.raylib.com/) installed system-wide, or internet access during CMake configure so it can be fetched automatically.

## Demonstration
https://github.com/user-attachments/assets/56bb8cce-6420-4d43-a317-ab286444b7b6


## Build from source
- Create build directory
```bash
cmake -S . -B build
```

- Build with CMake
```bash
cmake --build build --parallel
```

- Run built program
```bash
./route_visualization
```

## How it works
- Generate perlin noise for continuous random weight
- Generate random graph, with weight using values from the perlin noises.
- Dijkstra algorithm to get shortest path from user's chosen starting point
- Raylib to visualize the graph and the path in easy to understand way

## References
- [Excellent perlin noise explanation!](https://rtouti.github.io/graphics/perlin-noise-algorithm)
- [Raylib, a very powerful C and C++ compatible graphics library! Highly recommend to those that want to explore.](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [cplusplus.com, useful documentation.](https://cplusplus.com)
