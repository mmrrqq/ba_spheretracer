#include "PrimitiveScene.h"

/**
 * @brief Parse scene information from file.
 * Reads the file specified by filePath and extracts the scene information.
 * @param filePath Path of the .sce File to Read
 */
void PrimitiveScene::ReadScene(std::string filePath)
{
    clearData();

    std::ifstream ifs(filePath);
    if (!ifs)
    {
        std::cerr << "Cannot open file " << filePath << std::endl;
        exit(1);
    }

    char line[200];
    std::string token;

    // parse file
    while (ifs && (ifs >> token) && (!ifs.eof()))
    {
        if (token[0] == '#')
        {
            ifs.getline(line, 200);
        }
        else if (token == "camera")
        {
            ifs >> Eye[0] >> Eye[1] >> Eye[2] >> Eye[3] >> //
                LookAt[0] >> LookAt[1] >> LookAt[2] >> LookAt[3];
        }
        else if (token == "torus")
        {
            Torus torus;
            ifs >> torus;
            Tori.push_back(torus);
        }
        else if (token == "areaLight")
        {
            AreaLight light;
            ifs >> light;
            AreaLights.push_back(light);
        }
        else if (token == "sphere")
        {
            Sphere sphere;
            ifs >> sphere;
            Spheres.push_back(sphere);
        }
        else if (token == "material")
        {
            Material material;
            ifs >> material;
            Materials.push_back(material);
        }
        else if (token == "mesh")
        {
            SDField sdField;
            Meshes.push_back(std::move(sdField)); // move to vector first ..
            ifs >> Meshes.back();                 // .. and then set data
        }
    }
    ifs.close();
}

void PrimitiveScene::clearData()
{
    Spheres.clear();
    Tori.clear();
    Meshes.clear();
    Materials.clear();
    AreaLights.clear();
    Eye = glm::vec4(0);
    LookAt = glm::vec4(0);
}