#include "string_util.h"

bool endswith(const std::string& filename, const std::string& suffix) {
    if (filename.length() >= suffix.length()) {
        return 0 == filename.compare (filename.length() 
        - suffix.length(), suffix.length(), suffix);
    } else {
        return false;
    }
}

bool startswith(const std::string& filename, const std::string& prefix) {
    
    if (filename.length() >= prefix.length()) {
        return 0 == filename.compare(0, prefix.size(), prefix);
    } else {
        return false;
    }
}
