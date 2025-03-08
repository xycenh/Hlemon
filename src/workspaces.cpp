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
		std::cout << workspaceEvent.substr(1) << std::endl;
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
		std::string activeColor = "#222222";
		std::string focusedColor = "#393939";
		std::string color = "";
		bool flag;
		for (size_t i = 0; i < workspaces.size(); ++i) {
				const std::string &ws = workspaces[i];
        if (ws.empty())
            continue;

				if (ws == "LM") {
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
            name = "%{A:sh -c \"bspc desktop -f \'" + name + "\'\":}%{B" + color + "}%{O10}" + name + "%{O10}%{B-}%{A}";
            shownWorkspaces.push_back(name);
        }
        else if (status == 'o') {
            name = "%{A:sh -c \"bspc desktop -f \'" + name + "\'\":}%{O2}" + name + "%{O2}%{A}";
            shownWorkspaces.push_back(name);
        }
    }
}

std::string Workspaces::getWorkspaces() {
    std::string result = "";
    for (const std::string name : shownWorkspaces) {
				if (name == "LM") {
					result += "%{F#777777}|%{F-} ";
					continue;
				}
        result += name + " ";
    }
    return result;
}
