#ifndef DELAY_H
#define DELAY_H

#include <chrono>
#include <thread>

// Macro to delay
#define DELAY_S(s)    (std::this_thread::sleep_for(std::chrono::seconds(s)))
#define DELAY_MS(ms)  (std::this_thread::sleep_for(std::chrono::milliseconds(ms)))

#endif // DELAY_H
