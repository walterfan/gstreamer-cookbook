#pragma once

#include <string>
#include <vector>
#include <map>

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames);

int load_yaml(const std::string& path, 
    std::map<std::string, std::vector<std::string>>& config);