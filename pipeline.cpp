#include "pipeline.hpp"

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace vkEngine {

    Pipeline::Pipeline(const std::string& vertFilepath, const std::string& fragFilepath) {
        createGraphicsPipeline(vertFilepath , fragFilepath);
    }
    
    std::vector<char> Pipeline::readFile(const std::string& filePath) {
        
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Falied to open file: " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }


    void Pipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath) {
        auto vertCode = readFile(vertFilepath);
        auto fragCode = readFile(fragFilepath);

        std::cout << "Vert code: " << vertCode.size() << std::endl;
        std::cout << "Frag code: " << fragCode.size() << std::endl;
    }


}