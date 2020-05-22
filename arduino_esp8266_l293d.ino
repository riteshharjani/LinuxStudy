

/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourAuth";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YouAreStupid";
char pass[] = "ThisAintRealPassword;)";


const int motorLeft_1 = 5;
const int motorLeft_2 = 16;
const int motorLeft_en = 4;

const int motorRight_1 = 14;
const int motorRight_2 = 12;
const int motorRight_en = 13;



void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // motor_left 
  pinMode(motorLeft_1, OUTPUT);
  pinMode(motorLeft_2, OUTPUT);
  digitalWrite(motorLeft_1, LOW);
  digitalWrite(motorLeft_2, LOW);

  pinMode(motorRight_1, OUTPUT);
  pinMode(motorRight_2, OUTPUT);
  digitalWrite(motorRight_1, LOW);
  digitalWrite(motorRight_2, LOW);
}

void loop()
{
  Blynk.run();
}

BLYNK_WRITE(V1)
{
  int buttonState = param.asInt();
  Serial.print("buttonState = ");
  Serial.println(buttonState);
  digitalWrite(LED_BUILTIN, buttonState);
}

enum motorState {
  MOVE_BACK = -1,
  MOVE_STOP,
  MOVE_FRONT
};

void motor_run(int motor_1, int motor_2, int ms)
{
  switch (ms) {
    case MOVE_BACK:
      digitalWrite(motor_1, LOW);
      digitalWrite(motor_2, HIGH);
      break;
    case MOVE_STOP:
      digitalWrite(motor_1, LOW);
      digitalWrite(motor_2, LOW);
      break;
    case MOVE_FRONT:
      digitalWrite(motor_1, HIGH);
      digitalWrite(motor_2, LOW);
      break;
    default:
      Serial.println("Invalid signal");
  }
  Serial.println("Motor run");
}

BLYNK_WRITE(V2) {
  // This is left side motor control

  int x = param[0].asInt();
  motorState ms;

  if (x >= 0 && x < 512) {
    ms = MOVE_BACK;
  } else if ( x >= 512 && x < 768) {
    ms = MOVE_STOP;
  } else {
    ms = MOVE_FRONT;
  }

  //motor_left_run(ms);
  motor_run(motorLeft_1, motorLeft_2, ms);
  
  Serial.print("MOTOR_L = ");
  Serial.print(x);
  Serial.print("\t");
  Serial.println(ms);

}


BLYNK_WRITE(V3) {

  // This is right side motor control
  
  int x = param[0].asInt();
  motorState ms;

  if (x >= 0 && x < 512) {
    ms = MOVE_BACK;
  } else if ( x >= 512 && x < 768) {
    ms = MOVE_STOP;
  } else {
    ms = MOVE_FRONT;
  }

  //motor_right_run(ms);
  motor_run(motorRight_1, motorRight_2, ms);

  Serial.print("MOTOR_R = ");
  Serial.print(x);
  Serial.print("\t");
  Serial.println(ms);
}
