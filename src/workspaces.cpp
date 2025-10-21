#include "workspaces.h"
#include "lemonOutput.h"
#include "functions.h"
#include <cmath>
#include <ostream>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <xcb/randr.h>
#include <xcb/xproto.h>

std::vector<std::string> Workspaces::shownWorkspaces = {};
std::vector<std::string> Workspaces::windowsInWorkspace = {};

Workspaces::Workspaces() {
    exec();
    // Xsession xsession;
}

Xsession::Xsession() {
    xcb_connection_t* conn = get_x11_conn();
    xcb_ewmh_connection_t ewmh = get_ewmh_conn(conn);
    xcb_screen_t* screen = get_screen(conn);
    subscribe_event_for_root_window(conn, screen);
    std::vector<Monitor> monitors = get_monitors(conn, screen);
    event_loop(conn, &ewmh, monitors, screen);
    xcb_ewmh_connection_wipe(&ewmh);
    xcb_disconnect(conn);
}

// Get window class
std::string get_window_class(xcb_connection_t* conn, xcb_window_t win) {
    xcb_icccm_get_wm_class_reply_t class_reply;
    if (xcb_icccm_get_wm_class_reply(conn, xcb_icccm_get_wm_class(conn, win), &class_reply, nullptr)) {
        std::string result = class_reply.class_name ? std::string(class_reply.class_name) : "Unknown";
        xcb_icccm_get_wm_class_reply_wipe(&class_reply);
        return result;
    }
    return "Unknown";
}

// Get the root window of a given window
xcb_window_t get_window_root(xcb_connection_t* conn, xcb_window_t win) {
    xcb_query_tree_reply_t* reply = xcb_query_tree_reply(conn, xcb_query_tree(conn, win), nullptr);
    if (reply) {
        xcb_window_t root = reply->root;
        free(reply);
        return root;
    }
    return XCB_WINDOW_NONE;
}

// Get current desktop index for a screen
uint32_t get_current_desktop(xcb_ewmh_connection_t* ewmh, int screen_idx) {
    uint32_t desktop = 0;
    xcb_ewmh_get_current_desktop_reply(ewmh,
        xcb_ewmh_get_current_desktop(ewmh, screen_idx), &desktop, nullptr);
    return desktop;
}

// Get active window for a screen
xcb_window_t get_active_window(xcb_ewmh_connection_t* ewmh, int screen_idx) {
    xcb_window_t win = 0;
    xcb_ewmh_get_active_window_reply(ewmh,
        xcb_ewmh_get_active_window(ewmh, screen_idx), &win, nullptr);
    return win;
}

// Get desktops that have windows
std::vector<uint32_t> get_desktops_with_windows(xcb_ewmh_connection_t* ewmh) {
    std::vector<uint32_t> desktops_with_windows;
    xcb_ewmh_get_windows_reply_t clients;
    
    if (!xcb_ewmh_get_client_list_reply(ewmh, xcb_ewmh_get_client_list(ewmh, 0), &clients, nullptr))
        return desktops_with_windows;

    for (unsigned i = 0; i < clients.windows_len; ++i) {
        xcb_window_t win = clients.windows[i];
        uint32_t desktop = 0;
        xcb_ewmh_get_wm_desktop_reply(ewmh, xcb_ewmh_get_wm_desktop(ewmh, win), &desktop, nullptr);
        
        if (std::find(desktops_with_windows.begin(), desktops_with_windows.end(), desktop) == desktops_with_windows.end()) {
            desktops_with_windows.push_back(desktop);
        }
    }

    xcb_ewmh_get_windows_reply_wipe(&clients);
    std::sort(desktops_with_windows.begin(), desktops_with_windows.end());
    return desktops_with_windows;
}

// Build workspaces output for lemonbar
std::string build_workspaces_output(xcb_ewmh_connection_t* ewmh, uint32_t focused_desktop) {
    xcb_ewmh_get_utf8_strings_reply_t names_reply;
    if (!xcb_ewmh_get_desktop_names_reply(ewmh, xcb_ewmh_get_desktop_names(ewmh, 0), &names_reply, nullptr))
        return "";

    std::vector<std::string> names_vec;
    size_t offset = 0;
    for (unsigned i = 0; i < names_reply.strings_len; ++i) {
        if (names_reply.strings[i] == '\0') {
            std::string name(&names_reply.strings[offset], i - offset);
            // Remove newline characters
            name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
            names_vec.push_back(name);
            offset = i + 1;
        }
    }
    if (offset < names_reply.strings_len) {
        std::string name(&names_reply.strings[offset], names_reply.strings_len - offset);
        // Remove newline characters
        name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
        names_vec.push_back(name);
    }
    xcb_ewmh_get_utf8_strings_reply_wipe(&names_reply);

    // Get only desktops that have windows
    std::vector<uint32_t> desktops_with_windows = get_desktops_with_windows(ewmh);
    
    // Always include the focused desktop
    if (std::find(desktops_with_windows.begin(), desktops_with_windows.end(), focused_desktop) == desktops_with_windows.end()) {
        desktops_with_windows.push_back(focused_desktop);
    }
    
    // Sort to maintain order
    std::sort(desktops_with_windows.begin(), desktops_with_windows.end());

    std::string result = "";
    std::string focusedColor = "#45475a";

    for (uint32_t idx : desktops_with_windows) {
        std::string ws_name = (idx < names_vec.size()) ? names_vec[idx] : "Desktop " + std::to_string(idx);
        
        std::string formatted;
        if (idx == focused_desktop) {
            // Only focused workspace gets background color
            formatted = "%{A:bspc desktop -f '" + ws_name + "':}%{B" + focusedColor + "}%{O10}" + ws_name + "%{O10}%{B-}%{A}";
        } else {
            // Other workspaces have no background
            formatted = "%{A:bspc desktop -f '" + ws_name + "':}%{O10}" + ws_name + "%{O10}%{A}";
        }
        result += formatted;
    }

    return result;
}

std::string build_windows_output(xcb_ewmh_connection_t* ewmh, xcb_window_t focused_win, uint32_t current_desktop) {
    xcb_ewmh_get_windows_reply_t clients;
    if (!xcb_ewmh_get_client_list_reply(ewmh, xcb_ewmh_get_client_list(ewmh, 0), &clients, nullptr))
        return "";

    std::string focusedColor = "#45475a";
    std::string result = "";

    for (unsigned i = 0; i < clients.windows_len; ++i) {
        xcb_window_t win = clients.windows[i];
        uint32_t desktop = 0;
        xcb_ewmh_get_wm_desktop_reply(ewmh, xcb_ewmh_get_wm_desktop(ewmh, win), &desktop, nullptr);

        // Only show windows in current desktop
        if (desktop != current_desktop)
            continue;

        std::string cls = get_window_class(ewmh->connection, win);
        cls.erase(std::remove(cls.begin(), cls.end(), '\n'), cls.end());

        // Normalize class names
        if (cls == "com.mitchellh.ghostty")
            cls = "ghostty";
        else if (cls == "TelegramDesktop")
            cls = "telegram";

        std::string win_id_str = std::to_string(win);

        // Clickable areas: left-click to focus, right-click to close
        std::string click_tag = "%{A:bspc node -f " + win_id_str + ":}%{A3:bspc node " + win_id_str + " -c:}";

        std::string formatted;
        if (win == focused_win) {
            formatted = click_tag + "%{B" + focusedColor + "}%{O10}" + truncateString(cls) + "%{O10}%{B-}%{A}%{A}";
        } else {
            formatted = click_tag + "%{O10}" + truncateString(cls) + "%{O10}%{B-}%{A}%{A}";
        }

        result += formatted;
    }

    xcb_ewmh_get_windows_reply_wipe(&clients);
    return result;
}

// Update lemonbar with current state
void Xsession::update_display(xcb_connection_t* conn, xcb_ewmh_connection_t* ewmh, 
                               const std::vector<Monitor>& monitors, xcb_window_t root) {
    int screen_idx = 0;
    xcb_window_t active_win = get_active_window(ewmh, screen_idx);
    uint32_t focused_desktop = get_current_desktop(ewmh, screen_idx);

    // Get focused monitor
    int focused_monitor = 1;
    if (active_win != XCB_WINDOW_NONE) {
        xcb_translate_coordinates_reply_t* trans = xcb_translate_coordinates_reply(
            conn, xcb_translate_coordinates(conn, active_win, root, 0, 0), nullptr);
        if (trans) {
            int abs_x = trans->dst_x;
            int abs_y = trans->dst_y;
            free(trans);

            xcb_get_geometry_reply_t* geo = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, active_win), nullptr);
            if (geo) {
                int center_x = abs_x + geo->width / 2;
                int center_y = abs_y + geo->height / 2;
                free(geo);

                int mon_idx = 1;
                for (const auto& mon : monitors) {
                    if (center_x >= mon.x && center_x < mon.x + mon.width &&
                        center_y >= mon.y && center_y < mon.y + mon.height) {
                        focused_monitor = mon_idx;
                        break;
                    }
                    mon_idx++;
                }
            }
        }
    } else {
        xcb_query_pointer_reply_t* ptr = xcb_query_pointer_reply(conn, xcb_query_pointer(conn, root), nullptr);
        if (ptr) {
            int mouse_x = ptr->root_x;
            int mouse_y = ptr->root_y;
            free(ptr);

            int mon_idx = 1;
            for (const auto& mon : monitors) {
                if (mouse_x >= mon.x && mouse_x < mon.x + mon.width &&
                    mouse_y >= mon.y && mouse_y < mon.y + mon.height) {
                    focused_monitor = mon_idx;
                    break;
                }
                mon_idx++;
            }
        }
    }

    // Build output
    std::string workspaces_output = build_workspaces_output(ewmh, focused_desktop);
    std::string windows_output = build_windows_output(ewmh, active_win, focused_desktop);

    // Format final output: workspaces | windows
    std::string output = workspaces_output;
    if (!windows_output.empty()) {
        output += " %{F#7f849c}|%{F-} " + windows_output;
    }

    // Store in static members for getter access
    Workspaces::shownWorkspaces = {workspaces_output};
    Workspaces::windowsInWorkspace = {windows_output};

    updateLemonbar(lemonOutput());
}

xcb_connection_t* Xsession::get_x11_conn() {
    xcb_connection_t* conn = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(conn))
        throw std::runtime_error("Failed to connect to X server");
    return conn;
}

xcb_ewmh_connection_t Xsession::get_ewmh_conn(xcb_connection_t* conn) {
    xcb_ewmh_connection_t ewmh;
    if (!xcb_ewmh_init_atoms_replies(&ewmh, xcb_ewmh_init_atoms(conn, &ewmh), nullptr))
        throw std::runtime_error("Failed to init EWMH atoms");
    return ewmh;
}

void Xsession::event_loop(xcb_connection_t* conn, xcb_ewmh_connection_t* ewmh, std::vector<Monitor> monitors, xcb_screen_t* screen) {
    xcb_generic_event_t* event;
    while ((event = xcb_wait_for_event(conn))) {
        uint8_t type = event->response_type & ~0x80;
        bool update = false;

        if (type == XCB_PROPERTY_NOTIFY) {
            auto* e = (xcb_property_notify_event_t*)event;
            if (e->atom == ewmh->_NET_ACTIVE_WINDOW ||
                e->atom == ewmh->_NET_CLIENT_LIST ||
                e->atom == ewmh->_NET_CURRENT_DESKTOP) {
                update = true;
            }
        } else if (type == XCB_CREATE_NOTIFY || type == XCB_DESTROY_NOTIFY ||
                   type == XCB_MAP_NOTIFY || type == XCB_UNMAP_NOTIFY) {
            update = true;
        }

        if (update) {
            update_display(conn, ewmh, monitors, screen->root);
        }
        free(event);
    }
}

xcb_screen_t* Xsession::get_screen(xcb_connection_t* conn) {
    return xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
}

void Xsession::subscribe_event_for_root_window(xcb_connection_t* conn, xcb_screen_t* screen) {
    uint32_t mask = XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, &mask);
    xcb_flush(conn);
}

std::vector<Monitor> Xsession::get_monitors(xcb_connection_t* conn, xcb_screen_t* screen) {
    std::vector<Monitor> monitors;
    xcb_randr_get_screen_resources_reply_t* res_reply = xcb_randr_get_screen_resources_reply(
        conn, xcb_randr_get_screen_resources(conn, screen->root), nullptr);
    if (res_reply) {
        xcb_randr_output_t* outputs = xcb_randr_get_screen_resources_outputs(res_reply);
        int num_outputs = xcb_randr_get_screen_resources_outputs_length(res_reply);
        for (int i = 0; i < num_outputs; ++i) {
            xcb_randr_get_output_info_cookie_t out_cookie = xcb_randr_get_output_info(conn, outputs[i], XCB_CURRENT_TIME);
            xcb_randr_get_output_info_reply_t* out_reply = xcb_randr_get_output_info_reply(conn, out_cookie, nullptr);
            if (out_reply && out_reply->crtc != XCB_NONE && out_reply->connection == XCB_RANDR_CONNECTION_CONNECTED) {
                xcb_randr_get_crtc_info_cookie_t crtc_cookie = xcb_randr_get_crtc_info(conn, out_reply->crtc, XCB_CURRENT_TIME);
                xcb_randr_get_crtc_info_reply_t* crtc_reply = xcb_randr_get_crtc_info_reply(conn, crtc_cookie, nullptr);
                if (crtc_reply) {
                    Monitor mon;
                    mon.x = crtc_reply->x;
                    mon.y = crtc_reply->y;
                    mon.width = crtc_reply->width;
                    mon.height = crtc_reply->height;
                    mon.name = std::string((const char*)xcb_randr_get_output_info_name(out_reply),
                                           xcb_randr_get_output_info_name_length(out_reply));
                    monitors.push_back(mon);
                    free(crtc_reply);
                }
            }
            if (out_reply) free(out_reply);
        }
        free(res_reply);
    }

    if (monitors.empty()) {
        Monitor mon;
        mon.x = 0;
        mon.y = 0;
        mon.width = screen->width_in_pixels;
        mon.height = screen->height_in_pixels;
        mon.name = "Default";
        monitors.push_back(mon);
    }

    return monitors;
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
    std::string activeColor = "#313244";
    std::string focusedColor = "#45475a";
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
            name = "%{A:sh -c \"bspc desktop -f \'" + name + "\'\":}%{O10}" + name + "%{O10}%{A}";
            shownWorkspaces.push_back(name);
        }
    }
}

void Workspaces::checkWindows(const char &status,const std::string &workspace) {
    if (status == 'F') return;
    std::string focusedColor = "#45475a";

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


    // for (const auto& id : window_ids)
    //     std::cout << id << std::endl;

    std::vector<std::string> window_classes;
    
    for (auto it = window_ids.begin(); it != window_ids.end();) {
        std::string command = "xprop -id " + *it + " WM_CLASS 2>/dev/null | awk -F '\"' 'NF>1 {print $(NF-1)}'";
;
        std::string window_class = trim(::exec(command.c_str()));

        if (window_class == "com.mitchellh.ghostty")
            window_class = "ghostty";
        else if (window_class == "TelegramDesktop")
            window_class = "telegram";

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
        if (focused_window_id == window_ids[i])
            windowsInWorkspace.push_back("%{A3:bspc node " + window_ids[i] + " -c:}%{B" + focusedColor + "}%{O10}" + truncateString(window_classes[i]) + "%{O10}%{B-}%{A}");
        else windowsInWorkspace.push_back("%{A:bspc node -f " + window_ids[i] + ":}%{A3:bspc node " + window_ids[i] + " -c:}%{O10}" + truncateString(window_classes[i]) + "%{O10}%{B-}%{A}%{A}");

    }
}

std::string Workspaces::getWindows() {
    std::string result = "";
    for (const std::string& ws : windowsInWorkspace) {
        result += ws ;
    }
    // if (result != "") return " %{F#7f849c}|%{F-} " + result;
    if (result != "") return " " + result;
    return result;
}

std::string Workspaces::getWorkspaces() {
    std::string result = "";
    for (const std::string name : shownWorkspaces) {
				if (name == "LM" || name == "LT") {
					result += " %{F#7f849c}|%{F-} ";
					continue;
				}
        result += name;
    }
    return result;
}



// std::string Workspaces::getWorkspaces() {
//     std::string result = " ";
//     for (const std::string& ws : shownWorkspaces) {
//         result += ws;
//     }
//     result += " %{F#7f849c}|%{F-} ";
//     return result;
// }
