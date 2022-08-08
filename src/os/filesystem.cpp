#include "filesystem.h"

#include <fstream>
#include <sstream>
#include <string>

std::string ReadFileText(const char *file_path) {
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}