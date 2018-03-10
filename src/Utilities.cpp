//
// Created by Benjamin Trapani on 3/10/18.
//

#include "Utilities.h"
#include <sstream>
#include <fstream>

using namespace FractalTerrain;

std::string Utilities::slurpFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        std::stringstream errorStream;
        errorStream << "Unable to open file " << path;
        throw std::invalid_argument(errorStream.str());
    }
    std::stringstream slurpedFile;
    slurpedFile << file.rdbuf();
    return slurpedFile.str();
}