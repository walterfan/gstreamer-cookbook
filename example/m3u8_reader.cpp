#include <iostream>
#include <cstdlib>
#include <string>
#include "string_util.h"
#include "hls_util.h"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return EXIT_FAILURE;
    }

    const char* file_path = argv[1];

    HlsPlaylist playlist0(file_path);
    playlist0.read_m3u8_file();
    std::cout << playlist0 << std::endl;

    return 0;
}