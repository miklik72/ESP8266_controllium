#include <Arduino.h>

/*************************************************************************************************************************
    DEVELOPED BY:   ARTUR FERREIRA MOREIRA
    DATE:           MAY, 13 OF 2017
    EMAIL:          artur31415926@gmail.com
    WEBPAGE:        http://daedalusstone.com/
    INSTAGRAM:      https://www.instagram.com/artur31415/
    GOOGLEPLAY:     https://play.google.com/store/apps/developer?id=Synapse
    YOUTUBE:        https://www.youtube.com/channel/UC6blOB30re0J-Oiksqaob1g/videos
    GITHUB:         https://github.com/artur31415
    TWITTER:        https://twitter.com/artur31415
    LINKEDIN:       https://www.linkedin.com/in/artur31415

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
[ENG-USA]
      This library was developed to work with the Android app "Controllium", with which,
    you can control your ESP8266 projects using your Android device.
      Through the library you can attach any of the UI elements to the Arduino hardware pins
    accordingly to theirs functionalities.
      You can use widgets to send and receive data from the ESP8266 using you Android Device.
      Look at the examples to understand how the library works.

      Any Ideas or criticism, email me.

    Have fun!
[PT-BR]
      Esta biblioteca foi desenvolvida para trabalhar com o aplicativo Android "Controllium", com o qual,
    Você pode controlar seus projetos de ESP8266 usando seu dispositivo Android.
      Através da biblioteca você pode anexar qualquer um dos elementos UI para os pinos de hardware arduino
    De acordo com s  uas funcionalidades.
      Voce pode usar widgets para enviar e receber dados do ESP8266 usando o seu dispositivo Android.
    Veja os exemplos para entender como funciona a biblioteca.

      Quaisquer idéias ou críticas, email me.

    Divirta-se!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ /                                                  Arrows                                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
[ENG-USA]
    Whenever the arrow widgets are pressed on the App, they are sended to the connected ESP8266 an some
    pins show its state, and it is also show at serial monitor.

[PT-BR]
    Quando os widgets de ceta forem pressionados no applicativo, os mesmos são enviados para o ESP8266 que esta conectado
    e alguns pinos mostram os estados dos widgets, os mesmos tão são mostrados no monitor serial.
**************************************************************************************************************************/
#include "Controllium.h"
#include <ESP8266WiFi.h>

//Wifi SSID and Pass
const char* ROUTER_SSID = "YOUR_SSID";
const char* ROUTER_PASS = "YOUR_PASS";

//Soft AP SSID and Pass
char* SOFT_AP_SSID = "Controllium";
char* SOFT_AP_PASS = "0123456789";

//ESP8266 DIGITAL PINS
int D[] = {16, 5, 4, 0, 2, 14, 12, 13, 15};
static const uint8_t motA1 = 5;
static const uint8_t motA2 = 4;
static const uint8_t motB1 = 0;
static const uint8_t motB2 = 14;
static const uint8_t led2 = 2;
static const uint8_t led16 = 16;
static const uint16_t speed_min = 300;
static const uint16_t speed_max = 500;
static const uint16_t speed_acc = 50;
//variable used to control the blinking of the "alive" led
long ledBlinkLastTime = 0;
int speed = speed_min;
byte direction = 1; // 0 = down, 1 - stop , 2 - up

float arrowDownValue = 0;
float arrowLeftValue = 0;
float arrowUpValue = 0;
float arrowRightValue= 0;

//Controllium object
Controllium controllium("ESPcar");//Name of this device, if you have more than one connected, provide a different name to each

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                        UTILS FUNCTIONS BEGIN                                                  /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool accelerate () {
  if (speed < speed_max) {
    speed = speed + speed_acc;
    Serial.print("speed = ");
    Serial.println(speed);
    return true;
  }
  else {
    //Serial.print("maxspeed = ");
    //Serial.println(speed);
    return false;
  }
}

void goUp() {
  if (accelerate()) {
    //Serial.println("UPa");
    analogWrite(motA1, speed);
    analogWrite(motA2, 0);
  }
}

void goDown() {
  if (accelerate()) {
    //Serial.println("DOWNa");
    analogWrite(motA1, 0);
    analogWrite(motA2, speed);
  }
}

void goStop() {
    if(direction < 1) {
      analogWrite(motA1, speed_max);
      analogWrite(motA2, 0);
    } else if(direction > 1) {
      analogWrite(motA1, 0);
      analogWrite(motA2, speed_max);
    }
    delay(200);
    analogWrite(motA1, LOW);
    analogWrite(motA2, LOW);
}

//Jusy a function to blink a pin
void blink(int pin)
{
  for(int i = 0; i < 2; ++i)
  {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
    delay(50);
  }
  digitalWrite(pin, HIGH);
}

//Setup Soft AP
void setupAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  //randomSeed(analogRead(A0) + analogRead(A0));
  //sprintf(SOFT_AP_SSID, "%s_%04d", SOFT_AP_SSID, random(1023));

  WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASS, 6);
}

//Try to connects to WIFI
//If it fails, returns false
//Otherwise, returns true
bool connectToWifi()
{
  WiFi.begin(ROUTER_SSID, ROUTER_PASS);

  // Wait for connect to AP
  Serial.print("\n\nConnecting to ");

  Serial.println(ROUTER_SSID);

  int tries=0;

  while (WiFi.status() != WL_CONNECTED)
   {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30)
    {
      break;
    }
  }

  Serial.println();

  if (tries > 30)
  {
    Serial.println("Failed To Connect, initializing the Soft AP!\n\n");
    return false;
  }

  return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                        UTILS FUNCTIONS BEGIN                                                  /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  //Init the serial for debug
  Serial.begin(115200);

  //Init the ESP pins
  for(int i = 0; i < 9; ++i)
  {
    pinMode(D[i], OUTPUT);
    digitalWrite(D[i], LOW);
  }
  digitalWrite(led2, HIGH);
  digitalWrite(led16, HIGH);

  // if(connectToWifi())//Try to connect to WIFI
  // {
  //   Serial.println("Connected!");
  //   Serial.print("IP Address: ");
  //   Serial.println(WiFi.localIP());
  //
  //   digitalWrite(D[1], HIGH);// Pin to show that it is connected to wifi
  // }
  // else//Otherwise, setup a soft AP
  // {
    setupAP();

    Serial.println("Soft AP On!");

    Serial.print("\nSSID = ");
    Serial.println(SOFT_AP_SSID);
    Serial.print("PASS = ");
    Serial.println(SOFT_AP_PASS);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    //digitalWrite(D[2], HIGH);// Pin to show that it is NOT connected to wifi
  //}

  //Begin server
  Serial.print("\nUdp server started at port ");
  Serial.println(UDP_DEFAULT_PORT);
  controllium.BeginUdpServer(UDP_DEFAULT_PORT);

  //Instantiate Controllium Devices and add them to the controllium object
  ControlliumDevice arrowDown("ARROW_DOWN_0", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice arrowLeft("ARROW_LEFT_0", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice arrowUp("ARROW_UP_0", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice arrowRight("ARROW_RIGHT_0", DEVICE_TYPE_BUTTON, "0");
  // ControlliumDevice calibrate("BTN_CALIBATE", DEVICE_TYPE_BUTTON, "0");
  // ControlliumDevice switch0("SWITCH_0", DEVICE_TYPE_BUTTON, "0");
  // ControlliumDevice accelerometer("Acc", DEVICE_TYPE_ACCELEROMETER, "no_data");



  controllium.AddDevice(arrowDown);
  controllium.AddDevice(arrowLeft);
  controllium.AddDevice(arrowUp);
  controllium.AddDevice(arrowRight);
  // controllium.AddDevice(calibrate);
  // controllium.AddDevice(switch0);
  // controllium.AddDevice(accelerometer);

  Serial.println("Waiting for clients to connect...");
  blink(led2);
}

void loop()
{
  //Just a blinking led to show that the device is alive
  // if(millis() - ledBlinkLastTime > 2000)
  // {
  //   ledBlinkLastTime = millis();
  //   blink(D[3]);
  // }
  if(controllium.Update(200))//Returns true if a new msg was received
  {
    Serial.print(millis());
    Serial.println(" New MSG!\n\n");
    //ARROWS TO PINS
    arrowDownValue = (float)controllium.GetDevice(0)->GetDoubleValue();
    arrowLeftValue = (float)controllium.GetDevice(1)->GetDoubleValue();
    arrowUpValue = (float)controllium.GetDevice(2)->GetDoubleValue();
    arrowRightValue = (float)controllium.GetDevice(3)->GetDoubleValue();
    // float btn_calibrate = (float)controllium.GetDevice(4)->GetDoubleValue();
    // int switch0 = (int)controllium.GetDevice("SWITCH_0")->GetDoubleValue();
    // String _accX = controllium.GetDevice("Acc")->GetValueFromRaw(0);
    // String _accY = controllium.GetDevice("Acc")->GetValueFromRaw(1);
    // String _accZ = controllium.GetDevice("Acc")->GetValueFromRaw(2);

    Serial.print("\nArrowDown = ");
    Serial.println(arrowDownValue);
    Serial.print("ArrowLeft = ");
    Serial.println(arrowLeftValue);
    Serial.print("ArrowUp = ");
    Serial.println(arrowUpValue);
    Serial.print("ArrowRight = ");
    Serial.println(arrowRightValue);
    Serial.print("ArrowRight = ");
    Serial.println(arrowRightValue);
    // Serial.print("Calibrate = ");
    // Serial.println(btn_calibrate);
    // Serial.print("Switch = ");
    // Serial.println(switch0);

    // Serial.print("Acc Data= {_accX: ");
    // Serial.print(_accX);
    //
    // Serial.print(", _accY: ");
    // Serial.print(_accY);
    //
    // Serial.print(". _accZ: ");
    // Serial.print(_accZ);
    //
    // Serial.println(" }");

    if(arrowLeftValue > 0) {
      //digitalWrite(D[5], (int)arrowLeftValue);
      digitalWrite(motB1, HIGH);
      digitalWrite(motB2, LOW);
    } else if(arrowRightValue > 0) {
      //digitalWrite(D[7], (int)arrowRightValue);
      digitalWrite(motB1, LOW);
      digitalWrite(motB2, HIGH);
    } else {
      digitalWrite(motB1, LOW);
      digitalWrite(motB2, LOW);
    }
  }

  if(arrowUpValue > 0) {
    //digitalWrite(D[6], (int)arrowUpValue);
    goUp();
    direction = 2;
    //Serial.println("UP");
  } else if(arrowDownValue > 0) {
    //digitalWrite(D[4], (int)arrowDownValue);
    goDown();
    direction = 0;
    //Serial.println("DOWN");
  } else if(speed > speed_min) {
    speed = speed_min;
    goStop();
    direction = 1;
  }

  digitalWrite(led16, (arrowDownValue + arrowLeftValue + arrowRightValue + arrowUpValue > 0) ? LOW : HIGH);
}
