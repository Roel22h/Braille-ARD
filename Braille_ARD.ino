//Librería para controlar servo
#include <Servo.h>
#include <SoftwareSerial.h>

Servo srv[6];                // Lista de Servos, pines y posiciones iniciales
int swnumber[2] = { 2, 4 };  //Lista de botones

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

char inputBuffer[32];
int bufferIndex = 0;

bool inputAnswers[32];

void setup() {
  // Iniciar consola serial
  Serial.begin(9600);

  // Iniciar servos en puerto pwm
  for (int i = 0; i < 6; i++) {
    srv[i].attach(servo[i]);
  }

  // Iniciar botones en puertos
  for (int i = 0; i < 2; i++) {
    pinMode(swnumber[i], INPUT);
  }

  setInitPosition();
  Serial.println();
  Serial.println("========================");
  Serial.println("Ingrese una cadena de caracteres:");
}

void loop() {
  while (Serial.available() > 0) {
    char inputChar = Serial.read();
    setInputBuffer(inputChar);
  }

  if (strlen(inputBuffer) > 0) {
    print();
    showScore();
    clearInputBuffer();
  }
}

void setInitPosition() {
  for (int i = 0; i < 6; i++) {
    srv[i].write(servDownPosition[i]);
  }
}

void setEndPosition() {
  Serial.println();

  for (int i = 0; i < 6; i++) {
    srv[i].write(servUpPosition[i]);
  }

  Serial.println("Servomotores ELEVADOS");
}

void print() {
  Serial.println("Entrada: " + String(inputBuffer));
  Serial.print("Ejecutando... ");

  for (int index = 0; index < (strlen(inputBuffer)); index++) {
    String character = String(inputBuffer[index]);
    Serial.print(character);

    int charPosition = searchCharacterPosition(character);

    if (charPosition != -1) {
      printCharacter(charPosition);
      setAnswer(index);
    } else {
      Serial.println("Caracter no identificado: " + String(character));
    }
  }

  setInitPosition();

  Serial.println();
  Serial.println("Ejecucion terminada.");
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
}

void setAnswer(int index) {
  while (digitalRead(swnumber[0]) == LOW && digitalRead(swnumber[1]) == LOW) {}

  if (digitalRead(swnumber[0]) == HIGH || digitalRead(swnumber[1]) == HIGH) {
    inputAnswers[index] = digitalRead(swnumber[0]) == HIGH ? true : false;
    delay(1000);
  }
}

void setInputBuffer(char inputChar) {
  if (int(inputChar) != 10) {
    inputBuffer[bufferIndex] = inputChar;
    bufferIndex++;
    delay(10);
  }
}

void showScore() {
  Serial.println("RESPUESTAS: ");
   for (int index = 0; index < (strlen(inputBuffer)); index++) {
    Serial.println(inputAnswers[index] ? "Correct" : "Wrong");
  }
}

void clearInputBuffer() {
  for (int index = 0; index < (sizeof(inputBuffer)); index++) {
    inputBuffer[index] = '\0';
    inputAnswers[index] = '\0';
    delay(10);
  }

  bufferIndex = 0;

  Serial.println("========================");
  Serial.println("Ingrese una cadena de caracteres:");
}
