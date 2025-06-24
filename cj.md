这是一个典型的“环境监测系统”项目，涉及嵌入式开发（Arduino）和Web端可视化（Python/Flask + HTML/JavaScript）。你的 `.ino` 文件已经很好地读取传感器数据并通过串口输出 JSON。接下来，我们来实现：

---

## 🐍 Python 后端（Flask 接收串口数据并提供 API）

**文件名：`server.py`**

```python
from flask import Flask, jsonify, render_template
import serial
import threading
import json

app = Flask(__name__)
data_buffer = {"temp": "", "humid": "", "press": "", "time": ""}

def read_serial():
    global data_buffer
    try:
        ser = serial.Serial('COM3', 9600)  # 替换为你的 Arduino 端口
        while True:
            line = ser.readline().decode('utf-8').strip()
            try:
                data = json.loads(line)
                data_buffer = data
            except json.JSONDecodeError:
                continue
    except serial.SerialException as e:
        print("串口错误:", e)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def get_data():
    return jsonify(data_buffer)

if __name__ == '__main__':
    threading.Thread(target=read_serial, daemon=True).start()
    app.run(debug=True)
```

---

## 🌐 HTML + JavaScript 前端（动态显示传感器数据）

**文件结构：**

```
project_folder/
│
├── server.py
├── templates/
│   └── index.html
```

**文件名：`templates/index.html`**

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Мониторинг Окружающей Среды</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding-top: 50px; }
    .data-box { margin: 20px auto; font-size: 24px; }
  </style>
</head>
<body>
  <h1>Данные с Arduino</h1>
  <div class="data-box">Температура: <span id="temp">--</span> °C</div>
  <div class="data-box">Влажность: <span id="humid">--</span> %</div>
  <div class="data-box">Давление: <span id="press">--</span> мм рт.ст</div>
  <div class="data-box">Время: <span id="time">--</span></div>

  <script>
    function updateData() {
      fetch('/data')
        .then(res => res.json())
        .then(data => {
          document.getElementById('temp').textContent = data.temp;
          document.getElementById('humid').textContent = data.humid;
          document.getElementById('press').textContent = data.press;
          document.getElementById('time').textContent = data.time;
        });
    }
    setInterval(updateData, 2000); // 每2秒刷新一次
    updateData(); // 初始加载
  </script>
</body>
</html>
```

---

## ✅ 使用说明

1. 连接好 Arduino 和 PC，确认串口号（如 COM3 或 /dev/ttyUSB0）。
2. 上传你的 `.ino` 文件到 Arduino。
3. 安装 Flask 和 pyserial：

```bash
pip install flask pyserial
```

4. 运行 `server.py`：

```bash
python server.py
```

5. 打开浏览器访问 [http://localhost:5000](http://localhost:5000) 查看实时数据。

---

## 🔄 选做拓展（可加分）

* 使用 chart.js 实现图表曲线；
* 用 LocalStorage 或 SQLite 存历史数据；
* 添加传感器报警功能（如超温预警）；
* 加入蓝牙支持：让 Python 接收 HC-05 数据（类似串口）。

如果你还需要将数据存储在 EEPROM（25LC640）中、或显示在 SSD1306 OLED 上，也可以告诉我，我帮你扩展代码。
