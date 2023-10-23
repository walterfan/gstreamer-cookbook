#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "string_util.h"
#include "hls_util.h"

static const std::string EXT_X_TARGETDURATION = "#EXT-X-TARGETDURATION";
static const std::string EXT_INF = "#EXTINF";

int parse_m3u8_file(const std::string& filename, HlsPlaylist& hlsPlaylist) {
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the M3U8 file." << std::endl;
        return -1;
    }

    std::string line;
    HlsSegment currentSegment;
    bool inSegment = false;

    while (std::getline(file, line)) {
        if (line.find("#EXT-X-TARGETDURATION") != std::string::npos) {
            // Extract target duration
            size_t pos = line.find(':');
            hlsPlaylist.m_target_duration = str_to_num<uint32_t>(line.substr(pos + 1));
        } else if (line.find("#EXTINF") != std::string::npos) {
            // Extract segment duration
            size_t pos = line.find(':');
            currentSegment.m_duration = std::stof(line.substr(pos + 1));
            inSegment = true;
        } else if (inSegment) {
            // Extract segment URL
            currentSegment.m_uri = line;
            hlsPlaylist.m_segments.push_back(currentSegment);
            inSegment = false;
        } else if (line.find("#EXT-X-START") != std::string::npos) {
            // Extract #EXT-X-START tag
            size_t pos = line.find(':');
            size_t commaPos = line.find(',');
            hlsPlaylist.m_start_tag.m_time_offset = std::stof(line.substr(pos + 1, commaPos - pos - 1));
            hlsPlaylist.m_start_tag.m_precise = line.substr(commaPos + 1);
        }
    }
    file.close();
    return hlsPlaylist.m_segments.size();
}

/*
example:

#EXTM3U
#EXT-X-VERSION:3
#EXT-X-MEDIA-SEQUENCE:0
#EXT-X-TARGETDURATION:10

#EXTINF:8.9999990463256836,
record_20231019160652_00000.ts
#EXTINF:9.7999992370605469,
record_20231019160652_00001.ts
*/

int write_m3u8_file(const std::string& filename, const HlsPlaylist& hlsPlaylist) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return -1;
    }

    // Write the #EXTM3U header
    file << "#EXTM3U" << std::endl;

    // Write the #EXT-X-VERSION tag
    file << "#EXT-X-VERSION:3" << std::endl;

    // Write the #EXT-X-MEDIA-SEQUENCE tag
    file << "#EXT-X-MEDIA-SEQUENCE:" << hlsPlaylist.m_media_sequence << std::endl;

    // Write the #EXT-X-TARGETDURATION tag
    file << "#EXT-X-TARGETDURATION:" << hlsPlaylist.m_target_duration << std::endl;

    // Write the #EXT-X-START tag
    file << "#EXT-X-START:TIME-OFFSET=" << hlsPlaylist.m_start_tag.m_time_offset 
    << ",PRECISE=" << hlsPlaylist.m_start_tag.m_precise << std::endl;

    // Write the segment information
    for (const HlsSegment& segment : hlsPlaylist.m_segments) {
        file << "#EXTINF:" << segment.m_duration << "," << std::endl; // Segment duration
        file << segment.m_uri << std::endl; // Segment URL
    }

    file.close();
    return 0;
}

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

int find_closed_small_value(std::vector<std::string>& numbers, const std::string& target) {
    if (numbers.empty()) {
        // Handle the case when the list is empty
        throw std::runtime_error("The list is empty.");
    }

    int64_t diff = stoull(target) - stoull(numbers[0]); 
    
    if (diff < 0) {
        return -1;
    }
    int i = 1;
    for (; i < numbers.size(); ++i) {
        diff = stoull(target) - stoull(numbers[i]);  // Calculate the absolute difference

        if (diff < 0) {
            return i -  1;
        }
    }
    return i;
}