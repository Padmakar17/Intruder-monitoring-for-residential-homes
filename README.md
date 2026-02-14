# Intruder-monitoring-for-residential-homes
ESP32–STM32 based smart surveillance system for residential security integrating live video streaming, multi-sensor intrusion detection, RFID access control, and real-time MQTT cloud alerts. Features UART inter-controller communication, OLED status display, and multi-mode alerting for secure, continuous monitoring.

An embedded IoT-based real-time surveillance and environmental monitoring system designed for residential security. This project integrates live video streaming, RFID-based access control, multi-sensor intrusion detection, and MQTT cloud alerts using ESP32, ESP32-CAM, and STM32 microcontrollers. The system provides continuous monitoring, rapid threat detection, and multi-mode alerting for smart home safety.

The system architecture consists of IR motion, reed switch (door), and shock sensors connected to an ESP32 for intrusion detection. An MFRC522 RFID module enables secure authentication, while a 0.96” I2C OLED displays system status in real time. Alerts are triggered using a buzzer and LEDs locally, and telemetry is sent to the cloud via MQTT for remote monitoring. The ESP32-CAM hosts an HTTP live video streaming server and communicates detected events to the STM32 via UART. The STM32 processes object detection signals and handles extended control logic. Cloud connectivity allows real-time event logging and dashboard visualization.

Key features include live video streaming over Wi-Fi, RFID-based access control with temporary system sleep mode, multi-sensor intrusion detection, MQTT-based cloud alerts, UART inter-controller communication, FreeRTOS task-based execution, and real-time OLED status display. The system ensures secure monitoring by combining hardware-based sensing, embedded firmware control, and cloud-based telemetry.

Hardware components used in this project include ESP32 Dev Module, ESP32-CAM, STM32 microcontroller, IR motion sensor, reed switch door sensor, shock/vibration sensor, MFRC522 RFID module, 0.96” OLED display (I2C), buzzer, LEDs, and Wi-Fi network. Technologies used include Embedded C, Arduino framework, ESP-IDF, FreeRTOS, MQTT protocol, HTTP streaming server, UART communication, SPI, and I2C interfaces.

To run the project, upload the ESP32 intrusion firmware using Arduino IDE after configuring Wi-Fi and MQTT credentials. Build and flash the ESP32-CAM firmware using ESP-IDF, then access the live stream using the device IP address. The STM32 firmware handles UART communication and event processing.

This project demonstrates strong concepts in embedded systems design, real-time firmware development, communication protocols, IoT cloud integration, and multi-controller system architecture.
