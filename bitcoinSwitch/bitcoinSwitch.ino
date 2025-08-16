#include <ArduinoJson.h>
#include <WebSocketsClient.h>

String config_ssid;
String config_password;
String config_device_string;
String config_threshold_inkey;
int config_threshold_amount;
int config_threshold_pin;
int config_threshold_time;

String apiUrl = "/api/v1/ws/";

// length of switch device id
// 7dhdyJ9bbZNWNVPiFSdmb5
int uidLength = 22;

bool paid;
long thresholdSum = 0;
String payloadStr = "";

WebSocketsClient webSocket;

void setup() {
    Serial.begin(115200);
    #ifdef TDISPLAY
    setupTFT();
    #endif
    setupConfig();
    setupWifi();

    pinMode(2, OUTPUT); // To blink on board LED

    if (config_device_string == "") {
        Serial.println("No device string configured!");
        printTFT("No device string!", 21, 95);
        return;
    }

    if (!config_device_string.startsWith("wss://")) {
        Serial.println("Device string does not start with wss://");
        printTFT("no wss://!", 21, 95);
        return;
    }

    String cleaned_device_string = config_device_string.substring(6); // Remove wss://
    String host = cleaned_device_string.substring(0, cleaned_device_string.indexOf('/'));
    String apiPath = cleaned_device_string.substring(cleaned_device_string.indexOf('/'));
    Serial.println("Websocket host: " + host);
    Serial.println("Websocket API Path: " + apiPath);

    if (config_threshold_amount != 0) { // Use in threshold mode
        Serial.println("Using THRESHOLD mode");
        Serial.println("Connecting to websocket: " + host + apiUrl + config_threshold_inkey);
        webSocket.beginSSL(host, 443, apiUrl + config_threshold_inkey);
    } else { // Use in normal mode
        Serial.println("Using NORMAL mode");
        Serial.println("Connecting to websocket: " + host + apiPath);
        webSocket.beginSSL(host, 443, apiPath);
    }
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000);
}

void loop() {
    loopWifi();
    digitalWrite(2, LOW);
    payloadStr = "";
    delay(1000);
    while (paid == false) { // loop and wait for payment
        webSocket.loop();
        if (paid) {
            if (config_threshold_amount != 0) {
                // If in threshold mode we check the "balance" pushed by the
                // websocket and use the pin/time preset
                // executeThreshold();
            } else {
                // If in normal mode we use the pin/time pushed by the websocket
                // pin-time format
                int pin = payloadStr.substring(0, payloadStr.indexOf('-')).toInt();
                int time = payloadStr.substring(payloadStr.indexOf('-') + 1).substring(0, payloadStr.indexOf('-')).toInt();
                Serial.println("Executing parsed payload:");
                Serial.println("Pin: " + String(pin));
                Serial.println("Time: " + String(time));
                pinMode(pin, OUTPUT);
                digitalWrite(pin, HIGH);
                flashTFT();
                delay(time);
                digitalWrite(pin, LOW);
                clearTFT();
            }
        }
    }
    paid = false;
}

// void executeThreshold() {
//     StaticJsonDocument<1900> doc;
//     DeserializationError error = deserializeJson(doc, payloadStr);
//     if (error) {
//         Serial.print("deserializeJson() failed: ");
//         Serial.println(error.c_str());
//         return;
//     }
//     thresholdSum = thresholdSum + doc["payment"]["amount"].toInt();
//     Serial.println("thresholdSum: " + String(thresholdSum));
//     if (thresholdSum >= (config_threshold_amount * 1000)) {
//         pinMode(config_threshold_pin, OUTPUT);
//         digitalWrite(config_threshold_pin, HIGH);
//         delay(config_threshold_time);
//         digitalWrite(config_threshold_pin, LOW);
//         thresholdSum = 0;
//     }
// }

//////////////////WEBSOCKET///////////////////
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WebSocket] Disconnected!\n");
            printTFT("No Websocket!", 21, 95);
            break;
        case WStype_CONNECTED:
            Serial.printf("[WebSocket] Connected to url: %s\n", payload);
            // send message to server when Connected
            webSocket.sendTXT("Connected");
            printTFT("WS connected!", 21, 95);
            break;
        case WStype_TEXT:
            payloadStr = (char *)payload;
            payloadStr.replace(String("'"), String('"'));
            Serial.println("Received data from socket: " + payloadStr);
            paid = true;
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}
