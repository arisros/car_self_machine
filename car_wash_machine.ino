
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// === Wi-Fi Configuration ===
const char* ssid = "ssid";
const char* password = "pwd";

ESP8266WebServer server(80);

bool systemStarted = false;
bool emergencyStop = false;

// === Motor and Relay Pins ===
const int MOTOR_PWM = D5;
const int MOTOR_IN1 = D6;
const int MOTOR_IN2 = D7;

const int WATER_SOAP_RELAY = D8;
const int BRUSH_RELAY = D0;
const int FAN_RELAY = D2;

// === Sequence Data ===
const String SEQUENCES[] = {
  "WASH", "WASH", "WASH", "WASH",
  "BRUSH", "BRUSH", "BRUSH",
  "AIRFLOW", "AIRFLOW", "AIRFLOW"
};
const int SEQUENCE_LENGTH = sizeof(SEQUENCES) / sizeof(SEQUENCES[0]);

int sequenceIndex = 0;
bool goingForward = true;
bool sequenceCompleted = false;
String lastExecutedStep = "";

int currentDirection = 1;
const int MOTOR_SPEED = 220;

// === Ultrasonic Sensor Pins ===
const int ULTRASONIC_TRIG_S = D3;
const int ULTRASONIC_ECHO_S = D4;

// === Setup ===
void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  pinMode(WATER_SOAP_RELAY, OUTPUT);
  pinMode(BRUSH_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);

  pinMode(ULTRASONIC_TRIG_S, OUTPUT);
  pinMode(ULTRASONIC_ECHO_S, INPUT);

  connectWiFi();
  setupWebServer();
}

// === Main Loop ===
void loop() {
  server.handleClient();

  if (!systemStarted || emergencyStop || sequenceCompleted) {
    stopMotor();
    return;
  }

  // Read sensor distance
  int distanceSouth = smoothDistance(readDistanceCMSouth);
  Serial.print("Distance South: ");
  Serial.print(distanceSouth);
  Serial.println(" cm");

  // Obstacle logic based on distance sensor
  if (distanceSouth < 7) {
    currentDirection = 1;
    Serial.println("Obstacle South. Reversing.");
    stopMotor();
    delay(200);
    moveMotor(currentDirection);
    delay(200);
    return;
  }
  if (distanceSouth > 20 ) {
    currentDirection = -1;
    Serial.println("Obstacle North. Moving forward.");
    stopMotor();
    delay(200);
    moveMotor(currentDirection);
    delay(200);
    return;
  }

  String step = SEQUENCES[sequenceIndex];
  if (step != lastExecutedStep) {
    Serial.println("Executing: " + step);
    executeSequence(step);
    lastExecutedStep = step;
  }

  moveMotor(currentDirection);
  delay(500); // simulate motion

  if (goingForward) {
    sequenceIndex++;
    if (sequenceIndex >= SEQUENCE_LENGTH) {
      sequenceIndex -= 2;
      goingForward = false;
      currentDirection = -1;
    }
  } else {
    sequenceIndex--;
    if (sequenceIndex < 0) {
      sequenceCompleted = true;
      systemStarted = false;
    }
  }
  delay(500);
}

// === Sequence Execution ===
void executeSequence(String step) {
  if (step == "WASH") {
    digitalWrite(WATER_SOAP_RELAY, HIGH);
    delay(1000);
    digitalWrite(WATER_SOAP_RELAY, LOW);
  } else if (step == "BRUSH") {
    digitalWrite(BRUSH_RELAY, HIGH);
    delay(1000);
    digitalWrite(BRUSH_RELAY, LOW);
  } else if (step == "AIRFLOW") {
    digitalWrite(FAN_RELAY, HIGH);
    delay(1000);
    digitalWrite(FAN_RELAY, LOW);
  }
}

// === Motor Functions ===
void moveMotor(int direction) {
  if (direction == 1) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  } else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  }
  analogWrite(MOTOR_PWM, MOTOR_SPEED);
}

void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
  digitalWrite(WATER_SOAP_RELAY, LOW);
  digitalWrite(BRUSH_RELAY, LOW);
  digitalWrite(FAN_RELAY, LOW);
}

// === Wi-Fi Connection ===
void connectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// === Web Server Setup ===
void setupWebServer() {
  server.on("/", []() {
    server.send(200, "text/html", R"rawliteral(
      <html><body>
      <h1>Car Wash Control</h1>
      <form action="/start" method="POST"><button>▶️ Start</button></form>
      <form action="/stop" method="POST"><button>⏹️ Emergency Stop</button></form>
      </body></html>
    )rawliteral");
    Serial.println("Web page served.");
  });

  server.on("/start", HTTP_POST, []() {
    systemStarted = true;
    emergencyStop = false;
    sequenceIndex = 0;
    goingForward = true;
    sequenceCompleted = false;
    lastExecutedStep = "";
    server.sendHeader("Location", "/");
    server.send(303);
    Serial.println("System started.");
  });

  server.on("/stop", HTTP_POST, []() {
    emergencyStop = true;
    systemStarted = false;
    stopMotor();
    server.sendHeader("Location", "/");
    server.send(303);
    Serial.println("Emergency stop activated.");
  });

  server.begin();
  Serial.println("Web server started.");
}

// === Ultrasonic Sensor Reading ===
int smoothDistance(int (*readFunc)()) {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    total += readFunc();
    delay(10);
  }
  return total / 3;
}

// Read distance from the sensor
int readDistanceCMSouth() {
  digitalWrite(ULTRASONIC_TRIG_S, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_S, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_S, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_S, HIGH);
  return duration * 0.034 / 2; // Convert duration to distance in cm
}


