# camera: eye, look at
camera 0 1 5 1  0 0 0 1

# light: position, color and size
areaLight -1 10 -1 1  1.0 0.2 0.2 1.0  0.02
areaLight 10 0 5 1    0.2 0.2 1.0 1.0  0.10
areaLight 5 5 5 1     0.5 0.5 0.5 1.0  0.05

# material: ambient color, diffuse color, specular color and shininess
material 0.1 0.1 0.1 1   0.7 0.7 0.7 1   0.7 0.7 0.7 1   10.0
material 0.1 0.0 0.1 1   0.6 0.0 0.6 1   0.7 0.0 0.7 1   10.0
material 0.1 0.1 0.0 1   0.9 0.9 0.0 1   0.6 0.6 0.0 1   10.0
material 0.01 0.1 0.0 1  0.05 0.2 0.0 1  0.05 0.2 0.0 1  10.0
