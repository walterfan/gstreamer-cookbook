#include <gst/gst.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <ctime>   // localtime
#include <iomanip> // put_time

static const char* DEFAUT_TIME_FMT = "%y%m%d_%H%M%S";

struct DeviceManager {
    int check_audio_device();

    GMainLoop* m_loop;
    GstDeviceMonitor* m_monitor;
};

int DeviceManager::check_audio_device()
{
    return 0;
    
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "Usage  : " << argv[0] 
            << " --audio_src=osxaudiosrc|alsasrc|audiotestsrc"
            << " --device=<device_id>"
            << " --audio_sink=<splitmuxsink|hlssink2>" 
            << std::endl;
        std::cout << "Example: " << argv[0] 
            << " --audio_src=osxaudiosrc"
            << " --device=1832"
            << " --audio_sink=hlssink2" 
            << std::endl;
        return 1; 
    }

    std::map<std::string, std::string> arguments;


    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check if the argument is of the form "--name=value"
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string name = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            arguments[name] = value;
        } else {
            // Handle other types of arguments if needed
            std::cout << "Unknown argument: " << arg << std::endl;
        }
    }


    
    return 0;
}

