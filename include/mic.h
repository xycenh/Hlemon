#ifndef MIC_H
#define MIC_H

#include <iostream>
#include <alsa/asoundlib.h>
#include <poll.h>
#include <unistd.h>
#include <string>

class Mic {
private:
    static int current_percent; // Static member to store the current microphone volume
    static bool is_muted;       // Static member to store mute state
public:
    Mic();
    static void monitor_mic_changes(); // Public method to monitor mic changes
    static std::string getMicVolume(); // Public method to get the current mic volume as a string
};

#endif // MIC_H

