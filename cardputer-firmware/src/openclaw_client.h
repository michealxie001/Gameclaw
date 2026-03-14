#ifndef OPENCLAW_CLIENT_H
#define OPENCLAW_CLIENT_H

#include <WiFiClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

class OpenClawClient {
private:
    WebSocketsClient webSocket;
    String serverUrl;
    String bearerToken;
    bool connected = false;
    
    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    
public:
    OpenClawClient(String url, String token);
    bool connect();
    void disconnect();
    bool isConnected();
    void sendVoiceData(uint8_t* audioData, size_t dataSize);
    void sendCommand(String command, String context);
    void loop(); // 处理WebSocket事件
};

#endif