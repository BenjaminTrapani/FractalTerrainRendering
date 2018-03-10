//
// Created by Benjamin Trapani on 3/10/18.
//
#pragma once
#include <string>

namespace FractalTerrain {
    class Utilities {
    public:
        Utilities() = delete;

        static std::string slurpFile(const std::string& path);
    };
}
