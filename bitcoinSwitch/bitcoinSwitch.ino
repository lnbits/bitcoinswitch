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

WebSocketsClient webSocket;

void setup() {
    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    #ifdef TFT
    setupTFT();
    printHome(false, false, false);
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
    webSocket.loop();
}

void executePayment(uint8_t *payload) {
  printTFT("Payment received!", 21, 15);
  flashTFT();

  String parts[3]; // pin, time, comment
  // format: {pin-time-comment} where comment is optional
  String payloadStr = String((char *)payload);
  int numParts = splitString(payloadStr, '-', parts, 3);

  int pin = parts[0].toInt();
  printTFT("Pin: " + String(pin), 21, 35);

  int time = parts[1].toInt();
  printTFT("Time: " + String(time), 21, 55);

  String comment = "";
  if (numParts == 3) {
      comment = parts[2];
      Serial.println("[WebSocket] received comment: " + comment);
      printTFT("Comment: " + comment, 21, 75);
  }
  Serial.println("[WebSocket] received pin: " + String(pin) + ", duration: " + String(time));

  if (config_threshold_amount != 0) {
      // If in threshold mode we check the "balance" pushed by the
      // websocket and use the pin/time preset
      // executeThreshold();
      return; // Threshold mode not implemented yet
  }

  // the magic happens here
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);

  printHome(true, true, false);

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
bool ping_toggle = false;
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_ERROR:
            Serial.printf("[WebSocket] Error: %s\n", payload);
            printHome(true, false, false);
            break;
        case WStype_DISCONNECTED:
            Serial.println("[WebSocket] Disconnected!\n");
            printHome(true, false, false);
            break;
        case WStype_CONNECTED:
            Serial.printf("[WebSocket] Connected to url: %s\n", payload);
            // send message to server when Connected
            webSocket.sendTXT("Connected");
            printHome(true, true, false);
            break;
        case WStype_TEXT:
            executePayment(payload);
            break;
        case WStype_BIN:
            Serial.printf("[WebSocket] Received binary data: %s\n", payload);
            break;
        case WStype_FRAGMENT_TEXT_START:
            break;
        case WStype_FRAGMENT_BIN_START:
            break;
        case WStype_FRAGMENT:
            break;
        case WStype_FRAGMENT_FIN:
            break;
        case WStype_PING:
            Serial.printf("[WebSocket] Ping!\n");
            ping_toggle = !ping_toggle;
            printHome(true, true, ping_toggle);
            // pong will be sent automatically
            break;
        case WStype_PONG:
            // is not used
            Serial.printf("[WebSocket] Pong!\n");
            printHome(true, true, true);
            break;
    }
}
