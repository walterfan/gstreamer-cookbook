#pragma once

#include <string>
#include <vector>

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames);