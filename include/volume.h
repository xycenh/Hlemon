#ifndef VOLUME_H
#define VOLUME_H

#include <iostream>
#include <alsa/asoundlib.h>
#include <poll.h>
#include <unistd.h>
#include <string>

class Volume {
private:
    static int current_percent; // Static member to store the current volume
    static bool is_muted;       // New static member to store mute state
public:
    Volume();
    static void monitor_volume_changes(); // Public method to monitor volume changes
    static std::string getVolume();       // Public method to get the current volume as a string
};

#endif // VOLUME_H

