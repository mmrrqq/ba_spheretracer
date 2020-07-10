A realtime spheretracer developed for my bachelors thesis.
Does render primitives as well as distance fields which can be computed from
meshes using a compute shader.

## howto

cmake && make:

```sh
mkdir build && cd build && cmake .. && make -j<threads>
```

The build will probably only work on linux machines!

## Usage

Execute `Spheretracer` in the build directory.
Three different demo scenes can be selected in the imgui debug overlay.

## warning

Creating the distance field can result in incomplete 3D textures if the
graphics driver does time out.
The timeout differs for different operating systems.
To disable the timeout on linux for nvidia hardware completely, I followed this
guide:
https://forums.developer.nvidia.com/t/a-warning-message-from-cuda-visual-profiler/23490/4
