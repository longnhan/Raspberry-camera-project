#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include <thread>
#include "camera_control.h"
#include "button_handler.h"
#include "gui_display.h"
#include "log.h"

void signalHandler(int signal);
void buttonThread();

#endif /*_MAIN_H_*/