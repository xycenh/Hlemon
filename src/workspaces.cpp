#include "workspaces.h"
#include "lemonOutput.h"
#include "functions.h"
#include <ostream>
#include <string>
#include <vector>
#include <cstdio>

std::vector<std::string> Workspaces::shownWorkspaces = {};
std::vector<std::string> Workspaces::windowsInWorkspace = {};

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
    std::vector<std::string> workspaces = parseString(workspaceEvent.substr(1));
    checkWorkspaceStatus(workspaces);
    updateLemonbar(lemonOutput());
  }
  pclose(pipe);
}

std::vector<std::string> Workspaces::parseString(std::string s) {
  std::vector<std::string> workspaces;
  std::istringstream iss(s);
  std::string token;
  std::vector<std::string> parts;

  while (std::getline(iss, token, ':')) {
    parts.push_back(token);
  }

  size_t numParts = parts.size();

  for (size_t i = 0; i < numParts; ++i) {
    workspaces.push_back(parts[i]);
  }
  return workspaces;
}

void Workspaces::checkWorkspaceStatus(const std::vector<std::string> &workspaces) {
    shownWorkspaces = {};
    windowsInWorkspace = {};
    std::string activeColor = "#222222";
    std::string focusedColor = "#393939";
    std::string color = "";
    bool flag;
    for (size_t i = 0; i < workspaces.size(); ++i) {
            const std::string &ws = workspaces[i];

        if (ws.empty())
            continue;

        if (ws == "LM" || ws == "LT") {
            flag = false;
            
            if (i + 2 > workspaces.size() || i + 4 > workspaces.size()) {
                flag = true;
            }
            if (flag) 
                break;	
            else
                shownWorkspaces.push_back(ws);
            continue;
        }

        if (ws[0] == 'm') {
            color = activeColor;
            continue;
        }
        if (ws[0] == 'M') {
            color = focusedColor;
            continue;
        }

        char status = ws[0];
        std::string name = ws.substr(1);

        if (status == 'O' || status == 'F') {
            checkWindows(status, name);
            name = "%{A:sh -c \"bspc desktop -f \'" + name + "\'\":}%{B" + color + "}%{O10}" + name + "%{O10}%{B-}%{A}";
            shownWorkspaces.push_back(name);
        }
        else if (status == 'o') {
            name = "%{A:sh -c \"bspc desktop -f \'" + name + "\'\":}%{O2}" + name + "%{O2}%{A}";
            shownWorkspaces.push_back(name);
        }
    }
}

void Workspaces::checkWindows(const char &status,const std::string &workspace) {
    if (status == 'F') return;
    std::string focusedColor = "#393939";
    std::string color = "#000000";

    std::string windows_id_command = "bspc query -N -d " + workspace;
    std::string windows_id = trim(::exec(windows_id_command.c_str()));
    std::vector<std::string> window_ids;
    size_t pos_1 = 0;
    size_t found_1;
    while ((found_1 = windows_id.find('\n', pos_1)) != std::string::npos) {
        window_ids.push_back(windows_id.substr(pos_1, found_1 - pos_1));  
        pos_1 = found_1 + 1;  
    }
    window_ids.push_back(windows_id.substr(pos_1));

    std::vector<std::string> window_classes;
    
    for (auto it = window_ids.begin(); it != window_ids.end();) {
        std::string command = "xprop -id " + *it + " WM_CLASS 2>/dev/null | awk -F '\"' 'NF>1 {print $(NF-1)}'";
        std::string window_class = trim(::exec(command.c_str()));

        if (!window_class.empty()) {
            window_classes.push_back(window_class);
            ++it;
        } else {
            it = window_ids.erase(it);
        }
    }
    
    if (window_ids.size() != window_classes.size()) 
        std::cout << "error happened!" << std::endl;

    std::string focused_window_id_command = "bspc query -N -n focused";
    std::string focused_window_id = trim(::exec(focused_window_id_command.c_str()));

    for (size_t i = 0; i < window_ids.size(); i++) {
        if (focused_window_id == window_ids[i]) color = focusedColor;
        else color = "#000000";

        windowsInWorkspace.push_back("%{B" + color + "}%{O10}" + truncateString(window_classes[i]) + "%{O10}%{B-}");
    }
}

std::string Workspaces::getWindows() {
    std::string result = "";
    for (const std::string name : windowsInWorkspace) {
        result += name ;
    }
    if (result != "") return "%{F#777777}|%{F-} " + result;
    return result;
}

std::string Workspaces::getWorkspaces() {
    std::string result = "";
    for (const std::string name : shownWorkspaces) {
				if (name == "LM" || name == "LT") {
					result += "%{F#777777}|%{F-} ";
					continue;
				}
        result += name + " ";
    }
    return result;
}
