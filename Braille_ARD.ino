//Librería para controlar servo
#include <Servo.h>
#include <SoftwareSerial.h>
#include "pitches.h"

// Serial consola JAVA y ARDUINO
SoftwareSerial javaSerial(0, 1);

Servo srv[6];                      // Lista de Servos, pines y posiciones iniciales
int swnumber[4] = { 2, 4, 7, 8 };  // Lista de botones

int servo[] = { 3, 5, 6, 9, 10, 11 };  // Puertos pwm

int servDownPosition[] = { 155, 170, 170, 175, 166, 162 };  // Posiciones iniciales (calibración de servomotores a una misma altura)
int servUpPosition[] = { 130, 145, 145, 145, 140, 142 };    // Posiciones finales

char chars[] = "abcdefghijklmnopqrstuvwxyz,;:.?!#1234567890 ";  // Arreglo de letras y posiciones para cada servo

int arr[6][44] = {
  { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },  //0
  { 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0 },  //1
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //2
  { 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0 },  //3
  { 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0 },  //4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }   //5
};

String inputBuffer;

bool nextOptionSent = false;
bool repeatOptionSent = false;
bool finishOptionSent = false;
bool optionNextAssigned = false;

int startTone = NOTE_B0;

void setup() {
  // Iniciar consola JAVA serial
  javaSerial.begin(9600);

  // Iniciar servos en puerto pwm
  for (int i = 0; i < 6; i++) {
    srv[i].attach(servo[i]);
  }

  // Iniciar botones en puertos
  for (int i = 0; i < 4; i++) {
    pinMode(swnumber[i], INPUT);
  }

  setInitPosition();
}

void loop() {
  while (javaSerial.available() > 0) {
    char inputChar = javaSerial.read();
    setInputBuffer(inputChar);
  }

  if (inputBuffer.length() > 0) {
    if (inputBuffer == "?") {
      sendLastOption();
    } else {
      print();
    }
  }
}

void setInitPosition() {
  for (int i = 0; i < 6; i++) {
    srv[i].write(servDownPosition[i]);
  }

  delay(1000);
}

void setEndPosition() {
  for (int i = 0; i < 6; i++) {
    srv[i].write(servUpPosition[i]);
  }
}

void setInputBuffer(char inputChar) {
  if (int(inputChar) != 10) {
    inputBuffer = inputChar;
    delay(20);
  }
}

void print() {
  int charPosition = searchCharacterPosition(inputBuffer);

  if (charPosition != -1) {
    printCharacter(charPosition);
    sendAnswer();
    sendOptionNext();
    clearProps();
  } else {
    // Serial.println("Caracter no identificado: " + String(character));
  }

  setInitPosition();
}

int searchCharacterPosition(String input) {
  for (int i = 0; i < sizeof(chars); i++) {
    if (tolower(input[0]) == chars[i]) {
      return i;
      break;
    }
  }

  return -1;
}

void printCharacter(int charPosition) {
  for (int i = 0; i < 6; i++) {
    int positionStatus = arr[i][charPosition];

    if (positionStatus == 1) {
      srv[i].write(servUpPosition[i]);
    } else {
      srv[i].write(servDownPosition[i]);
    }
  }

  delay(1000);
}

void sendAnswer() {
  if (!nextOptionSent) {
    while (digitalRead(swnumber[1]) == LOW && digitalRead(swnumber[2]) == LOW) {}

    if (digitalRead(swnumber[1]) == HIGH) {
      javaSerial.print(true);
    } else {
      javaSerial.print(false);
    }

    nextOptionSent = true;
    delay(10);
  } else {
    return;
  }
}

void sendOptionNext() {
  if (!optionNextAssigned) {
    while (digitalRead(swnumber[0]) == LOW) {}
    javaSerial.print("N");
    optionNextAssigned = true;
    delay(10);
  } else {
    return;
  }
}

void sendLastOption() {
  if (!finishOptionSent && !repeatOptionSent) {
    while (digitalRead(swnumber[0]) == LOW && digitalRead(swnumber[3]) == LOW) {}

    if (digitalRead(swnumber[0]) == HIGH) {
      javaSerial.print("F");
      finishOptionSent = true;
    } else {
      javaSerial.print("A");
      repeatOptionSent = true;
    }

    delay(10);
    clearProps();
  } else {
    return;
  }
}

void clearProps() {
  inputBuffer = "";
  delay(10);

  nextOptionSent = false;
  delay(10);
  optionNextAssigned = false;
  delay(10);
  repeatOptionSent = false;
  delay(10);
  finishOptionSent = false;
  delay(10);
}
