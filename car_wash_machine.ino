
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
const int OBSTACLE_NEAR = 15;

// Motor Control
int currentDirection = 1; // 1: forward, -1: backward
const int MOTOR_SPEED = 125; // Speed of the motor (0-255)

void setup() {
  // Set motor pins as outputs
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  // Set ultrasonic sensor pins
  pinMode(ULTRASONIC_TRIG_N, OUTPUT);
  pinMode(ULTRASONIC_ECHO_N, INPUT);
  pinMode(ULTRASONIC_TRIG_S, OUTPUT);
  pinMode(ULTRASONIC_ECHO_S, INPUT);

  Serial.begin(9600);
}

void loop() {
  int distanceNorth = readDistanceCMNorth();
  delay(100); // Small delay to avoid interference
  int distanceSouth = readDistanceCMSouth();
  Serial.print("Distance North: ");
  Serial.print(distanceNorth);
  Serial.print(" cm,\n ");

  Serial.print("Distance South: ");
  Serial.print(distanceSouth);
  Serial.print(" cm,\n");
  // Move the motor in the current direction
  
  Serial.print("Distance South: ");
  Serial.print(distanceSouth);
  // if the distance south is less than the minimum distance then reverse the direction
  // if the distance north is less than the minimum distance then reverse the direction
  // if the distance south is greater than the minimum distance then move forward
  // if the distance north is greater than the minimum distance then move forward
  if (distanceSouth < OBSTACLE_NEAR) {
    Serial.println("Obstacle detected South, reversing direction");
    reverseDirection();
    stopMotor();
    moveMotor(currentDirection);
  
  }
  if (distanceNorth < OBSTACLE_NEAR) {
    Serial.println("Obstacle detected North, reversing direction");
    reverseDirection();
    stopMotor();
    moveMotor(currentDirection);
  }  

  delay(100);
}

// Reads the distance using HC-SR04
int readDistanceCMNorth() {
  digitalWrite(ULTRASONIC_TRIG_N, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_N, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_N, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_N, HIGH);
  int distanceCM = duration * 0.034 / 2;

  return distanceCM;
}

int readDistanceCMSouth() {
  digitalWrite(ULTRASONIC_TRIG_S, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_S, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_S, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_S, HIGH);
  int distanceCM = duration * 0.034 / 2;

  return distanceCM;
}

// Controls the motor movement direction
void moveMotor(int direction) {
  if (direction == 1) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_PWM, MOTOR_SPEED);
  } else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    analogWrite(MOTOR_PWM, MOTOR_SPEED);
  }
}

// Stops the motor
void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
}

// Reverses motor direction
void reverseDirection() {
  currentDirection *= -1;
}

