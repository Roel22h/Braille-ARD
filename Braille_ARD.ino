//Librería para controlar servo
#include <Servo.h>
#include <SoftwareSerial.h>

Servo srv[6];  // Lista de Servos, pines y posiciones iniciales

int servo[] = { 3, 5, 6, 9, 10, 11 };  // Puertos pwm

int servDownPosition[] = { 155, 170, 170, 175, 166, 162 };  // Posiciones iniciales (calibración de servomotores a una misma altura)
int servUpPosition[] = { 130, 145, 145, 145, 140, 142 };    // Posiciones finales

char chars[] = "abcdefghijklmnopqrstuvwxyz,;:.?!#1234567890 ";  //Arreglo de letras y posiciones para cada servo

int arr[6][44] = {
  { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },  //0
  { 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0 },  //1
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //2
  { 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0 },  //3
  { 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0 },  //4
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }   //5
};

bool calibrated = false;
bool executed = false;
String inputBuffer = "";

void setup() {
  //Iniciar consola serial
  Serial.begin(9600);

  //Iniciar servos en puerto pwm
  for (int i = 0; i < 6; i++) {
    srv[i].attach(servo[i]);
  }
}

void loop() {
  if (!calibrated) {
    setInitPosition();
    calibrated = true;
  }

  if (!executed) {
    Serial.println("Ingrese un carácter:");

    while (Serial.available() == 0) {}

    char inputChar = Serial.read();
    String input = String(inputChar);

    if (int(inputChar) != 10) {
      print(input);
    }

    executed = true;
  }

  if (executed && Serial.available() > 0) {
    executed = false;
  }
}

void setInitPosition() {
  Serial.println();

  for (int i = 0; i < 6; i++) {
    srv[i].write(servDownPosition[i]);
  }

  Serial.println("Servomotores en producciones iniciales.");
}

void setEndPosition() {
  Serial.println();

  for (int i = 0; i < 6; i++) {
    srv[i].write(servUpPosition[i]);
  }

  Serial.println("Servomotores ELEVADOS");
}

void print(String character) {
  int charPosition = searchCharacterPosition(character);

  if (charPosition != -1) {
    printCharacter(charPosition);
    delay(2000);
  } else {
    Serial.println("Caracter no identificado: " + String(character));
  }
}

int searchCharacterPosition(String input) {
  for (int i = 0; i < sizeof(chars); i++) {
    if (input[0] == chars[i]) {
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
}
