#include <M5Cardputer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";           // 请替换为你的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD";   // 请替换为你的WiFi密码

// OpenClaw服务器配置（指向蘑蘑）
const char* openclaw_url = "http://47.88.77.96:42617";
const char* bearer_token = "9673f533dd7cebce3d12d3090756c78e"; // 蘑蘑的认证令牌

// 状态变量
String current_state = "idle";
String current_detail = "等待指令...";
bool wifi_connected = false;

void setup() {
  // 初始化M5Cardputer
  M5Cardputer.begin();
  M5Cardputer.Speaker.setVolume(80); // 设置扬声器音量
  
  // 初始化串口
  Serial.begin(115200);
  
  // 连接WiFi
  connectWiFi();
  
  // 显示启动画面
  displayStartup();
}

void loop() {
  // 处理按键输入
  handleKeyPress();
  
  // 处理语音输入（如果按下特定键）
  // handleVoiceInput();
  
  // 更新显示
  updateDisplay();
  
  delay(50);
}

void connectWiFi() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Connecting to WiFi...");
  M5Cardputer.Display.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    M5Cardputer.Display.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    M5Cardputer.Display.println("\nWiFi Connected!");
    M5Cardputer.Display.println(WiFi.localIP());
  } else {
    M5Cardputer.Display.println("\nWiFi Failed!");
  }
  
  delay(2000);
}

void displayStartup() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("🎮 Gameclaw");
  M5Cardputer.Display.println("OpenClaw Terminal");
  M5Cardputer.Display.println("");
  M5Cardputer.Display.println("A: Send Command");
  M5Cardputer.Display.println("B: Voice Input");
  M5Cardputer.Display.println("Start: Menu");
}

void handleKeyPress() {
  if (M5Cardputer.Keyboard.isKeyPressed()) {
    KeyboardKey key = M5Cardputer.Keyboard.read();
    
    switch(key) {
      case KB_KEY_A:
        sendCommandToOpenClaw("manual_input", "用户按下A键");
        break;
      case KB_KEY_B:
        // 语音输入功能（后续实现）
        current_state = "listening";
        current_detail = "请说话...";
        break;
      case KB_KEY_ESC: // Start键
        showMenu();
        break;
      default:
        // 处理其他按键输入
        break;
    }
  }
}

void sendCommandToOpenClaw(String state, String detail) {
  if (!wifi_connected) {
    current_state = "error";
    current_detail = "未连接WiFi";
    return;
  }
  
  // 这里应该实现WebSocket或HTTP POST到OpenClaw
  // 暂时用简单的状态更新演示
  current_state = state;
  current_detail = detail;
  
  // TODO: 实际的OpenClaw API调用
  Serial.println("Sending to OpenClaw: " + state + " - " + detail);
}

void showMenu() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("📋 Menu");
  M5Cardputer.Display.println("1. Status");
  M5Cardputer.Display.println("2. Settings");
  M5Cardputer.Display.println("3. Exit");
}

void updateDisplay() {
  // 简化的状态显示
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println("🎮 Gameclaw");
    M5Cardputer.Display.println("State: " + current_state);
    M5Cardputer.Display.println("Detail: " + current_detail);
    
    if (wifi_connected) {
      M5Cardputer.Display.println("WiFi: ✓");
    } else {
      M5Cardputer.Display.println("WiFi: ✗");
    }
    
    lastUpdate = millis();
  }
  
  M5Cardputer.update();
}