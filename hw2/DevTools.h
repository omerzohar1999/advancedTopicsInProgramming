//
// Created by amit on 6/30/24.
//

#ifndef HW1_DEVTOOLS_H
#define HW1_DEVTOOLS_H

#include <vector>
#include <string>

class DevTools {
public:
    // Public constructor with no parameters
    DevTools();
    bool isVisualizationEnabled();

    std::string getVisualizationFileName();

private:
    // Local variable to store the visualization setting
    bool is_visualization_enabled = false;
    void removeVisualizationFile();
};



#endif //HW1_DEVTOOLS_H