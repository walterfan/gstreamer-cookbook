#pragma once

#include <string>
#include <vector>
#include <map>

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames);

int yaml_to_str_vec_map(const std::string& path, const std::string& root, 
    std::map<std::string, std::vector<std::string>>& config);



