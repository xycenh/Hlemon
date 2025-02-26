#include "clock.h"

void displayTime() {
	while(true) {
		lemonOutput();
	    auto now = std::chrono::system_clock::now();
	    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	    std::tm *local_time = std::localtime(&now_c);
	    int current_second = local_time->tm_sec;
	    int time_until_update = 60 - current_second;
	    std::this_thread::sleep_for(std::chrono::seconds(time_until_update));
	}
}

std::string getClock() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::ostringstream oss;
	oss << std::put_time(std::localtime(&now_c), "%H:%M");
	return " " + oss.str() + " ";
}

