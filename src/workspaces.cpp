#include "workspaces.h"
#include "lemonOutput.h"
#include <string>
#include <vector>

std::vector<std::string> Workspaces::shownWorkspaces = {};

Workspaces::Workspaces() {
    exec();
}

void Workspaces::exec() {
  FILE *pipe = popen("bspc subscribe", "r");
  if (!pipe) {
    std::cerr << "Failed to run bspc subscribe command." << std::endl;
    return;
  }

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe)) {
    workspaceEvent = buffer;
    if (!workspaceEvent.empty() && workspaceEvent.back() == '\n') {
      workspaceEvent.pop_back();
    }
    std::vector<std::string> workspaces = parseString(workspaceEvent);
    checkWorkspaceStatus(workspaces);
    lemonOutput();
  }
  pclose(pipe);
}

std::vector<std::string> Workspaces::parseString(std::string &s) {
  std::vector<std::string> workspaces;
  std::istringstream iss(s);
  std::string token;
  std::vector<std::string> parts;

  while (std::getline(iss, token, ':')) {
    parts.push_back(token);
  }

  size_t numParts = parts.size();
  /*if (numParts < 4) {*/
  /*  return workspaces;*/
  /*}*/

  for (size_t i = 1; i < numParts; ++i) {
    if (parts[i] == "LM") {
      break;
    }
    workspaces.push_back(parts[i]);
  }
  return workspaces;
}

void Workspaces::checkWorkspaceStatus(const std::vector<std::string> &workspaces) {
    shownWorkspaces = {};
    for (const std::string &ws : workspaces) {
        if (ws.empty())
            continue;

        char status = ws[0];
        std::string name = ws.substr(1);

        if (status == 'O' || status == 'o' || status == 'F') {
        shownWorkspaces.push_back(name);
        }
    }

}

std::string Workspaces::getWorkspaces() {
    std::string result = "";
    for (const std::string name : shownWorkspaces) {
        result += name + " ";
    }
    return result;
}
