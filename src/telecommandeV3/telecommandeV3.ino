#include <NRFLite.h>

NRFLite radio;

int X1 = A5;
int Y1 = A6;
int Y2 = A4;
int X2 = A3;

const static uint8_t PIN_CE = 9;
const static uint8_t PIN_CSN = 10;
const static uint8_t ID_EMETTEUR = 1;
const static uint8_t ID_RECEPTEUR = 0;

void setup() {
  pinMode(X1, INPUT);
  pinMode(X2, INPUT);
  pinMode(Y1, INPUT);
  pinMode(Y2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);

  if (!radio.init(ID_EMETTEUR, PIN_CE, PIN_CSN)) {
    Serial.println("Échec de l'initialisation !");
    analogWrite(3, 127);
    delay(100);
    analogWrite(3, 0);
    delay(100);
    analogWrite(3, 127);
    delay(100);
    analogWrite(3, 0);
    delay(100);
    analogWrite(3, 127);
    delay(100);
    analogWrite(3, 0);
    delay(100);
    while (1)
      ;
  }
  Serial.print("ok");
  analogWrite(3, 127);
}  

void loop() {
  handle_J2Y(analogRead(Y2));
  delay(40);
  handle_J1X(analogRead(Y1));
  delay(40);
  handle_J1Y(analogRead(X1));
  delay(40);
  handle_J2X(analogRead(X2));
  delay(40);
}

void handle_J1X(int value) {
  String direction;

  if (value > 530) {
    if (value > 990) {
      direction = "L4";
    } else if (value > 880) {
      direction = "L3";
    } else if (value > 780) {
      direction = "L2";
    } else {
      direction = "L1";
    }
  } else if (value < 490) {
    if (value < 100) {
      direction = "R4";
    } else if (value < 200) {
      direction = "R3";
    } else if (value < 300) {
      direction = "R2";
    } else {
      direction = "R1";
    }
  } else {
    direction = "RL";  //neutre
  }

  char message[5];
  strcpy(message, direction.c_str());

  radio.send(ID_RECEPTEUR, &message, sizeof(message));
  //Serial.println(message);
}

void handle_J1Y(int value) {
  String height;

  if (value > 990) {
    height = "M4";
  } else if (value > 880) {
    height = "M3";
  } else if (value > 780) {
    height = "M2";
  } else if (value > 500) {
    height = "M1";
  } else if (value > 400) {
    height = "DM";
  } else if (value > 300) {
    height = "D1";
  } else if (value > 200) {
    height = "D2";
  } else if (value > 100) {
    height = "D3";
  } else {
    height = "D4";
  }


  int x2Value = analogRead(X2);
  bool x2IsNeutral = x2Value >= 490 && x2Value <= 530;

  if (height == "DM" && !x2IsNeutral) {
    return;
  }

  char message[5];
  strcpy(message, height.c_str());

  radio.send(ID_RECEPTEUR, &message, sizeof(message));
  Serial.println(message);
}


void handle_J2X(int value) {
  String direction;

  if (value > 530) { 
    if (value > 990) {
      direction = "LA4";
    } else if (value > 880) {
      direction = "LA3";
    } else if (value > 780) {
      direction = "LA2";
    } else {
      direction = "LA1";
    }
  } else if (value < 490) {
    if (value < 100) {
      direction = "RA4";
    } else if (value < 200) {
      direction = "RA3";
    } else if (value < 300) {
      direction = "RA2";
    } else {
      direction = "RA1";
    }
  } else {
    direction = "RAL";
  }

  char message[5];
  strcpy(message, direction.c_str());

  radio.send(ID_RECEPTEUR, &message, sizeof(message));
  Serial.println(message);
}

void handle_J2Y(int value) {
  String m;
  if (value > 990) {
    m = "gaz0";
  } else if (value > 880) {
    m = "gaz1";
  } else if (value > 780) {
    m = "gaz2";
  } else if (value > 700) {
    m = "gaz3";
  } else if (value > 620) {
    m = "gaz4";
  } else if (value > 512) {
    m = "gaz5";
  } else if (value > 470) {
    m = "gaz6";
  } else if (value > 360) {
    m = "gaz7";
  } else if (value > 280) {
    m = "gaz8";
  } else if (value > 225) {
    m = "gaz9";
  } else if (value > 175) {
    m = "gaz10";
  } else if (value > 100) {
    m = "gaz11";
  } else if (value > 50) {
    m = "gaz12";
  } else {
    m = "gaz13";
  }

  char message[5];
  strcpy(message, m.c_str());

  radio.send(ID_RECEPTEUR, &message, sizeof(message));
  //Serial.println(message);
}