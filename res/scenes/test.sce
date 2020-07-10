# camera: eye, look at
camera 5 1 10 1  0 0 0 1

# light: position, color and size
areaLight -1 10 -1 1  0.8 0.2 0.2 1.0  0.02
areaLight 5 5 5 1     0.8 0.8 0.8 1.0  0.05

# sphere: position, radius and materialId
sphere 0.5 0.9 1.2 1  0.4  1
sphere 0.5 0.5 0.5 1  0.6  0
sphere 1.7 0.0 1.5 1  0.6  3

# torus: position, radius, tube radius and material id
torus 0.5 0.5 0.5 1  1.0  0.2  2
torus 0.0 4.0 0.0 1  2.0  0.2  0

# material: ambient color, diffuse color, specular color and shininess
material 0.1 0.1 0.1 1   0.7 0.7 0.7 1   0.7 0.7 0.7 1   10.0
material 0.1 0.0 0.1 1   0.6 0.0 0.6 1   0.7 0.0 0.7 1   10.0
material 0.1 0.1 0.0 1   0.9 0.9 0.0 1   0.6 0.6 0.0 1   10.0
material 0.01 0.1 0.0 1  0.05 0.2 0.0 1  0.05 0.2 0.0 1  10.0

# mesh: file, position (3), dimensions (3), materialId, uniform name
mesh res/meshes/amo.off  -5 2 0  3 3 3  0  USDField