// ============================================================
// BAI THUC HANH 7 - GIAI DOAN 2: WEBSOCKET HANDLER HEADER
// Mo ta: Khai bao ham xu ly su kien WebSocket.
//        Tach logic WebSocket ra file rieng de main.cpp
//        gon gang, de doc hon.
// ============================================================

#ifndef WS_HANDLER_H
#define WS_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Dang ky WebSocket handler vao AsyncWebSocket
// Goi 1 lan trong setup()
void ws_setup(AsyncWebSocket& ws);

// Callback xu ly su kien WebSocket (connect/disconnect/data)
// Duoc dang ky qua ws.onEvent()
void ws_onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                AwsEventType type, void* arg, uint8_t* data, size_t len);

#endif
