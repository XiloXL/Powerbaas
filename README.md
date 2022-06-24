# Powerbaas

## What is it?
Powerbaas Arduino library for smart meter P1 shield<br />
You can order your P1 Powerbaas shield on https://www.powerbaas.nl

## What can you do with it?
With this shield and library you are able to:
- Connect an ESP32 D1 R32 to your smart meter P1 connector
- Read current power usage
- Read power exported to grid during peak hours
- Read power exported to grid during off-peak hours
- Read power imported from grid during peak hours
- Read power imported from grid during off-peak hours
- Read gas used

## Added in version 1.0.1
- Webserver example

## Added in version 1.0.2
- MQTT example

## Added in version 1.1.0
- Support for KlikAanKlikUit 433Mhz socket switches
- Turn switch on or off via webserver button
- Let Powerbaas turn switch on/off based on power usage
- Let Powerbaas turn switch on/off based on time of day
- Fully pairable and configurable via webserver

## Added in version 1.2.0
- Support for Belgian Meters

## Added in version 1.3.0
- Current per phase
- Actual 

## Added in version 1.3.1
- Timestamp

## Added in version 1.3.2
- Home automation example

## Added in version 1.3.3
- Improved home automation example
- Only show current sensor data in JSON when sensor is connected on startup