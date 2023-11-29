#include <iostream>
#include <fstream>
#include "string_util.h"
#include "file_util.h"
#include "yaml-cpp/yaml.h"

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames) {

    // Check if the directory exists
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Directory not found." << std::endl;
        return -1;
    }

    // Iterate through the files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            if (startswith(filename, prefix) && endswith(filename, suffix)) {
                fileNames.push_back(filename.substr(prefix.length(), 
                    filename.length() - prefix.length() - suffix.length()));
            }
        }
    }

    if (fileNames.empty()) {
        return 0;
    }
    // Sort the file names alphabetically
    std::sort(fileNames.begin(), fileNames.end());

    // Display the sorted file names
    int i = 0;
    for (const std::string& fileName : fileNames) {
        std::cout << std::setw(4)<< (i++) << ". " << fileName << std::endl;
    }
    
    return fileNames.size();
    
}

int load_yaml(const std::string& path, 
    std::map<std::string, std::vector<std::string>>& config) {
    
    YAML::Node node = YAML::LoadFile(path);
    if (node["pipelines"]) {
        YAML::Node pipelines = node["pipelines"]; 
        YAML::const_iterator it=pipelines.begin();     
        for (; it!=pipelines.end(); ++it) {
            std::string key = it->first.as<std::string>();
            //std::cout << key << ":\n";
            std::vector<std::string> vals = it->second.as<std::vector<std::string>>();
            //for(auto& val: vals) {
            //    std::cout << "\t" << val << "\n";
            //}
            config.emplace(std::make_pair(key, vals));
        }
    }
}
