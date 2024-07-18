//
// Created by amit on 6/30/24.
//

#include "DevTools.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

#define CONFIG_FILE "project-config.txt"
#define VISUALIZATION_FIELD "VISUALIZATION"
#define VISUALIZATION_FILE_NAME "visualization.txt"

DevTools::DevTools() {
    try {
        // Check if the file 'project-config.txt' exists
        std::ifstream configFile(CONFIG_FILE);
        if (!configFile) {
            // If the file does not exist, return from the constructor
            return;
        }

        // If the file exists, read from it
        std::string line;
        while (std::getline(configFile, line)) {
            // Check if the line contains the setting for VISUALIZATION
            if (line.find(VISUALIZATION_FIELD) != std::string::npos) {
                std::istringstream iss(line);
                std::string key, equals, value;
                if (iss >> key >> equals >> value) {
                    // Convert value to boolean and set is_visualization_enabled
                    is_visualization_enabled = (value == "true");
                }
            }
        }
    } catch (const std::ifstream::failure &e) {
        std::cerr << "Exception reading from config file: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    removeVisualizationFile();
}

bool DevTools::isVisualizationEnabled() {
    return is_visualization_enabled;
}

std::string DevTools::getVisualizationFileName() {
    return VISUALIZATION_FILE_NAME;
}

void DevTools::removeVisualizationFile() {
    std::string fileName = VISUALIZATION_FILE_NAME;

    try {
        // Check if the file exists
        if (std::filesystem::exists(fileName)) {
            // Attempt to remove the file
            if (!std::filesystem::remove(fileName)) {
                std::cerr << "File " << fileName << " could not be removed." << std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}
