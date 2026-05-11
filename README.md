# Route Visualization

Wrote to be DAA's Quiz 2 Assignment Project.

|Name|NRP|
|----|---|
|Fernando|5025231091|


## Prerequisite
- C++ >=23
- CMake >=3.15
- [Raylib](https://www.raylib.com/)

## Build from source
- Create build directory
```bash
cmake -S . -B build
```

- Build with Make
```bash
cd build && make -j8
```

- Run built program
```bash
./route_visualization
```

## How it works (TODO)
- [x] Generate perlin noise for continuous random weight
- [ ] Generate random graph, with weight using values from the perlin noises.
- [ ] Raylib to visualize the graph in easy to understand way
- [ ] Dijkstra algorithm to get shortest path from user's chosen starting point
- [ ] Raylib to also visualize the path.

## References
- [Excellent perlin noise explanation!](https://rtouti.github.io/graphics/perlin-noise-algorithm)
- [Raylib, a very powerful C and C++ compatible graphics library! Highly recommend to those that want to explore.](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [cplusplus.com, useful documentation.](https://cplusplus.com)
