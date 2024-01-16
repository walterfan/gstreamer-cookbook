#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <ctime>
#include <iomanip> // for std::put_time
#include <sstream> // for std::ostringstream
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
static const std::string BLANK_STR = "";
static const char* HARD_TIME_FMT = "%Y%m%d%H%M%S";
static const char* EASY_TIME_FMT = "%y-%m-%d_%H-%M-%S";

static std::string extractTimeStr(const std::string& filename) {
    size_t begin_pos = filename.find('_');
    size_t mid_pos = filename.rfind('_');
    size_t end_pos = filename.rfind('.');
    
    if (begin_pos > 0 && mid_pos > 0 && mid_pos > begin_pos) {
        return filename.substr(begin_pos + 1, mid_pos);        
    }

    if (begin_pos > 0 && end_pos > 0 && end_pos > begin_pos) {
        return filename.substr(begin_pos + 1, end_pos);        
    }
    
    return BLANK_STR;
}

static std::string convertTimeFormat(const std::string& inputTime, 
    const std::string& inputFormat = HARD_TIME_FMT, 
    const std::string& outputFormat = EASY_TIME_FMT) {
    std::tm timeStruct = {};
    if (strptime(inputTime.c_str(), inputFormat.c_str(), &timeStruct) == nullptr) {
        std::cerr << "Error parsing input time: " << inputTime << std::endl;
        return ""; // Handle error
    }

    char outputTime[50]; // Adjust the size based on your expected output
    strftime(outputTime, sizeof(outputTime), outputFormat.c_str(), &timeStruct);
    
    return std::string(outputTime);
}

static std::string getCurrentTimeStr() {
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), EASY_TIME_FMT);
    return oss.str();
}

static std::string getFileTimeStr(const std::string& filename) {
    std::string timestr = extractTimeStr(filename);
    if (timestr.empty()) {
        return BLANK_STR;
    }
    return convertTimeFormat(timestr);
}

static int check_event(int fd, int timeout) {
    struct pollfd pfd = { fd, POLLIN, 0 };
    return poll(&pfd, 1, timeout);  // timeout of 50ms
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return EXIT_FAILURE;
    }

    const char* directoryPath = argv[1];

    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    // Initialize inotify
    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Add a watch for the specified directory
    wd = inotify_add_watch(fd, directoryPath, IN_CREATE | IN_DELETE | IN_MOVED_TO);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    std::cout << "Monitoring folder: " << directoryPath << std::endl;

    // Monitor events
    while (true) {
        length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        while (i < length) {
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->len) {

                if (event->mask & IN_CREATE) {
                    std::cout << getCurrentTimeStr() << ": File created: " << event->name 
                        << " at " << getFileTimeStr(event->name)<< std::endl;
                } else if (event->mask & IN_DELETE) {
                    std::cout << getCurrentTimeStr() << ": File deleted: " << event->name 
                         << " at " << getFileTimeStr(event->name)<< std::endl;
                } else if (event->mask & IN_MOVED_TO) {
                    std::cout << getCurrentTimeStr() << ": File moved to: " << event->name
                         << " at " << getFileTimeStr(event->name)<< std::endl;
                }
            }
            i += EVENT_SIZE + event->len;
        }
        i = 0;  // Reset index
    }

    // Clean up
    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}
