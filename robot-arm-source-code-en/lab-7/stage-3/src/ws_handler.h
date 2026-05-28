// ============================================================
// LAB 7 - STAGE 3: WEBSOCKET HANDLER HEADER
// Nang cap so voi STAGE 2: xu ly JSON instead of plain-text
// ============================================================

#ifndef WS_HANDLER_H
#define WS_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void ws_setup(AsyncWebSocket& ws);
void ws_onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                AwsEventType type, void* arg, uint8_t* data, size_t len);

#endif
