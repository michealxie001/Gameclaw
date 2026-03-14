#include "openclaw_client.h"
#include <M5Cardputer.h>
#include <ArduinoJson.h>

OpenClawClient::OpenClawClient(String url, String token) {
    serverUrl = url;
    bearerToken = token;
}

void OpenClawClient::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[WebSocket] Disconnected!");
            connected = false;
            break;
        case WStype_CONNECTED:
            Serial.println("[WebSocket] Connected to OpenClaw!");
            connected = true;
            // 发送认证信息
            DynamicJsonDocument authDoc(256);
            authDoc["type"] = "auth";
            authDoc["token"] = bearerToken;
            String authMsg;
            serializeJson(authDoc, authMsg);
            webSocket.sendTXT(authMsg);
            break;
        case WStype_TEXT:
            Serial.printf("[WebSocket] Received: %s\n", payload);
            // 处理来自OpenClaw的指令和回复
            handleOpenClawResponse((char*)payload);
            break;
        case WStype_BIN:
            Serial.println("[WebSocket] Received binary data");
            // 处理音频数据（TTS回复）
            handleAudioData(payload, length);
            break;
    }
}

bool OpenClawClient::connect() {
    // 连接到OpenClaw WebSocket端点
    webSocket.begin(serverUrl, 42617, "/websocket");
    webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(type, payload, length);
    });
    return true;
}

void OpenClawClient::disconnect() {
    webSocket.disconnect();
    connected = false;
}

bool OpenClawClient::isConnected() {
    return connected;
}

void OpenClawClient::sendVoiceData(uint8_t* audioData, size_t dataSize) {
    if (connected) {
        // 发送语音数据到OpenClaw
        DynamicJsonDocument voiceDoc(512);
        voiceDoc["type"] = "voice_input";
        voiceDoc["format"] = "opus";
        voiceDoc["sample_rate"] = 16000;
        // 实际应用中需要将audioData编码为base64或直接发送二进制
        String voiceMsg;
        serializeJson(voiceDoc, voiceMsg);
        webSocket.sendTXT(voiceMsg);
        // webSocket.sendBIN(audioData, dataSize); // 发送二进制音频数据
    }
}

void OpenClawClient::sendCommand(String command, String context) {
    if (connected) {
        DynamicJsonDocument cmdDoc(512);
        cmdDoc["type"] = "command";
        cmdDoc["command"] = command;
        cmdDoc["context"] = context;
        cmdDoc["timestamp"] = millis();
        
        String cmdMsg;
        serializeJson(cmdDoc, cmdMsg);
        webSocket.sendTXT(cmdMsg);
    }
}

void OpenClawClient::loop() {
    webSocket.loop();
}

void handleOpenClawResponse(char* response) {
    // 解析OpenClaw的回复并更新UI
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
        String responseType = doc["type"];
        if (responseType == "status_update") {
            // 更新状态显示
            String newState = doc["state"];
            String newDetail = doc["detail"];
            // 这里需要更新全局状态变量
            // current_state = newState;
            // current_detail = newDetail;
        } else if (responseType == "text_response") {
            // 显示文本回复
            String textReply = doc["text"];
            // 更新显示或触发TTS
        }
    }
}

void handleAudioData(uint8_t* audioData, size_t length) {
    // 播放TTS音频回复
    // M5Cardputer.Speaker.playRaw(audioData, length);
}