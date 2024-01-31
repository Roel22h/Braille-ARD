//Librería para controlar servo
#include <Servo.h>
#include <SoftwareSerial.h>
#include "pitches.h"

// Serial consola JAVA y ARDUINO
SoftwareSerial javaSerial(0, 1);

Servo srv[6];                // Lista de Servos, pines y posiciones iniciales
int swnumber[2] = { 2, 4 };  // Lista de botones

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
bool inputAnswers;

int startTone = NOTE_B0;

void setup() {
  // Iniciar consola JAVA serial
  javaSerial.begin(9600);

  // Iniciar servos en puerto pwm
  for (int i = 0; i < 6; i++) {
    srv[i].attach(servo[i]);
  }

  // Iniciar botones en puertos
  for (int i = 0; i < 2; i++) {
    pinMode(swnumber[i], INPUT);
  }

  setInitPosition();
  tone(7, startTone, 500);
}

void loop() {
  while (javaSerial.available() > 0) {
    char inputChar = javaSerial.read();

    setInputBuffer(inputChar);
  }

  if (inputBuffer.length() > 0) {
    print();
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
    setAnswer();
    sendAnswer();
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

void setAnswer() {
  while (digitalRead(swnumber[0]) == LOW && digitalRead(swnumber[1]) == LOW) {}

  if (digitalRead(swnumber[0]) == HIGH || digitalRead(swnumber[1]) == HIGH) {
    inputAnswers = digitalRead(swnumber[0]) == HIGH ? true : false;
    delay(10);
  }
}

void sendAnswer() {
  javaSerial.print(inputAnswers);
  inputBuffer = "";
}
