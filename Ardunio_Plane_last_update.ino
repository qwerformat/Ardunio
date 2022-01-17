
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Servo.h>
const char* ssid = "ard";

Servo seitenRuder, hoehenRuder, motorRegler;

int posSeiten = 0;
int posHoehen = 0;
int leistMotor = 0;
unsigned long lastsignal = 0;
int werte[3];
int arrayIndex = 0;
int zaehlSensor = 0;



char buff[100];
int counter = 0;
// Start a TCP Server on port 5045
WiFiServer server(1337);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  Serial.println("");
  server.begin();
  delay(10000);
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  
  seitenRuder.attach(12);
  hoehenRuder.attach(13);
  motorRegler.attach(14);
  seitenRuder.write(85);
  hoehenRuder.write(70);
  motorRegler.write(20);
  delay(300);

  // Start the TCP server
}

void loop() {
  TCPServer();
}

void TCPServer () {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println("Connected to client");
    }
    if (client.available() > 0) {
      int a = client.available();
      Serial.println("Buffer start");
      int len = client.readBytes(buff, a);
      Serial.println(buff);
      Serial.print("Bufferend");

      for (int packetcounter = 0; packetcounter < len; packetcounter++) {
        char ch = buff[packetcounter];
        if (ch >= '0' && ch <= '9')
        {
          werte[arrayIndex] = (werte[arrayIndex] * 10) + (ch - '0');
        }
        else if (ch == ',')
        {
          arrayIndex++;
          if (arrayIndex > 2) arrayIndex = 2;
        }
        else if (ch == '\n')
        {
          //Reduzieren auf Maxmiale und minimale Werte (0,180)
          posSeiten = (int)map(werte[1], 0, 180, 0, 180);
          posHoehen = (int)map(werte[2], 0, 180, 180, 0);
          leistMotor = (int)map(werte[0], 0, 100, 20, 160);
          seitenRuder.write(posSeiten);
          hoehenRuder.write(posHoehen);
          motorRegler.write(leistMotor);

          Serial.println(posSeiten);
          Serial.println(posHoehen);
          Serial.println(leistMotor);

          for (int i = 0; i <= 2; i++) werte[i] = 0;
          arrayIndex = 0;

        }

      }
    }
  }
}
