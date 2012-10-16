
// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <avr/wdt.h>

AF_DCMotor motor1(1, MOTOR34_64KHZ);
AF_DCMotor motor2(2, MOTOR34_64KHZ);
AF_DCMotor motor3(3, MOTOR34_64KHZ);
AF_DCMotor motor4(4, MOTOR34_64KHZ);

void setup() {
  wdt_reset();
  Serial.begin(9600);           // set up Serial library at 9600 bps
//  Serial.printlin("Motor test!");

  // turn on motor
  motor1.setSpeed(200);
 
  motor1.run(RELEASE);
  
//  wdt_enable(WDTO_2S  );
 
}

void motors_setSpeed(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}

void motors_run(int mode) {
  motor1.run(mode);
  motor2.run(mode);
  motor3.run(mode);
  motor4.run(mode);
}

int getMotorState(char c) {
  if (c > 0) {
      return FORWARD;
  } else if (c < 0) {
      return BACKWARD;
  } else {
      return RELEASE;
  }
}

void set_motor(int motor, int state, int vel) {
  vel = vel * 2;

  
//      ;;Serial.print("set_motor %d %d %d", motor, state, vel);
  /*    
  if (vel < 100) {
    vel = 100;
  }
  */
  
  switch (motor) {
    case 0:
      motor1.setSpeed(vel);
      motor1.run(state);   
      break;
    case 1:
      motor2.setSpeed(vel);
      motor2.run(state);      
      break;
    case 2:
      motor3.setSpeed(vel);
      motor3.run(state);    
      break;
    case 3:
      motor4.setSpeed(vel);
      motor4.run(state);
      break;
  }
}

unsigned long timeout = millis() + 10;
unsigned long cmd_timeout = millis();


char command[128];
int num_commands = 0;

void loop() {
  wdt_reset();
  static int foo = 0;
  foo++;
  
//  if ((foo % 1024) == 0)
//    Serial.println("loop");
    
    
  if (millis() > timeout) {
    num_commands = 0;
    motors_run(RELEASE);
//    Serial.println("timeout");
    delay(100);
  }
  
  
  while (Serial.available() > 0 && num_commands < 10) {
    char c = Serial.read();   
    command[num_commands++] = c;
    
    if (c == '\n' && num_commands < 4) {
      num_commands = 0;
      Serial.println("Reset command buffer");
      return;
    }
    
    timeout += 100;
    
  }
  
  if (num_commands == 4) {
      set_motor(1, getMotorState(command[0]), abs(command[0]));
      set_motor(2, getMotorState(command[0]), abs(command[0]));
      set_motor(0, getMotorState(command[1]), abs(command[1]));
      set_motor(3, getMotorState(command[1]), abs(command[1]));
   
    timeout = millis() + 200;
    num_commands = 0;
  }
}

