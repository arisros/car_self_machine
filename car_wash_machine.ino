
// Motor Driver (L298N) Pin Definitions
const int MOTOR_PWM = D5;    // ENA - PWM speed control
const int MOTOR_IN1 = D6;    // IN1 - Direction control
const int MOTOR_IN2 = D7;    // IN2 - Direction control

// Ultrasonic Sensor (HC-SR04) Pin Definitions
const int ULTRASONIC_TRIG_S = D2;
const int ULTRASONIC_ECHO_S = D1;
const int ULTRASONIC_TRIG_N = D4;
const int ULTRASONIC_ECHO_N = D3;

// Distance Thresholds (in cm)
const int OBSTACLE_NEAR = 25;
const int EMERGENCY_STOP = 5;

// Motor Control
int currentDirection = 1; // 1: forward, -1: backward
const int MOTOR_SPEED = 200; // Speed of the motor (0-255)

void setup() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  pinMode(ULTRASONIC_TRIG_N, OUTPUT);
  pinMode(ULTRASONIC_ECHO_N, INPUT);
  pinMode(ULTRASONIC_TRIG_S, OUTPUT);
  pinMode(ULTRASONIC_ECHO_S, INPUT);

  Serial.begin(9600);
}

void loop() {
  // int distanceNorth = smoothDistance(readDistanceCMNorth);
  int distanceSouth = smoothDistance(readDistanceCMSouth);
  

  // if (distanceNorth >= 400) {
  //   Serial.println("North sensor out of range");
  //   return;
  // }

  // Serial.print("Distance North: ");
  // Serial.print(distanceNorth);
  // Serial.print(" cm, ");

  Serial.print("Distance South: ");
  Serial.print(distanceSouth);
  Serial.println(" cm");

  // Emergency stop for too-close range
  // if (distanceSouth < EMERGENCY_STOP || distanceNorth < EMERGENCY_STOP) {
  //   Serial.println("!!! EMERGENCY STOP !!!");
  //   stopMotor();
  //   delay(500);
  //   return;
  // }

  // Obstacle logic
  if (distanceSouth < 7) {
    currentDirection = -1;
    Serial.println("Obstacle South, reversing direction...");
    stopMotor();
    delay(200);
    /* reverseDirection(); */
    moveMotor(currentDirection);
  } 
  else if (distanceSouth > 23) {
    currentDirection = 1;
    Serial.println("Obstacle North, reversing direction...");
    stopMotor();
    delay(200);
    // reverseDirection();
    moveMotor(currentDirection);
  } 
  else {
    moveMotor(currentDirection);
  }

  delay(100); // shorter delay for faster loop
}

// Read smoothed distance using 3-sample average
int smoothDistance(int (*readFunc)()) {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    total += readFunc();
    delay(10);
  }
  return total / 3;
}

// Reads distance using North HC-SR04
int readDistanceCMNorth() {
  digitalWrite(ULTRASONIC_TRIG_N, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_N, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_N, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_N, HIGH);
  return duration * 0.034 / 2;
}

// Reads distance using South HC-SR04
int readDistanceCMSouth() {
  digitalWrite(ULTRASONIC_TRIG_S, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_S, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_S, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_S, HIGH);
  return duration * 0.034 / 2;
}

// Control motor direction and speed
void moveMotor(int direction) {
  if (direction == 1) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  } else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  }
  analogWrite(MOTOR_PWM, MOTOR_SPEED);
  delay(200); // Allow time for motor to start
  stopMotor();
}

// Stop motor
void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
}

// Reverse direction
void reverseDirection() {
  currentDirection *= -1;
}

