#include "volume.h"
#include <signal.h>
#include <cerrno>
#include <cstring>
#include "lemonOutput.h"

// Initialize the static members
int Volume::current_percent = 0;
bool Volume::is_muted = false; // New static variable for mute state

Volume::Volume() {
    Volume::monitor_volume_changes();
}

void Volume::monitor_volume_changes() {
    snd_mixer_t *mixer = nullptr;
    snd_mixer_elem_t *elem = nullptr;
    int err;

    if ((err = snd_mixer_open(&mixer, 0)) < 0) {
        std::cerr << "Failed to open mixer: " << snd_strerror(err) << std::endl;
        return;
    }

    if ((err = snd_mixer_attach(mixer, "default")) < 0) {
        std::cerr << "Failed to attach mixer: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_selem_register(mixer, NULL, NULL)) < 0) {
        std::cerr << "Failed to register mixer: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_load(mixer)) < 0) {
        std::cerr << "Failed to load mixer elements: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");
    elem = snd_mixer_find_selem(mixer, sid);
    if (!elem) {
        std::cerr << "Failed to find Master element" << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Get initial volume and mute state
    long last_volume = 0, min, max;
    int last_mute_state = 0;
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &last_volume);
    snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &last_mute_state);

    Volume::current_percent = static_cast<int>((last_volume - min) * 100 / (max - min));
    Volume::is_muted = (last_mute_state == 0);

    struct pollfd fds[1];
    if (snd_mixer_poll_descriptors(mixer, fds, 1) < 0) {
        std::cerr << "Failed to get poll descriptors" << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    while (true) {
        int ret = poll(fds, 1, 500);
        if (ret < 0) {
            if (errno == EINTR) continue;
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }

        if (ret > 0 && (fds[0].revents & POLLIN)) {
            snd_mixer_handle_events(mixer);

            long current_volume = 0;
            int current_mute_state = 0;
            snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_volume);
            snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_mute_state);

            int last_percent = static_cast<int>((last_volume - min) * 100 / (max - min));
            Volume::current_percent = static_cast<int>((current_volume - min) * 100 / (max - min));
            Volume::is_muted = (current_mute_state == 0);

            if (current_volume != last_volume || current_mute_state != last_mute_state) {
                lemonOutput();
                last_volume = current_volume;
                last_mute_state = current_mute_state;
            }
        }
    }

    snd_mixer_close(mixer);
}

// Get current volume or mute state as a string
std::string Volume::getVolume() {
		std::string icon = "";
		std::string color = "#ffffff";


		if (Volume::is_muted) {
				color = "#ff8888"; 
		}

		if (Volume::current_percent == 0) {
				icon = " ";
				color = "#ff8888";
		} else if (Volume::current_percent< 15) {
				icon = " ";
		} else if (Volume::current_percent < 80) {
				icon = " ";
		} else if (Volume::current_percent < 100) {
				icon = " ";
		} else if (Volume::current_percent == 100) {
				icon = " ";
		}
		std::string format = " %{F" + color + "}" + icon + " " + std::to_string(Volume::current_percent) + "%" + " %{F-}";
    /*return Volume::is_muted ? icon : icon + " " + std::to_string(Volume::current_percent) + "%";*/
    return format; 
}

