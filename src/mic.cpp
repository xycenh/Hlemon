#include "mic.h"
#include "functions.h"
#include "lemonOutput.h"
#include <cerrno>
#include <cstring>

// Initialize static members
int Mic::current_percent = 0;
bool Mic::is_muted = false;

Mic::Mic() {
    Mic::monitor_mic_changes();
}

// Function to monitor microphone changes
void Mic::monitor_mic_changes() {
    snd_mixer_t *mixer = nullptr;
    snd_mixer_elem_t *elem = nullptr;
    int err;

    // Initialize the mixer
    if ((err = snd_mixer_open(&mixer, 0)) < 0) {
        std::cerr << "Failed to open mixer: " << snd_strerror(err) << std::endl;
        return;
    }

    // Attach the mixer to the default sound card
    if ((err = snd_mixer_attach(mixer, "default")) < 0) {
        std::cerr << "Failed to attach mixer: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Register mixer
    if ((err = snd_mixer_selem_register(mixer, NULL, NULL)) < 0) {
        std::cerr << "Failed to register mixer: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Load mixer elements
    if ((err = snd_mixer_load(mixer)) < 0) {
        std::cerr << "Failed to load mixer elements: " << snd_strerror(err) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Find the Capture mixer element
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Capture");
    elem = snd_mixer_find_selem(mixer, sid);
    if (!elem) {
        std::cerr << "Failed to find Capture element" << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Get initial mic volume
    long last_volume = 0;
    long min, max;
    snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
    snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &last_volume);
    int vol_percent = static_cast<int>((last_volume - min) * 100 / (max - min));
    Mic::current_percent = vol_percent;

    // Get initial mute state
    int last_mute_state = 0;
    snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &last_mute_state);
    Mic::is_muted = (last_mute_state == 0);

	// Check microphone volume
	long current_volume = 0;
	snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_volume);
	vol_percent = static_cast<int>((current_volume - min) * 100 / (max - min));

	// Check mute state
	int current_mute_state = 0;
	snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_mute_state);
	bool muted = (current_mute_state == 0);


	Mic::current_percent = vol_percent;
	Mic::is_muted = muted;
	updateLemonbar(lemonOutput());
	last_volume = current_volume;

    // Set up polling for mixer events
    struct pollfd fds[1];
    int count = snd_mixer_poll_descriptors(mixer, fds, 1);
    if (count < 0) {
        std::cerr << "Failed to get poll descriptors: " << snd_strerror(count) << std::endl;
        snd_mixer_close(mixer);
        return;
    }

    // Main event loop
    while (true) {
        int ret = poll(fds, 1, 500); // 500ms timeout
        if (ret < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal
            }
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }

        // Handle mixer events
        if (ret > 0 && (fds[0].revents & POLLIN)) {
            snd_mixer_handle_events(mixer);

            // Check microphone volume
            long current_volume = 0;
            snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_volume);
            int vol_percent = static_cast<int>((current_volume - min) * 100 / (max - min));

            // Check mute state
            int current_mute_state = 0;
            snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &current_mute_state);
            bool muted = (current_mute_state == 0);

            // Update and print changes if needed
            if (current_volume != last_volume || muted != Mic::is_muted) {
                Mic::current_percent = vol_percent;
                Mic::is_muted = muted;
                updateLemonbar(lemonOutput());
                last_volume = current_volume;
            }
        }
    }

    // Clean up
    snd_mixer_close(mixer);
}

// Get current microphone volume as a string
std::string Mic::getMicVolume() {
		std::string icon = iconColor("MIC");
		std::string format = "";

		if (Mic::is_muted) {
				format = " " + icon + " off ";
		} else {
				format = " " + icon + " " + std::to_string(Mic::current_percent) + "% ";
		}
		/*std::string format = " %{F" + color + "}" + icon + " " + std::to_string(Mic::current_percent) + "%" + " %{F-}";*/
    return format;
}
