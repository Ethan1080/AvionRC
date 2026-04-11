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

unsigned long dernierClignotement = 0;
unsigned long dernierMessageRecu = 0;

static unsigned long dernierEnvoi = 0;

String testMessage = "test";

bool etatLed = false;

struct PayloadStruct {
  char type;    // 'X', 'Y', 'A', 'G' pour savoir quel joystick bouge
  int valeur;   // La valeur précise
};

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

  PayloadStruct payload;

  if (millis() - dernierEnvoi > 40) {
    dernierEnvoi = millis();

    handle_J2Y(analogRead(Y2));
    handle_J1Y(analogRead(X1));
    handle_J2X(analogRead(Y1));
  }

  if (millis() - dernierMessageRecu > 350) {

    Serial.println("perte de signal!!");

    if (millis() - dernierClignotement > 400) {
      dernierClignotement = millis();
      etatLed = !etatLed;
      digitalWrite(3, etatLed);
    }

  } else {
    digitalWrite(3, HIGH);
  }
}

void handle_J2X(int value) {
  PayloadStruct payload;
  payload.type = 'D'; // profondeur
  payload.valeur = map(value, 0, 1023, -100, 100);
  
  if (payload.valeur > -5 && payload.valeur < 5) payload.valeur = 0;

  if(radio.send(ID_RECEPTEUR, &payload, sizeof(payload))){
    dernierMessageRecu = millis();
  }
}

void handle_J2Y(int value) {
  PayloadStruct payload;
  payload.type = 'G'; // gaz
  payload.valeur = map(value, 0, 1023, 500, 0);
  
  if (payload.valeur > -5 && payload.valeur < 5) payload.valeur = 0;

  radio.send(ID_RECEPTEUR, &payload, sizeof(payload));
}

void handle_J1Y(int value) {
  PayloadStruct payload;
  payload.type = 'P'; // profondeur
  payload.valeur = map(value, 0, 1023, -100, 100);

  if (payload.valeur > -5 && payload.valeur < 5) payload.valeur = 0;

  radio.send(ID_RECEPTEUR, &payload, sizeof(payload));
}