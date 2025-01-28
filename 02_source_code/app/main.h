#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include "camera_control.h"
#include "button_handler.h"
#include "gui_display.h"
#include "log.h"

void signalHandler(int signal);

#endif /*_MAIN_H_*/