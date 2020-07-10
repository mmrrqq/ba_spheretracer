A Realtime-Spheretracer developed for my bachelors thesis.
Can render primitives as well as distance fields which can be computed from
meshes using a compute shader.

## howto

clone this repository:

```sh
git clone --recursive https://github.com/Moarqi/ba_spheretracer
```

cmake && make:

```sh
cd ba_spheretracer && mkdir build && cd build && cmake .. && make -j<threads>
```

The build will probably only work on linux machines!

## Usage

Three different demo scenes can be selected in the imgui debug overlay.

## warning

Creating the distance field can result in incomplete 3D textures if the
graphics driver does time out.
The timeout differs for different operating systems.
To disable the timeout on linux for nvidia hardware completely, I followed this
guide:
https://forums.developer.nvidia.com/t/a-warning-message-from-cuda-visual-profiler/23490/4
