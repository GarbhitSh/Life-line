#include <IRremote.h>
//
#include <Dabble.h>
#include <math.h>

#define IR_RECEIVER_PIN 4

#define NEON_LIGHT 9

#define MOTOR_1_SPEED 5
#define MOTOR_1_DIR1 6
#define MOTOR_1_DIR2 8

#define MOTOR_2_SPEED 10
#define MOTOR_2_DIR1 11
#define MOTOR_2_DIR2 13

#define IR_SENSOR_LEFT A3
#define IR_SENSOR_RIGHT A2

#define TRIG_PIN A0
#define ECHO_PIN A1

#define BUTTON 2
#define LED 3
#define LDR A6
#define POT A7

#define ARROW_UP_BUTTON 24
#define ARROW_DOWN_BUTTON 82
#define ARROW_RIGHT_BUTTON 90
#define ARROW_LEFT_BUTTON 8
#define OK_BUTTON 28
#define STAR_BUTTON 22
#define HASH_BUTTON 13
#define NUM_0_BUTTON 25
#define NUM_1_BUTTON 69
#define NUM_2_BUTTON 70
#define NUM_3_BUTTON 71

#define NUM_4_BUTTON 68
#define NUM_5_BUTTON 64
#define NUM_6_BUTTON 67
#define NUM_7_BUTTON 7
#define NUM_8_BUTTON 21
#define NUM_9_BUTTON 9

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

long duration;
int distance;
unsigned long preMillis;
unsigned long preMillis_IR;
unsigned long millis_stopper;
int counter;
int remote_data;
int led_counter = 0;
int led_start_counter = 0;
int ultrasonic_counter = 0;
double ultrasonic_reading = 0;

bool flag_blink = false;
bool flag_blink_counter = false;
bool flag_breath = false;
bool flag_breath_counter = false;
bool flag_led = false;

int speed_IR = 150;
int speed_app = 250;

int speed_obstacle_avoider = 150;
int speed_line_follower = 120;

void BACKWARD(int speed) {
  analogWrite(MOTOR_1_SPEED, speed);
  analogWrite(MOTOR_2_SPEED, speed);

  digitalWrite(MOTOR_1_DIR1, LOW);
  digitalWrite(MOTOR_1_DIR2, HIGH);

  digitalWrite(MOTOR_2_DIR1, LOW);
  digitalWrite(MOTOR_2_DIR2, HIGH);
}

void FORWARD(int speed) {
  analogWrite(MOTOR_1_SPEED, speed);
  analogWrite(MOTOR_2_SPEED, speed);

  digitalWrite(MOTOR_1_DIR1, HIGH);
  digitalWrite(MOTOR_1_DIR2, LOW);

  digitalWrite(MOTOR_2_DIR1, HIGH);
  digitalWrite(MOTOR_2_DIR2, LOW);
}

void LEFT(int speed) {
  analogWrite(MOTOR_1_SPEED, speed);
  analogWrite(MOTOR_2_SPEED, speed);

  digitalWrite(MOTOR_1_DIR1, HIGH);
  digitalWrite(MOTOR_1_DIR2, LOW);

  digitalWrite(MOTOR_2_DIR1, LOW);
  digitalWrite(MOTOR_2_DIR2, HIGH);
}

void RIGHT(int speed) {
  analogWrite(MOTOR_1_SPEED, speed);
  analogWrite(MOTOR_2_SPEED, speed);

  digitalWrite(MOTOR_1_DIR1, LOW);
  digitalWrite(MOTOR_1_DIR2, HIGH);

  digitalWrite(MOTOR_2_DIR1, HIGH);
  digitalWrite(MOTOR_2_DIR2, LOW);
}

void STOP() {
  analogWrite(MOTOR_1_SPEED, 0);
  analogWrite(MOTOR_2_SPEED, 0);

  digitalWrite(MOTOR_1_DIR1, LOW);
  digitalWrite(MOTOR_1_DIR2, LOW);

  digitalWrite(MOTOR_2_DIR1, LOW);
  digitalWrite(MOTOR_2_DIR2, LOW);
}

int ULTRASONIC() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = (0.034 * duration) / 2;

  //Serial.print("DISTANCE: ");
  Serial.println(distance);

  return (distance);
}

void BLINK(int time) {
  if (flag_led && flag_blink) {
    if (millis() - preMillis > time) {
      if (flag_blink_counter) {
        digitalWrite(NEON_LIGHT, HIGH);
        flag_blink_counter = false;
      } else {
        digitalWrite(NEON_LIGHT, LOW);
        flag_blink_counter = true;
      }

      preMillis = millis();
    }
  }
}

void BREATH(int time) {
  if (flag_led && flag_breath) {
    if ((millis() - preMillis > time) && !flag_breath_counter) {
      analogWrite(NEON_LIGHT, counter);
      counter += 1;
      if (counter == 255)
        flag_breath_counter = true;
      preMillis = millis();
    } else if ((millis() - preMillis > time) && flag_breath_counter) {
      analogWrite(NEON_LIGHT, counter);
      counter -= 1;
      if (counter == 0)
        flag_breath_counter = false;
      preMillis = millis();
    }
  }
}

void setup() {
  pinMode(MOTOR_1_SPEED, OUTPUT);
  pinMode(MOTOR_1_DIR1, OUTPUT);
  pinMode(MOTOR_1_DIR2, OUTPUT);

  pinMode(MOTOR_2_SPEED, OUTPUT);
  pinMode(MOTOR_2_DIR1, OUTPUT);
  pinMode(MOTOR_2_DIR2, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);

  pinMode(NEON_LIGHT, OUTPUT);
  digitalWrite(NEON_LIGHT, LOW);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LDR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(POT, INPUT);

  IrReceiver.begin(IR_RECEIVER_PIN);

  Serial.begin(250000);
  Dabble.begin(9600, 12, 7);
}

void loop() {

  BLINK(250);
  BREATH(10);
   
  if (IrReceiver.decode()) {
    IrReceiver.resume();
    remote_data = IrReceiver.decodedIRData.command;

    Serial.println(remote_data);

    if (remote_data == STAR_BUTTON) {
      Serial.println("MANUAL MODE");
      digitalWrite(3,HIGH);
      while (true) {

        BLINK(250);
        BREATH(5);

        flag_led = true;

        if (IrReceiver.decode()) {
          IrReceiver.resume();
          remote_data = IrReceiver.decodedIRData.command;

          if (remote_data == 0)
            continue;

          preMillis_IR = millis();

          if (remote_data == NUM_1_BUTTON || remote_data == NUM_3_BUTTON || remote_data == HASH_BUTTON) {
            
            break;
            
          } else {
            switch (remote_data) {
              case ARROW_UP_BUTTON:
                Serial.println("FORWARD");
                FORWARD(speed_IR);
                break;

              case ARROW_DOWN_BUTTON:
                Serial.println("BACKWARD");
                BACKWARD(speed_IR);
                break;

              case ARROW_RIGHT_BUTTON:
                Serial.println("RIGHT");
                RIGHT(speed_IR);
                break;

              case ARROW_LEFT_BUTTON:
                Serial.println("LEFT");
                LEFT(speed_IR);
                break;

              case OK_BUTTON:
                Serial.println("STOP");
                STOP();
                break;

              case NUM_4_BUTTON:
                Serial.println("SLOW");
                speed_IR = 140;
                break;

              case NUM_6_BUTTON:
                Serial.println("FAST");
                speed_IR = 180;
                break;
                

              case NUM_7_BUTTON:
                Serial.println("BREATHING");
                flag_blink = false;
                flag_breath = true;
                preMillis = millis();
                break;

              case NUM_8_BUTTON:
                Serial.println("BLINKING");
                flag_blink = true;
                flag_breath = false;
                flag_blink_counter = true;
                preMillis = millis();
                break;

              case NUM_9_BUTTON:
                Serial.println("LED STOPPED");
                digitalWrite(NEON_LIGHT, LOW);
                flag_blink = false;
                flag_breath = false;
                counter = 0;
                flag_breath_counter = false;
                preMillis = millis();
                break;

              default:
                Serial.println("MODE CHANGE");
                break;
            }
          }
        } else {
          if (millis() - preMillis_IR > 500) {
            STOP();
            preMillis_IR = millis();
          }
        }
      }
    }

    else if (remote_data == NUM_1_BUTTON) {
      Serial.println("OBSTACLE AVOIDING MODE");
      while (true) {
        if (IrReceiver.decode()) {
          IrReceiver.resume();
          if (IrReceiver.decodedIRData.command == 0) {
          } else {
            break;
          }
        } else {
          if (ULTRASONIC() <= 25) {
            digitalWrite(NEON_LIGHT, HIGH);
            STOP();
            delay(100);
            BACKWARD(speed_obstacle_avoider);
            delay(250);
            RIGHT(speed_obstacle_avoider);
            delay(350);
          } else {
            digitalWrite(NEON_LIGHT, LOW);
            FORWARD(speed_obstacle_avoider);
          }
        }
      }
    }

    else if (remote_data == NUM_3_BUTTON) {
      Serial.println("LINE TRACKING MODE");
      while (true) {
        if (IrReceiver.decode()) {
          IrReceiver.resume();
          if (IrReceiver.decodedIRData.command == 0) {
          } else {
            break;
          }
        } else {
          if (digitalRead(IR_SENSOR_RIGHT) == 1 && digitalRead(IR_SENSOR_LEFT) == 1) {
            STOP();
            break;
          } else if (digitalRead(IR_SENSOR_RIGHT) == 1) {
            RIGHT(110);
          } else if (digitalRead(IR_SENSOR_LEFT) == 1) {
            LEFT(110);
          } else {
            FORWARD(100);
          }
        }
      }
    }
    else if (remote_data == HASH_BUTTON) {
      digitalWrite(3,LOW);
//      break;
      
    }
  }

  //##################################### BLUETOOTH ############################
  
  Dabble.processInput();
  Serial.println("inside dabble");
    if (GamePad.getAngle() >= 45 && GamePad.getAngle() < 135) {
      //Serial.println("FORWARD");
      FORWARD(constrain((int)round(GamePad.getRadius() * 42.5), 0, speed_app));        
      millis_stopper = millis();
    } else if (GamePad.getAngle() >= 135 && GamePad.getAngle() < 225) {
      //Serial.println("LEFT");
      LEFT(constrain((int)round(GamePad.getRadius() * 42.5), 0, speed_app));            
      millis_stopper = millis();
    } else if (GamePad.getAngle() >= 225 && GamePad.getAngle() < 315) {
      //Serial.println("BACKWARD");
      BACKWARD(constrain((int)round(GamePad.getRadius() * 42.5), 0, speed_app));       
      millis_stopper = millis();
    } else if ((GamePad.getAngle() >= 315 || GamePad.getAngle() < 45) && GamePad.getRadius() != 0) {
      //Serial.println("RIGHT");
      RIGHT(constrain((int)round(GamePad.getRadius() * 42.5), 0, speed_app));          
      millis_stopper = millis();
    }

    else if (GamePad.isUpPressed()) {
      FORWARD(speed_app);
      millis_stopper = millis();
    } else if (GamePad.isDownPressed()) {
      BACKWARD(speed_app);
      millis_stopper = millis();
    } else if (GamePad.isRightPressed()) {
      RIGHT(speed_app);
      millis_stopper = millis();
    } else if (GamePad.isLeftPressed()) {
      LEFT(speed_app);
      millis_stopper = millis();
    }

    else if (GamePad.isSelectPressed()) {
      if (led_counter == 0) {
        flag_led = true;
        flag_blink = false;
        flag_breath = true;
        counter = 0;
        flag_breath_counter = false;
        preMillis = millis();
        led_counter = 1;
        //Serial.println("BREATHING");
      } else if (led_counter == 1) {
        flag_blink = true;
        flag_breath = false;
        preMillis = millis();
        led_counter = 2;
        //Serial.println("BLINKING");
      } else if (led_counter == 2) {
        flag_blink = false;
        flag_led = false;
        digitalWrite(NEON_LIGHT, LOW);
        led_counter = 0;
      }
    }

    else if (GamePad.isCrossPressed()) {
      STOP();

      flag_led = false;
      flag_blink = false;
      flag_breath = false;
      digitalWrite(NEON_LIGHT, LOW);
      //Serial.println("STOP");
      delay(100);
    }

    else if (GamePad.isCirclePressed()) {
      //Serial.println("OBSTACLE");
      //Obstacle Avoiding Mode
        while (true) {
          Dabble.processInput();
          if (ULTRASONIC() <= 25) {
            digitalWrite(NEON_LIGHT, HIGH);
            STOP();
            delay(100);
            BACKWARD(speed_obstacle_avoider);
            delay(250);
            RIGHT(speed_obstacle_avoider);
            delay(350);
          } else {
            digitalWrite(NEON_LIGHT, LOW);
            FORWARD(speed_obstacle_avoider);
          }

          if (GamePad.isCrossPressed()) {
            digitalWrite(NEON_LIGHT, LOW);
            break;
          }
        }
    }

    else if (GamePad.isTrianglePressed()) {
      //Serial.println("LINE");
      //Line Tracking Mode
        while (true) {
          Dabble.processInput();
          if (digitalRead(IR_SENSOR_RIGHT) == 1 && digitalRead(IR_SENSOR_LEFT) == 1) {
            STOP();
            digitalWrite(NEON_LIGHT, LOW);
            break;
          } else if (digitalRead(IR_SENSOR_RIGHT) == 1) {
            RIGHT(speed_line_follower);
            digitalWrite(NEON_LIGHT, HIGH);
          } else if (digitalRead(IR_SENSOR_LEFT) == 1) {
            LEFT(speed_line_follower);
            digitalWrite(NEON_LIGHT, HIGH);
          } else {
            FORWARD(speed_line_follower);
            digitalWrite(NEON_LIGHT, LOW);
          }

          if (GamePad.isCrossPressed()) {
            digitalWrite(NEON_LIGHT, LOW);
            break;
          }
        }
    }

    else if (GamePad.isSquarePressed()) {

    }

    if (millis() - millis_stopper > 50) {
      STOP();
      //Serial.println("STOPPED!!!");
    }
  }
