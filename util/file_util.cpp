#include <iostream>
#include <fstream>
#include <dirent.h>
#include "string_util.h"
#include "file_util.h"
#include "yaml-cpp/yaml.h"

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames) {

    struct dirent* direntp;
    // Open the directory
    DIR* dirp = opendir(path.c_str());

    if (!dirp) {
        std::cerr << "Directory not found." << path << std::endl;
        return -1;
    }

    while( NULL != (direntp = readdir(dirp))) {
        std::string filename = direntp->d_name;
        if(".." == filename || "." == filename)
            continue;

        if (startswith(filename, prefix)
                        && endswith(filename, suffix)) {
            std::string strPath = path;
            strPath.append("/");
            strPath.append(filename);

            fileNames.push_back(filename.substr(prefix.length(), 
                    filename.length() - prefix.length() - suffix.length()));
        }
    }

    while((-1 == closedir(dirp)) && (errno == EINTR));


    if (fileNames.empty()) {
        return 0;
    }
    // Sort the file names alphabetically
    std::sort(fileNames.begin(), fileNames.end());
    
    return fileNames.size();
}   

int yaml_to_str_vec_map(const std::string& path, const std::string& key,
    std::map<std::string, std::vector<std::string>>& config) {
    
    YAML::Node rootNode = YAML::LoadFile(path);
    if (rootNode[key]) {
        YAML::Node nodes = rootNode[key]; 
        YAML::const_iterator it=nodes.begin();     
        for (; it!=nodes.end(); ++it) {
            std::string key = it->first.as<std::string>();
            //std::cout << key << ":\n";
            std::vector<std::string> vals = it->second.as<std::vector<std::string>>();
            //for(auto& val: vals) {
            //    std::cout << "\t" << val << "\n";
            //}
            config.emplace(std::make_pair(key, vals));
        }
    }
    return 0;
}

YAML::Node load_yaml(const std::string& path, const std::string& key) {
    YAML::Node rootNode = YAML::LoadFile(path);
    return rootNode[key];
}

