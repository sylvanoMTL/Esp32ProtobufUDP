#include <WiFi.h>
#include <WiFiUdp.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "data.pb.h"

// WiFi settings (connect to server ESP32 AP)
const char* ssid = "ESP32-AP";         // AP SSID
const char* password = "12345678";     // AP Password
IPAddress serverIP(192, 168, 4, 1);    // AP IP
const int udpPort = 3333;

WiFiUDP udp;
float phase = 0.0;  // sine wave phase

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to AP
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("Client IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Fill SensorData message
    SensorData message = SensorData_init_zero;
    message.sine_value = sin(phase);
    phase += 0.1;
    if (phase > 2 * PI) phase -= 2 * PI;

    message.analog_value = analogRead(34); // replace with your analog pin

    // Encode using nanopb
    uint8_t buffer[64];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    if (pb_encode(&stream, SensorData_fields, &message)) {
        // Send via UDP
        udp.beginPacket(serverIP, udpPort);
        udp.write(buffer, stream.bytes_written);
        udp.endPacket();

        Serial.printf("Sent: sine=%.3f, analog=%d\n", message.sine_value, message.analog_value);
    } else {
        Serial.println("Encoding failed!");
    }

    delay(10); // 10 Hz
}
