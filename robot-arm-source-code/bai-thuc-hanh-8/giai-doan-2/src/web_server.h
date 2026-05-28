// =============================================================
//  web_server.h  –  WiFi connection & REST API / Web Server
// =============================================================
#pragma once
#include "cmd_parser.h"
#include "servo_ctrl.h"

// Initialize WiFi connection and setup Web Server routes
void webServerInit(CmdParser& parser, ServoCtrl& servos);

// Handle web client requests in loop()
void webServerHandle();
