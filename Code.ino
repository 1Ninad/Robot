#define LS A0
#define RS A1

#define LM1 7
#define LM2 8
#define RM1 5
#define RM2 6

#define EN1 9
#define EN2 10

#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define MAX_DISTANCE 200
#define OBSTACLE_DISTANCE 5

bool isFollowingLine = true;
unsigned long crossTime = 0;
const unsigned long CROSSING_DELAY = 5000;

void setup() {
  pinMode(LS, INPUT);
  pinMode(RS, INPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
}

void moveMotors(int leftSpeed, int rightSpeed) {
  digitalWrite(LM1, (leftSpeed > 0) ? HIGH : LOW);
  digitalWrite(LM2, (leftSpeed > 0) ? LOW : HIGH);
  analogWrite(EN1, abs(leftSpeed));

  digitalWrite(RM1, (rightSpeed > 0) ? HIGH : LOW);
  digitalWrite(RM2, (rightSpeed > 0) ? LOW : HIGH);
  analogWrite(EN2, abs(rightSpeed));
}

void stopMotors() {
  moveMotors(0, 0);
}

void turnLeft(int speed) {
  moveMotors(-speed, speed);
}

void turnRight(int speed) {
  moveMotors(speed, -speed);
}

void moveForward(int speed) {
  moveMotors(speed, speed);
}

void stop() {
  stopMotors();
}

void loop() {
  int leftSensorValue = analogRead(LS);
  int rightSensorValue = analogRead(RS);

  unsigned long duration;
  unsigned int distance;

  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration / 58;

  int threshold = 500;

  if (isFollowingLine) {
    if (distance <= OBSTACLE_DISTANCE) {
      stop();
      delay(1000);
    } else {
      moveForward(25);
    }

    if (leftSensorValue < threshold && rightSensorValue < threshold) {
      moveForward(250);
      crossTime = 0;
    } else if (leftSensorValue < threshold && rightSensorValue >= threshold) {
      turnLeft(250);
      crossTime = 0;
    } else if (leftSensorValue >= threshold && rightSensorValue < threshold) {
      turnRight(250);
      crossTime = 0;
    } else {
      stop();
      if (crossTime == 0) {
        crossTime = millis();
      } else if (millis() - crossTime >= CROSSING_DELAY) {
        isFollowingLine = true;
        crossTime = 0;
      }
    }
  } else {
    stop();
    if (crossTime == 0) {
      crossTime = millis();
    } else if (millis() - crossTime >= CROSSING_DELAY) {
      isFollowingLine = true;
      crossTime = 0;
    }
  }
}
