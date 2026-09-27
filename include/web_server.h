#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include "config.h"

// Initializes WiFi SoftAP and web server
void webServerInit();

// Optional handle function if not using dedicated FreeRTOS task
void webServerHandle();

#endif // WEB_SERVER_H
