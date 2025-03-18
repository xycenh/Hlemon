#include "functions.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include <cstdio>
#include <array>

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    FILE *pipe = popen(cmd, "r");
    if (!pipe) {
        std::cerr << "Failed to execute command." << std::endl;
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    if (pclose(pipe) == -1) {
        std::cerr << "Error closing pipe." << std::endl;
    }

    return result;
}

std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string readFile(const char *path) {
  std::ifstream file(path);
  std::string value;
  if (!file.is_open()) {
    std::cerr << "ERROR: Failed to open " << path << std::endl;
    return "N/A";
  }
  if (file.is_open()) {
    file >> value;
    file.close();
  }
  return value;
}

int readIntFile(const char *path) {
  std::ifstream file(path);
  int value = -1;
  if (file.is_open()) {
    file >> value;
    file.close();
  }
  return value;
}

std::string iconColor(const std::string icon) {
	return "%{F#6896a5}" + icon + "%{F-}";
} 


std::string truncateString(const std::string& str) {
    return (str.length() > 15) ? str.substr(0, 12) + "..." : str;
}
