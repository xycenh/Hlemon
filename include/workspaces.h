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
  static std::vector<std::string> windowsInWorkspace;

  std::string workspaceEvent = "";
  void exec();
  std::vector<std::string> parseString(std::string);
  void checkWorkspaceStatus(const std::vector<std::string> &);

  void checkWindows(const char &,const std::string &);

public:
  Workspaces();
  static std::string getWorkspaces();
  static std::string getWindows();
};

#endif // !WORKSPACES_H
