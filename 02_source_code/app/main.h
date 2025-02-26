#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include <thread>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "camera_control.h"
#include "button_handler.h"
#include "gui_display.h"
#include "log.h"
#include "delay.h"

void signalHandler(int signal);
void buttonThread();
void cameraThread();

#endif /*_MAIN_H_*/