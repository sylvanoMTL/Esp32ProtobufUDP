#include <WiFi.h>
#include <WiFiUdp.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "data.pb.h"


// AP settings
const char* ssid = "ESP32-AP";
const char* password = "12345678";

WiFiUDP udp;
const int udpPort = 3333;
uint8_t incomingPacket[128];

void setup() {
    Serial.begin(115200);

    // Start AP
    WiFi.softAP(ssid, password);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    udp.begin(udpPort);
}

void loop() {
    int packetSize = udp.parsePacket();
    if(packetSize) {
        int len = udp.read(incomingPacket, sizeof(incomingPacket));
        if(len > 0 && len < sizeof(incomingPacket)) {
            SensorData message = SensorData_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(incomingPacket, len);

            if(pb_decode(&stream, SensorData_fields, &message)) {
                // Serial.printf("Received: sine=%.3f, analog=%d\n",
                //               message.sine_value, message.analog_value);
                Serial.print(message.sine_value);Serial.print("\t");Serial.print(message.analog_value);Serial.println();
            } else {
                Serial.println("Decoding failed!");
            }
        }
    }
}