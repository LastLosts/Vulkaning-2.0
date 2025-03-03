# Vulkaning 2.0
Vulkaning 2.0 is rewritten and upgraded version of [Vulkaning](https://github.com/LastLosts/Vulkaning).
### Changes from Vulkaning:
1. Using vulkan.h instead of vulkan.hpp, because it was slowing compile times significantly.
2. Using glfw instead of sdl, because of less code.
3. Better structre (separated engine code with demos code).
4. No c++ exceptions.

## Building

### Linux
To build the project on Linux you need to install CMake and C++ 20 compiler such as gcc or clang.
```shell
git clone --recurse-submodules https://github.com/LastLosts/Vulkaning-2.0.git
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Windows and Visual Studio 
> [!WARNING]  
> This wasn't tested and isn't guaranteed to work.
> 
To build the project on Windows you need to have CMake and Visual Studio.
1. Clone or download the project sources
```batch
git clone --recurse-submodules https://github.com/LastLosts/Vulkaning-2.0.git
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release .. 
```
This will generate Visual Studio solution file.
