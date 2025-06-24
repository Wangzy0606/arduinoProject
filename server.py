import serial
import json
from datetime import datetime
import time
from flask import Flask, render_template
import threading

app = Flask(__name__)

# 全局变量存储最新数据
latest_data = {
    'temp': 0,
    'humid': 0,
    'press': 0,
    'time': '00:00',
    'received_at': '等待数据...'
}

class ArduinoDataReceiver:
    def __init__(self):
        self.ser = None
        self.port = 'COM6'  # 修改为你的实际端口
        self.baudrate = 9600
        self.connect_to_arduino()
        
    def connect_to_arduino(self):
        """稳定的连接方法"""
        try:
            # 先关闭已有连接
            if self.ser and self.ser.is_open:
                self.ser.close()
                
            # 添加延迟确保端口释放
            time.sleep(1)
            
            # 创建新连接
            self.ser = serial.Serial(
                self.port,
                self.baudrate,
                timeout=2,
                exclusive=True  # 防止其他程序占用
            )
            print(f"✅ 成功连接到 {self.port}")
            time.sleep(2)  # 等待Arduino初始化
            return True
        except serial.SerialException as e:
            print(f"❌ 连接失败: {e}")
            return False
    
    def read_data(self):
        if not self.ser or not self.ser.is_open:
            if not self.connect_to_arduino():
                return None
        
        try:
            line = self.ser.readline().decode('utf-8').strip()
            return self.parse_data(line) if line else None
        except Exception as e:
            print(f"⚠️ 读取错误: {e}")
            self.ser.close()
            return None
    
    def parse_data(self, data_str):
        try:
            data = json.loads(data_str)
            return {
                'temp': float(data['temp']),
                'humid': float(data['humid']),
                'press': float(data['press']),
                'time': data['time'],
                'received_at': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
        except json.JSONDecodeError:
            print(f"⚠️ JSON解析失败: {data_str}")
            return None
    
    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("🔌 串口连接已关闭")

def background_task():
    """独立的后台数据采集线程"""
    receiver = ArduinoDataReceiver()
    while True:
        data = receiver.read_data()
        if data:
            global latest_data
            latest_data = data
        time.sleep(0.5)

@app.route('/')
def index():
    """网页路由"""
    return render_template('index.html', **latest_data)

if __name__ == "__main__":
    # 启动后台线程
    threading.Thread(target=background_task, daemon=True).start()
    
    # 关键修改：禁用调试重载
    app.run(
        host='0.0.0.0',
        port=5000,
        debug=True,      # 保留调试输出
        use_reloader=False  # 禁用自动重载
    )
