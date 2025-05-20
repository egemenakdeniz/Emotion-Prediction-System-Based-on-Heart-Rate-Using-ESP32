# Emotion Prediction System Based on Heart Rate Using ESP32

This project aims to predict the emotional state of a user based on their heart rate (BPM), using the MAX30100 pulse sensor and an ESP32 microcontroller. The system calculates the average BPM and sends it to a Flask-based server that returns the predicted emotion using a machine learning model. The result is displayed on an OLED screen.

## 🧠 Project Description

- **Sensor Input:** Heart rate is measured using the MAX30100 sensor.
- **Microcontroller:** ESP32 calculates the average of 20 valid BPM values.
- **Communication:** Data is sent via Wi-Fi to a Flask API hosted on Render.
- **Machine Learning:** A pre-trained ML model predicts the emotion based on the BPM value.
- **Output:** The predicted emotion and BPM value are shown on a 128x64 OLED display.

## 🧰 Technologies and Tools

- ESP32 Development Board  
- MAX30100 Pulse and Oximeter Sensor  
- SSD1306 OLED Display  
- Flask (Python backend)  
- TensorFlow / Scikit-learn (for model training)  
- Arduino IDE (for ESP32 coding)  
- Render (for hosting the Flask app)

## 📦 Installation

### ESP32 Setup
1. Connect MAX30100 and OLED to ESP32 via I2C (SDA: GPIO21, SCL: GPIO22).
2. Upload the code using Arduino IDE.
3. Configure your Wi-Fi credentials and server URL in the ESP32 sketch.

### Flask Server Setup
1. Clone the repository and run `app.py`.
2. Make sure `heart_rate_emotion_model1.pkl` and `label_encoder1.pkl` are in the same directory.
3. Use Render or similar platform to deploy the Flask app for cloud access.

## 📡 System Workflow

```mermaid

graph TD
    A[MAX30100 Sensor] --> B[ESP32 Calculates Avg BPM]
    B --> C[Send Data via HTTP POST]
    C --> D[Flask API]
    D --> E[ML Model Predicts Emotion]
    E --> F[Send Response to ESP32]
    F --> G[Display on OLED]

💻 Example Output
makefile
Kopyala
Düzenle
Emotion: Happy
BPM: 78.4

📁 File Structure
bash
Kopyala
Düzenle
/esp32-code       -> ESP32 Arduino source files
/flask-server     -> Python Flask app and ML model
README.md         -> Project description

👤 Author
Egemen Akdeniz
