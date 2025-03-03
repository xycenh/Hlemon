#include "functions.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

std::string exec(const char *cmd) {
  std::string result;
  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    std::cerr << "Failed to execute command." << std::endl;
    return "";
  }
  char ch;
  while (fread(&ch, sizeof(ch), 1, pipe))
    result += ch;

  pclose(pipe);
  return result;
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


std::string trim(const std::string& str) {
    // Find the first non-whitespace character
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);

    // Find the last non-whitespace character
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

    // Return the trimmed string
    return (start < end) ? std::string(start, end) : std::string();
}

std::string iconColor(const std::string icon) {
	return "%{F#6896a5}" + icon + "%{F-}";
} 
