// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 3: WEBSOCKET HANDLER HEADER
// Nang cap so voi giai doan 2: xu ly JSON thay vi plain-text
// ============================================================

#ifndef WS_HANDLER_H
#define WS_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void ws_setup(AsyncWebSocket& ws);
void ws_onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                AwsEventType type, void* arg, uint8_t* data, size_t len);

#endif
