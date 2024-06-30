//
// Created by amit on 6/30/24.
//

#include "DevTools.h"

#include <fstream>
#include <string>
#include <sstream>

DevTools::DevTools() {
    // Check if the file 'project-config.txt' exists
    std::ifstream configFile("project-config.txt");
    if (!configFile) {
        // If the file does not exist, return from the constructor
        return;
    }

    // If the file exists, read from it
    std::string line;
    while (std::getline(configFile, line)) {
        // Check if the line contains the setting for VISUALIZATION
        if (line.find("VISUALIZATION") != std::string::npos) {
            std::istringstream iss(line);
            std::string key, equals, value;
            if (iss >> key >> equals >> value) {
                // Convert value to boolean and set is_visualization_enabled
                is_visualization_enabled = (value == "true");
            }
        }
    }
}

bool DevTools::isVisualizationEnabled() {
   return is_visualization_enabled;
}

std::string DevTools::getVisualizationFileName() {
    return "visualization.txt";
}
