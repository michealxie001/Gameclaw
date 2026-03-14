# Star Office UI 集成指南

## 项目概述

将 [Star Office UI](https://github.com/ringhyacinth/Star-Office-UI) 与 ZeroClaw 集成，创建一个可视化、交互式的 AI 助手办公室界面。

## 集成架构

```
ZeroClaw (AI 助手核心) ↔ HTTP API ↔ Star Office UI (可视化前端)
```

## 安装步骤

### 1. 克隆 Star Office UI 项目

```bash
git clone https://github.com/ringhyacinth/Star-Office-UI.git
cd Star-Office-UI
```

### 2. 安装 Python 依赖

```bash
# 确保使用 Python 3.10+
python3 -m pip install -r backend/requirements.txt
```

### 3. 配置 ZeroClaw 集成

创建自定义的 `set_state.py` 脚本，用于与 ZeroClaw 通信：

```python
#!/usr/bin/env python3
"""Custom set_state script for ZeroClaw integration"""

import sys
import json
import requests
import os
from datetime import datetime

# ZeroClaw Gateway 配置
ZEROCLAW_GATEWAY_URL = "http://localhost:42617"
ZEROCLAW_AUTH_TOKEN = os.getenv("ZEROCLAW_AUTH_TOKEN", "")

def send_to_zeroclaw(state, detail):
    """发送状态到 ZeroClaw"""
    try:
        headers = {
            "Authorization": f"Bearer {ZEROCLAW_AUTH_TOKEN}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "message": f"AI 助手状态更新: {state} - {detail}",
            "context": {
                "state": state,
                "detail": detail,
                "timestamp": datetime.now().isoformat()
            }
        }
        
        response = requests.post(
            f"{ZEROCLAW_GATEWAY_URL}/webhook",
            headers=headers,
            json=payload,
            timeout=10
        )
        
        if response.status_code == 200:
            print(f"✅ 状态已发送到 ZeroClaw: {state} - {detail}")
            return True
        else:
            print(f"❌ 发送失败: {response.status_code} - {response.text}")
            return False
            
    except Exception as e:
        print(f"❌ 连接错误: {e}")
        return False

def main():
    if len(sys.argv) < 3:
        print("用法: python3 set_state.py <state> <detail>")
        print("状态选项: idle, writing, researching, executing, syncing, error")
        sys.exit(1)
    
    state = sys.argv[1]
    detail = sys.argv[2]
    
    # 验证状态
    valid_states = {"idle", "writing", "researching", "executing", "syncing", "error"}
    if state not in valid_states:
        print(f"无效状态: {state}")
        print(f"有效状态: {', '.join(valid_states)}")
        sys.exit(1)
    
    # 发送到 ZeroClaw
    success = send_to_zeroclaw(state, detail)
    
    # 同时保存到本地状态文件（兼容原版 Star Office UI）
    state_data = {
        "state": state,
        "detail": detail,
        "progress": 0,
        "updated_at": datetime.now().isoformat()
    }
    
    with open("state.json", "w", encoding="utf-8") as f:
        json.dump(state_data, f, ensure_ascii=False, indent=2)
    
    if success:
        print("✅ 状态更新完成")
    else:
        print("⚠️  ZeroClaw 连接失败，但本地状态已保存")

if __name__ == "__main__":
    main()
```

### 4. 配置环境变量

创建 `.env` 文件：

```bash
# ZeroClaw 配置
ZEROCLAW_GATEWAY_URL=http://localhost:42617
ZEROCLAW_AUTH_TOKEN=your_bearer_token_here

# Star Office UI 配置
FLASK_SECRET_KEY=your_strong_secret_key_here
ASSET_DRAWER_PASS=your_asset_password_here
```

### 5. 启动服务

```bash
# 启动 ZeroClaw 守护进程
zeroclaw daemon

# 启动 Star Office UI 后端
cd Star-Office-UI/backend
python3 app.py
```

### 6. 访问界面

打开浏览器访问: `http://<your-device-ip>:19000`

## 移动端优化

### 触摸屏支持
Star Office UI 已经内置了移动端优化，支持：
- ✅ 触摸操作
- ✅ 响应式布局
- ✅ 移动端手势

### 语音控制集成

在 Star Office UI 的控制面板中添加语音按钮：

```html
<!-- 在 frontend/index.html 中添加 -->
<button id="voice-control-btn" onclick="startVoiceControl()">
    🎤 语音控制
</button>

<script>
function startVoiceControl() {
    // 调用 ZeroClaw 语音识别
    fetch('/api/voice/start', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'}
    });
}
</script>
```

## 故障排除

### 常见问题

#### 1. ZeroClaw 连接失败
- 检查 ZeroClaw 是否正在运行：`zeroclaw status`
- 验证认证令牌是否正确
- 确保防火墙允许 42617 端口

#### 2. Star Office UI 无法启动
- 确保 Python 3.10+ 已安装
- 检查依赖是否完整安装
- 验证 `state.json` 文件是否存在

#### 3. 移动端显示异常
- 清除浏览器缓存
- 确保使用最新版本的浏览器
- 检查网络连接稳定性

## 高级功能

### 多 Agent 支持
Star Office UI 原生支持多 Agent 协作，可以同时显示多个 AI 助手的状态。

### 自定义角色
通过修改 `assets/` 目录中的像素素材，可以创建自定义的 AI 助手形象。

### 场景切换
利用 Star Office UI 的装修功能，可以创建不同的工作场景（办公室、篝火旁、咖啡厅等）。

## 性能优化

### 内存使用
- 在低内存设备上，禁用 AI 生图功能
- 减少同时在线的访客 Agent 数量
- 使用轻量级浏览器访问

### 电池续航
- 降低屏幕亮度
- 启用自动休眠
- 关闭不必要的后台服务

## 未来扩展

### 语音交互增强
- 集成实时语音转文字
- 添加语音命令识别
- 实现语音回复播放

### 硬件集成
- 连接物理按钮控制
- 集成 LED 状态指示灯
- 添加环境传感器

这个集成方案将 ZeroClaw 的强大 AI 能力与 Star Office UI 的可视化界面完美结合，创造出一个既实用又有趣的 AI 助手体验。