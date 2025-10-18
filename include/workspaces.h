#ifndef WORKSPACES_H
#define WORKSPACES_H

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <xcb/randr.h>
#include <xcb/xproto.h>

struct Monitor {
    int x, y, width, height;
    std::string name;
    std::string activeWorkspace;
    std::vector<std::string> visibleWindows;
    std::string activeWindow;
};

class Xsession {
public:
    Xsession();

private:
    xcb_connection_t* get_x11_conn();
    xcb_ewmh_connection_t get_ewmh_conn(xcb_connection_t*);
    xcb_screen_t* get_screen(xcb_connection_t*);
    void subscribe_event_for_root_window(xcb_connection_t*, xcb_screen_t*);
    std::vector<Monitor> get_monitors(xcb_connection_t* , xcb_screen_t* );
    void event_loop(xcb_connection_t* , xcb_ewmh_connection_t* , std::vector<Monitor> , xcb_screen_t* );
    void refresh(xcb_connection_t* , xcb_ewmh_connection_t* , const std::vector<Monitor>& , xcb_window_t );
    void update_display(xcb_connection_t* , xcb_ewmh_connection_t* , const std::vector<Monitor>&, xcb_window_t);
};

class Workspaces {
private:

  std::string workspaceEvent = "";
  void exec();
  std::vector<std::string> parseString(std::string);
  void checkWorkspaceStatus(const std::vector<std::string> &);

  void workspaces();
  void windows();

  void get_monitors();

  void checkWindows(const char &,const std::string &);

public:
  static std::vector<std::string> shownWorkspaces;
  static std::vector<std::string> windowsInWorkspace;
  Workspaces();
  static std::string getWorkspaces();
  static std::string getWindows();
};

#endif // !WORKSPACES_H
