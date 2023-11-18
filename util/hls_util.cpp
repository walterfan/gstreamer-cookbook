#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <tuple>
#include <dirent.h>
#include "string_util.h"
#include "time_util.h"
#include "hls_util.h"

static const std::string EXT_X_TARGETDURATION = "#EXT-X-TARGETDURATION";
static const std::string EXT_INF = "#EXTINF";
static const std::string TIME_OFFSET_PREFIX = "TIME-OFFSET=";
static const std::string PRECISE_PREFIX = "PRECISE=";
static const std::string PLAYLIST_PREFIX = "playlist_";
static const std::string PLAYLIST_SUFFIX = ".m3u8";

using sysclock = std::chrono::system_clock;

bool comparePlaylist(HlsPlaylistPtr& lhs, HlsPlaylistPtr& rhs) 
{ 
    return lhs->get_start_time() < rhs->get_start_time(); 
} 
//timestamp + sequence?
bool compareSegment(HlsSegment& lhs, HlsSegment& rhs) 
{ 
    return lhs.m_start_time < rhs.m_start_time; 
} 

//record_20231023170112_00043.ts -> 43
uint32_t HlsSegment::get_sequence() {
    size_t begin_pos = m_uri.rfind('_');
    size_t end_pos = m_uri.rfind('.');
    if (begin_pos > 0 && end_pos > begin_pos) {
        auto seq_str = m_uri.substr(begin_pos + 1, end_pos);
        return stoi(seq_str);
    }
    return 0;
}

int HlsSegment::update_uri(uint32_t seq) {
    size_t end_pos = m_uri.rfind('_');
    
    if (end_pos !=std::string::npos) {
        std::string seqnum =  "00000" + std::to_string(seq);
        m_uri = m_uri.substr(0, end_pos + 1) + seqnum.substr(seqnum.size() - 5) + ".ts"; 
        return 0;  
    }
    return -1;
}

HlsPlaylist::HlsPlaylist(const std::string& filename)
: m_filename(filename)
, m_media_sequence(0)
, m_target_duration(10) {
    size_t begin_pos = m_filename.rfind('_');
    size_t end_pos = m_filename.rfind('.');
    if (begin_pos > 0 && end_pos > begin_pos) {
        std::string time_str = m_filename.substr(begin_pos + 1, end_pos);
        m_start_time = get_timepoint(time_str);
    }
}

void HlsPlaylist::sort_segments() {
    std::sort(m_segments.begin(), m_segments.end(), compareSegment);
}

int HlsPlaylist::read_m3u8_file() {    
    std::ifstream file(m_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the M3U8 file:" << m_filename << std::endl;
        return -1;
    }

    std::string line;
    HlsSegment current_segment;
    bool is_in_segment = false;
    
    float total_duration = 0;
    //milliseconds
    TimePoint ts_start_time = m_start_time;

    while (std::getline(file, line)) {
        //std::cout << "m3u8 line: " << line << std::endl;
        if (line.find("#EXT-X-TARGETDURATION") != std::string::npos) {
            // Extract target duration
            size_t pos = line.find(':');
            m_target_duration = stoi(line.substr(pos + 1));
        } else if (line.find("#EXTINF") != std::string::npos) {
            // Extract segment duration
            size_t begin_pos = line.find(':');
            size_t end_pos = line.find(',');
            current_segment.m_duration = std::stof(line.substr(begin_pos + 1, end_pos));
            total_duration += current_segment.m_duration;
            
            current_segment.m_total_duration = total_duration;
            current_segment.m_start_time = ts_start_time;
            long long duration_ms = current_segment.m_duration * 1000;
            ts_start_time += std::chrono::milliseconds(duration_ms);
            is_in_segment = true;
        } else if (is_in_segment) {
            // Extract segment URL
            current_segment.m_uri = line;
            m_segments.push_back(current_segment);
            is_in_segment = false;
        } else if (line.find("#EXT-X-START") != std::string::npos) {
            // Extract #EXT-X-START tag
            size_t colon_pos = line.find(':');
            size_t time_offset_pos = line.find(TIME_OFFSET_PREFIX);
            size_t precise_pos = line.find(PRECISE_PREFIX);
            size_t commaPos = line.find(',');
            if (colon_pos > 0 && time_offset_pos > colon_pos && commaPos > time_offset_pos) {
                m_start_tag.m_time_offset 
                    = std::stof(line.substr(time_offset_pos + TIME_OFFSET_PREFIX.size(), commaPos));
            }
            if (precise_pos > commaPos) {
                m_start_tag.m_precise 
                    = line.substr(precise_pos + PRECISE_PREFIX.size());
            }
            
        }
    }
    file.close();
    return m_segments.size();
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
int HlsPlaylist::write_m3u8_file() {
//int write_m3u8_file(const std::string& filename, const HlsPlaylist& hlsPlaylist) {
    std::ofstream file(m_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return -1;
    }

    // Write the #EXTM3U header
    file << "#EXTM3U" << std::endl;

    // Write the #EXT-X-VERSION tag
    file << "#EXT-X-VERSION:3" << std::endl;

    // Write the #EXT-X-MEDIA-SEQUENCE tag
    file << "#EXT-X-MEDIA-SEQUENCE:" << m_media_sequence << std::endl;

    // Write the #EXT-X-TARGETDURATION tag
    file << "#EXT-X-TARGETDURATION:" << m_target_duration << std::endl;

    // Write the #EXT-X-START tag
    file << "#EXT-X-START:TIME-OFFSET=" << m_start_tag.m_time_offset 
    << ",PRECISE=" << m_start_tag.m_precise << std::endl;

    // Write the segment information
    for (const HlsSegment& segment : m_segments) {
        file << "#EXTINF:" << segment.m_duration << "," << std::endl; // Segment duration
        file << segment.m_uri << std::endl; // Segment URL
    }
    std::cout << "wrote m3u8 file:" << m_filename << std::endl;
    file.close();
    return 0;
}

std::ostream& operator<<(std::ostream& os, const HlsStartTag& hlsStartTag) {
    os << "HlsStartTag: m_time_offset=" << hlsStartTag.m_time_offset;
    os << ", m_time_offset="<< hlsStartTag.m_precise;
    return os;
}

std::ostream& operator<<(std::ostream& os, const HlsSegment& hlsSegment) {
    os << "HlsSegment: m_duration=" << hlsSegment.m_duration;
    os << ", m_uri="<< hlsSegment.m_uri;
    os << ", m_total_duration=" << hlsSegment.m_total_duration;
    os << ", m_start_time=" << get_time_str(hlsSegment.m_start_time);
    return os;
}

std::ostream& operator<<(std::ostream& os, const HlsPlaylist& hlsPlaylist) {
    os << "Playlist: m_filename=" << hlsPlaylist.m_filename;
    os << ", m_start_time=" << get_time_str(hlsPlaylist.m_start_time);
    os << ", m_media_sequence="<< hlsPlaylist.m_media_sequence;
    os << ", m_target_duration="<< hlsPlaylist.m_target_duration;
    os << ", m_start_tag=" << hlsPlaylist.m_start_tag;
    os << ", m_segments: ";
    for(auto& segment: hlsPlaylist.m_segments) {
        os << "\n" << segment;
    }
    os << std::endl;
    return os;
}

 PlaylistComposer::PlaylistComposer(
    const std::string& folder,
    const TimePoint& beginTime, 
    const TimePoint& endTime)
    : m_folder(folder)
    , m_begin_time(beginTime)
    , m_end_time(endTime) {

}

std::chrono::milliseconds PlaylistComposer::last_scan_dir_duration() {
    auto now = sysclock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_scan_dir_time);
}
/*
int PlaylistComposer::find_playlists() {
    if (last_scan_dir_duration().count() < m_scan_interval_ms) {
        return m_src_playlists.size(); 
    }

    m_scan_dir_time = sysclock::now();
    // Check if the directory exists
    if (!std::filesystem::is_directory(m_folder)) {
        std::cerr << "Directory not found." << std::endl;
        return -1;
    }

    // Iterate through the files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(m_folder)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            if (startswith(filename, PLAYLIST_PREFIX) 
                && endswith(filename, PLAYLIST_SUFFIX)) {
                auto playlist = std::make_shared<HlsPlaylist>(entry.path().string());
                int read_ret = playlist->read_m3u8_file();
                if (read_ret < 0) {
                    std::cerr << "read error of file " << filename << std::endl;
                }
                m_src_playlists.push_back(playlist);
            }
        }
    }

    if (m_src_playlists.empty()) {
        return 0;
    }
    // Sort the file as timestamp in the filename
    std::sort(m_src_playlists.begin(), m_src_playlists.end(), comparePlaylist);

    return m_src_playlists.size();
}
*/
int PlaylistComposer::find_playlists() {
    if (last_scan_dir_duration().count() < m_scan_interval_ms) {
        return m_src_playlists.size();
    }

    m_scan_dir_time = sysclock::now();

    struct dirent* direntp;
    // Open the directory
    DIR* dirp = opendir(m_folder.c_str());

    if (!dirp) {
        std::cerr << "Directory not found." << m_folder << std::endl;
        return -1;
    }

    while( NULL != (direntp = readdir(dirp))) {
        std::string filename = direntp->d_name;
        if(".." == filename || "." == filename)
            continue;

        if (startswith(filename, PLAYLIST_PREFIX)
                        && endswith(filename, PLAYLIST_SUFFIX)) {
            std::string strPath = m_folder;
            strPath.append("/");
            strPath.append(filename);

            auto playlist = std::make_shared<HlsPlaylist>(strPath);
            int read_ret = playlist->read_m3u8_file();
            if (read_ret < 0) {
                std::cerr << "read error of file " << filename << std::endl;
                
            }
            m_src_playlists.push_back(playlist);
        }
    }

    while((-1 == closedir(dirp)) && (errno == EINTR));


    if (m_src_playlists.empty()) {
        return 0;
    }
    // Sort the file as timestamp in the filename
    std::sort(m_src_playlists.begin(), m_src_playlists.end(), comparePlaylist);

    return m_src_playlists.size();
}

int PlaylistComposer::find_begin_pos() {

    if (m_src_playlists.empty()) {
        throw std::runtime_error("The playlists is empty.");
        return -1;
    }
    std::time_t begin_time = sysclock::to_time_t(m_begin_time);
    std::time_t diff = begin_time
        - sysclock::to_time_t(m_src_playlists[0]->get_start_time()); 
    
    if (diff < 0) {
        return -1;
    } else if (diff == 0) {
        return 0;
    }

    int i = 1;
      
    for (; i < m_src_playlists.size(); ++i) {
        diff = begin_time
         - sysclock::to_time_t(m_src_playlists[i]->get_start_time()); 

        if (diff > 0) {
            continue;
        } else if (diff == 0) {
            return i;
        } else if (diff < 0) {
            return i -  1;
        }
    }
    //return the pos of last element
    return i-1;
}

int PlaylistComposer::find_end_pos() {
    size_t size = m_src_playlists.size();
    if (size == 0) {
        throw std::runtime_error("The playlists is empty.");
        return -2;
    }
    
    std::time_t end_time = sysclock::to_time_t(m_end_time);
    // the playlist's time should be greater than m_end_time
    std::time_t diff = sysclock::to_time_t
        (m_src_playlists[size - 1]->get_start_time()) -  end_time; 
    
    if (diff < 0) {
        return -1;
    } else if (diff == 0) {
        return size - 1;
    }

    int i = size  - 2;
      
    for (; i >= 0; --i) {
        diff = sysclock::to_time_t(m_src_playlists[i]->get_start_time()) -  end_time; 

        if (diff > 0) {
            continue;
        } else if (diff == 0) {
            return i;
        } else if (diff < 0) {
            return i + 1;
        }
    }
    return i;
}


int PlaylistComposer::filter_playlists()
{
    int begin_pos = find_begin_pos();
    int end_pos = find_end_pos();
    if (-1 == end_pos) {
        end_pos = m_src_playlists.size();
    }
    if (begin_pos < 0 || end_pos < 0 || begin_pos > end_pos) {
        std::cerr << "incorrect begin_pos=" << begin_pos 
            << ", end_pos=" << end_pos << std::endl;
        return -1;
    }
    std::cout << "DEBUG: correct begin_pos=" << begin_pos 
            << ", end_pos=" << end_pos << std::endl;

    for(int i = begin_pos; i < end_pos; ++i) {
        m_selected_playlists.push_back(m_src_playlists[i]);
        std::cout << i << ". DEBUG: m_selected_playlists=" << *m_src_playlists[i] << std::endl;
    }
    return end_pos - begin_pos;
}

int PlaylistComposer::copy_and_sort_segments() {
    if (m_selected_playlists.empty()) {
        std::cerr << "m_dest_playlists is empty" << std::endl;
        return -1;
    }
    if (!m_snapshot_playlist) {
        std::cerr << "m_snapshot_playlist is null" << std::endl;
        return -2;
    }
    //check segment's start time, keep that 
    //segment_time >=m_begin_time and segment_time < m_end_time
    for (auto& playListPtr: m_selected_playlists) {
        for(auto& segment: playListPtr->get_segments()) {
            //std::cout << "DEBUG check segment=" << segment << std::endl; 
            if (segment.m_start_time + std::chrono::seconds(10) >= m_begin_time
             && segment.m_start_time < m_end_time) {
                m_snapshot_playlist->append_segment(segment);
             }
        }
    }
    m_snapshot_playlist->sort_segments();
    return 0;
}
int PlaylistComposer::build_playlist() {
    if (m_selected_playlists.empty()) {
        std::cerr << "m_dest_playlists is empty" << std::endl;
        return -1;
    }
    std::string filename = get_format_time(m_begin_time, "playlist_{}.m3u8");
    m_snapshot_playlist = std::make_shared<HlsPlaylist>(filename);
    
    int ret = copy_and_sort_segments();
    if (ret < 0) {
        std::cerr << "copy_and_sort_segments error" << std::endl;
        return ret;
    }
    
    ret = update_playlist_info();
    if (ret < 0) {
        std::cerr << "update_playlist_info error" << std::endl;
        return ret;
    }
    return 0;
}

int PlaylistComposer::update_playlist_info()
{
    m_snapshot_playlist->set_target_duration(10);
    //set media sequence as the fist segment name
    if (m_snapshot_playlist->get_segments().empty()) {
        std::cerr << "m_snapshot_playlist segments is empty" << std::endl;
        return -2;
    }
    auto& firstSegment = m_snapshot_playlist->get_segments()[0];
    m_snapshot_playlist->set_media_sequence(firstSegment.get_sequence());

    //set timeoffset if m_begin_time > 1st segment_time
    if (m_begin_time > firstSegment.m_start_time) {
        auto diff = m_begin_time - firstSegment.m_start_time;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        m_snapshot_playlist->set_time_offset(diff_ms.count());
    }
    return 0;
}

int PlaylistComposer::fake_playlist() {
    if (m_src_playlists.empty()) {
        return -1;
    }

    std::string filename = get_format_time(m_begin_time, "playlist_{}.m3u8");
    m_snapshot_playlist = std::make_shared<HlsPlaylist>(filename);

    auto last_playlist = m_src_playlists[m_src_playlists.size() - 1];
    auto& segments = last_playlist->get_segments();
    auto& last_segment = segments[segments.size() - 1];
    //find the last segment of last playlist
    TimePoint latestStartTime = last_segment.m_start_time;
    TimePoint latestEndTime = last_segment.m_start_time 
        + std::chrono::milliseconds(static_cast<uint32_t>(last_segment.m_duration * 1000));
    //TimePoint beginTime;
    uint32_t begin_seq = 0;
    int32_t  end_seq = 0;

    auto seq = last_segment.get_sequence();
    auto uri = last_segment.m_uri;
    auto duration = m_begin_time - latestEndTime;
    //auto duration = beginTime - latestTime;
    auto segment_time = latestStartTime;
    while(segment_time <= m_end_time) {
        segment_time += std::chrono::seconds(10);
        if (begin_seq == 0 && segment_time > m_begin_time) {
            begin_seq = seq;
        }
        if (segment_time > m_end_time) {
            end_seq = seq;
        }
        seq++;
    }
    
    std::cout << "begin_seq="<< begin_seq << ", end_seq=" << end_seq << std::endl;
    for(int i=begin_seq; i < end_seq; ++i) {
        auto fake_segment = HlsSegment(10, uri);
        fake_segment.update_uri(i);
        m_snapshot_playlist->append_segment(fake_segment);
    }
    
    return update_playlist_info();
}