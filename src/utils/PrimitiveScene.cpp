#include "PrimitiveScene.h"

void PrimitiveScene::SetData()
{
    Shader.Bind();
    Shader.SetBuffer(3, sizeof(Sphere) * Spheres.size(), Spheres.data());
    Shader.SetBuffer(4, sizeof(Torus) * Tori.size(), Tori.data());
    Shader.SetBuffer(5, sizeof(AreaLight) * AreaLights.size(), AreaLights.data());
    Shader.SetBuffer(6, sizeof(Material) * Materials.size(), Materials.data());
    Shader.SetUniform("UMarchingSteps", 100);
    Shader.SetUniform("USDField", &sdField, 2);
}