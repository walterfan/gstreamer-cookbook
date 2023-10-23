#pragma once
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Define a structure to represent a single HLS segment
 * 
 */
struct HlsSegment {
    float m_duration;
	std::string m_uri;
};


/**
 * @brief Define a structure to represent the #EXT-X-START tag
 * 
 */
struct HlsStartTag {
    float m_time_offset = 0.0; // Time offset in seconds
    std::string m_precise = "YES"; // Precise attribute (can be "YES" or "NO")
};

/**
 * @brief define a structure to represent an HLS playlist
 * 
 * 
 * #EXTM3U
 * #EXT-X-VERSION:3
 * #EXT-X-ALLOW-CACHE:NO
 * #EXT-X-MEDIA-SEQUENCE:0
 * #EXT-X-TARGETDURATION:10
 * #EXT-X-START:TIME-OFFSET=2,PRECISE=YES

 * #EXTINF:8.9999990463256836,
 * record_20231019160652_00000.ts
 * #EXTINF:9.7999992370605469,
 * record_20231019160652_00001.ts
 * 
 */
struct HlsPlaylist {
    uint32_t m_media_sequence = 0;
    uint32_t m_target_duration = 0;
    HlsStartTag m_start_tag;
    std::vector<HlsSegment> m_segments;
};

int parse_m3u8_file(const std::string& filename, HlsPlaylist& hlsPlaylist);

int write_m3u8_file(const std::string& filename, const HlsPlaylist& hlsPlaylist);

int get_playlist_files(const std::string& path, 
    const std::string& prefix, 
    const std::string& suffix,
    std::vector<std::string>& fileNames);

int find_closed_small_value(std::vector<std::string>& numbers, const std::string& target);