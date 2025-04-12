# Smart Warehouse System

## Overview
The **Smart Warehouse System** is an IoT-based solution designed to monitor and manage environmental conditions in agricultural storage areas. This system uses a variety of sensors to track temperature, humidity, light intensity, water levels, and product color for sorting. Additionally, it provides real-time control over pumps, lights, stepper motors, and other devices through the Blynk app, allowing users to remotely monitor and adjust warehouse conditions to ensure optimal storage for agricultural products.

## Features
- **Real-time sensor monitoring**: Tracks temperature, humidity, light intensity, and water levels.
- **Color sorting**: Utilizes a color sensor to sort agricultural products based on predefined color values.
- **Device control**: Remotely control water pumps, stepper motors, and relays to manage warehouse systems.
- **Blynk app integration**: Use Blynk for remote monitoring and control via a mobile app.
- **Audible alerts**: Includes buzzer control to signal critical events (e.g., abnormal sensor readings).
- **WiFi connectivity**: Connects to a WiFi network for cloud communication and remote access.

## Components Used
- **ESP8266**: The main microcontroller that handles WiFi connectivity and controls the sensors and actuators.
- **DHT11**: A temperature and humidity sensor.
- **BH1750**: A light intensity sensor.
- **PCF8574**: An I/O expander used to control relays and the stepper motor.
- **TCS34725**: A color sensor for sorting agricultural products.
- **Relay modules**: Used for controlling devices like pumps and lights.
- **Stepper motor**: Controls mechanical actions such as moving products or adjusting warehouse equipment.
- **Buzzer**: For audio alerts when certain conditions are met (e.g., water level too low).

## Usage
- Open the Blynk app to monitor and control the warehouse conditions.
- View real-time temperature, humidity, light levels, and water levels.
- Use the app to turn on/off pumps, stepper motors, and other devices remotely.
- Set up custom alarms with the buzzer for critical conditions (e.g., low water level or temperature out of range).
- Sort agricultural products based on their color using the integrated TCS34725 sensor.

## Sensors and Devices
- **DHT11**: Measures temperature and humidity inside the warehouse.
- **BH1750**: Monitors light intensity, which can affect product quality.
- **TCS34725**: A color sensor that helps sort agricultural products based on color.
- **Water sensor**: Measures the water level in a tank or system to ensure adequate moisture for stored products.
- **Stepper motor**: Controls mechanical processes like product sorting or moving.
- **Relays**: Control various devices such as pumps and lights.

## Code Breakdown
- **checkSensors()**: Initializes and checks if all the sensors are working properly.
- **checkConnection()**: Establishes WiFi and Blynk connection.
- **sendSensorData()**: Sends real-time data from sensors to the Blynk app.
- **stepperMotor()**: Controls the stepper motor for moving or sorting items based on color.
- **Manual control**: Allows users to control relays and other devices via Blynk app buttons.

## Conclusion
The **Smart Warehouse System** offers a convenient way to remotely monitor and manage the conditions of agricultural storage, ensuring that products are stored under optimal conditions. By integrating IoT technology and the Blynk platform, this system provides real-time data and remote control to improve efficiency in warehouse management.
