#include <Arduino.h>

#include <WiFi.h>

#include <WebSocketsClient.h>
#include <MPU9250_asukiaaa.h>
#include <math.h>

#define SDA_PIN 21
#define SCL_PIN 22

const char* ssid = "TigerTank";
const char* pwd = "";
WebSocketsClient webSocket;

MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, pitch, roll;
uint8_t packet[3] = {0, 0, 2};

/*sign function for floating point numbers*/
float fsign(float a){
  return a >= 0 ? 1 : -1;
}

void setup() {
  /*Init serial*/
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Begin init");

  /*Init MPU-9250 (accel only)*/
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel(ACC_FULL_SCALE_2_G);
//  mySensor.beginGyro();
//  mySensor.beginMag();
  Serial.println("I2C init done");

  /*Init WiFi*/
  WiFi.begin(ssid, pwd);
  Serial.println("WiFi init done");

  /*Init WebSocket*/
  // server address, port and URL
  webSocket.begin("192.168.4.1", 81, "/");
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}

void loop() {
  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
//    aSqrt = mySensor.accelSqrt();

    /*Calculate pitch and roll angles*/
    pitch = atan2(aX, -aZ);
    roll = atan2(aY, sqrt(aZ*aZ + aX*aX));

    /*Convert angles to velocity request for the tank*/
    if(fabs(pitch) > 0.05)
      packet[0] = (uint8_t)max(min(127*pitch/M_PI_2 + 127, 255.0), 0.0);
    else
      packet[0] = 127;

    if(fabs(roll) > 0.05)
      packet[1] = (uint8_t)max(min(fsign(pitch)*127*roll/M_PI_2 + 127, 255.0), 0.0);
    else
      packet[1] = 127;

    /*Debug info*/
//    Serial.println("accelX: " + String(aX));
//    Serial.println("accelY: " + String(aY));
//    Serial.println("accelZ: " + String(aZ));
//    Serial.println("accelSqrt: " + String(aSqrt));
    Serial.print(pitch);
    Serial.print(", ");
    Serial.print(roll);
    Serial.print(", ");
    Serial.print(packet[0]);
    Serial.print(", ");
    Serial.println(packet[1]);
  } else {
    Serial.println("Cannot read accel values");
    packet[0] = 127;
    packet[1] = 127;
  }

  /*Send the packet if WiFi is connected*/
  if(WiFi.status() == WL_CONNECTED){
    webSocket.sendBIN(packet, 3);
  }else{
    Serial.println("Waiting for WiFi connection");
  }
  
  webSocket.loop();
  delay(50);
}
