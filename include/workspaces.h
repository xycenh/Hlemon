#ifndef WORKSPACES_H
#define WORKSPACES_H

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Workspaces {
private:
  static std::vector<std::string> shownWorkspaces;
  std::string workspaceEvent = "";
  void exec();
  std::vector<std::string> parseString(std::string);
  void checkWorkspaceStatus(const std::vector<std::string> &);

public:
  Workspaces();
  static std::string getWorkspaces();
};

#endif // !WORKSPACES_H
