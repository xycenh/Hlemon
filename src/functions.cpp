#include "functions.h"
#include <fstream>
#include <iostream>

std::string exec(const char *cmd) {
  std::string result;
  FILE *pipe = popen(cmd, "r");
  if (!pipe)
    return "Error in command";

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
