#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "time_util.h"

/**
 * @brief Define a structure to represent a single HLS segment
 * 
 */
struct HlsSegment {

    HlsSegment(): m_duration(0)
    , m_total_duration(0)
    , m_uri("") {

    }
    
    HlsSegment(float duration, const std::string& uri)
    : m_duration(duration)
    , m_total_duration(0)
    , m_uri(uri) {

    }

    uint32_t get_sequence();

    int update_uri(uint32_t seq);

    float m_duration;
    float m_total_duration;
    TimePoint m_start_time;
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
class HlsPlaylist {
public:
    HlsPlaylist(const std::string& filename);
    virtual ~HlsPlaylist() = default;

    int read_m3u8_file();

    int write_m3u8_file();

    void set_media_sequence(uint32_t seq) {
        m_media_sequence = seq;
    }

    void set_target_duration(uint32_t duration) {
        m_target_duration = duration;
    }

    void set_time_offset(uint32_t ms) {
        m_start_tag.m_time_offset = (float)(ms)/1000;
    }

    void append_segment(HlsSegment segment) {
        m_segments.push_back(segment);
    }

    void sort_segments();

    TimePoint& get_start_time() {
        return m_start_time;
    }

    std::vector<HlsSegment>& get_segments() {
        return m_segments;
    }

    bool operator<(const HlsPlaylist& other) const {
        return m_start_time < other.m_start_time;
    }

    bool operator==(const HlsPlaylist& other) const {
        return m_start_time == other.m_start_time;
    }

private:
    friend std::ostream & operator<<(std::ostream &os, const HlsPlaylist& hlsPlaylist);
    std::string m_filename;
    TimePoint m_start_time;
    uint32_t m_media_sequence = 0;
    uint32_t m_target_duration = 10;
    HlsStartTag m_start_tag;
    std::vector<HlsSegment> m_segments;
};


using HlsPlaylistPtr = std::shared_ptr<HlsPlaylist>;

class PlaylistComposer {
public:
    PlaylistComposer(
        const std::string& folder,
        const TimePoint& beginTime, 
        const TimePoint& endTime);

    int find_playlists();
    int filter_playlists();
    int build_playlist();
    int fake_playlist();
    std::chrono::milliseconds last_scan_dir_duration();

private:
    int find_begin_pos();
    int find_end_pos();
    int copy_and_sort_segments();
    int update_playlist_info();
    
    std::string m_folder;
    TimePoint m_begin_time;
    TimePoint m_end_time;

    TimePoint m_scan_dir_time;
    uint32_t m_scan_interval_ms = 1000;
    
    std::vector<HlsPlaylistPtr> m_src_playlists;
    std::vector<HlsPlaylistPtr> m_selected_playlists;
    HlsPlaylistPtr m_snapshot_playlist;
};

std::ostream& operator<<(std::ostream& os, const HlsStartTag& hlsStartTag);

std::ostream& operator<<(std::ostream& os, const HlsSegment& hlsSegment);

std::ostream& operator<<(std::ostream& os, const HlsPlaylist& hlsPlaylist);
