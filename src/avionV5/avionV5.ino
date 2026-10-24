#include <Servo.h>
#include <SPI.h>
#include <NRFLite.h>

NRFLite _radio;

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;
const static uint8_t ID_RECEPTEUR = 0;
#define ID_EMETTEUR 0

const static uint8_t PIN_CE = 9;
const static uint8_t PIN_CSN = 10;

Servo empennage;
Servo derive;
Servo pal_droite;
Servo pal_gauche;
Servo esc_motor;

int pos_neutre_derive = 80;
int amplitude_derive = 40;

int pos_neutre_empennage = 80;

int pos_neutre_pal_gauche = 65;
int pos_neutre_pal_droite = 65;

int amplitude_pal_up = 30;
int amplitude_pal_down = 5; //useless

int amplitude_empennage_up = 60;
int amplitude_empennage_down = 60;

int angle_min = pos_neutre_empennage - amplitude_empennage_down;
int angle_max = pos_neutre_empennage + amplitude_empennage_up;

unsigned long dernierMessageRecu = 0;

String _radioData;
String dernierMessage = "";

struct PayloadStruct {
  char type;   // 'X', 'Y', 'A' ou 'G'
  int valeur;  // Le chiffre précis (ex: -100 à 100)
};

void setup() {
  Serial.begin(9600);
  if (!_radio.init(RADIO_ID, PIN_CE, PIN_CSN)) {
    Serial.println("Erreur radio");
    while (1)
      ;
  }
  Serial.println("OK");

  pal_droite.attach(2);
  pal_gauche.attach(3);
  empennage.attach(4);
  derive.attach(6);

  esc_motor.attach(5);

  empennage.write(pos_neutre_empennage);

  derive.write(pos_neutre_derive);

  pal_droite.write(pos_neutre_pal_droite);
  pal_gauche.write(pos_neutre_pal_gauche);

  esc_motor.write(1000);

  delay(2000);
}


void controlAvion(PayloadStruct received) {
  char type = received.type;
  int val = received.valeur;
  if (type == 'G') {  //moteur
    esc_motor.write(map(val, 0, 500, 1000, 1500));
  } else if (type == 'P') {  //empennage
    if (val == 0) {
      empennage.write(pos_neutre_empennage);
    } else {
      if (val > 0) {
        int angle = map(val, 0, -100, pos_neutre_empennage, angle_min);
        empennage.write(angle);
      } else if (val < 0) {
        int angle = map(val, 0, 100, pos_neutre_empennage, angle_max);
        empennage.write(angle);
      } else {
        empennage.write(pos_neutre_empennage);
      }
    }
  } else if (type == 'A') {  //ailerons
    if (val == 0) {
      pal_droite.write(pos_neutre_pal_droite);
      pal_gauche.write(pos_neutre_pal_gauche);

    } else if (val > 0) {
      int angle_droite = map(val, 1, 100, pos_neutre_pal_droite, pos_neutre_pal_droite + amplitude_pal_up);
      int angle_gauche = map(val, 1, 100, pos_neutre_pal_gauche, pos_neutre_pal_gauche + amplitude_pal_up);

      pal_droite.write(angle_droite);
      pal_gauche.write(angle_gauche);

    } else {  // val < 0
      int angle_droite = map(val, -1, -100, pos_neutre_pal_droite, pos_neutre_pal_droite - amplitude_pal_up);
      int angle_gauche = map(val, -1, -100, pos_neutre_pal_gauche, pos_neutre_pal_gauche - amplitude_pal_up);
      
      pal_droite.write(angle_droite);
      pal_gauche.write(angle_gauche);
    }
  } else if (type == 'D') {  //derive
    if (val == 0) {
      derive.write(pos_neutre_derive);
    } else if (val > 0) {
      derive.write(map(val, 1, 100, pos_neutre_derive, pos_neutre_derive - amplitude_derive));
    } else {  // val < 0
      derive.write(map(val, -1, -100, pos_neutre_derive, pos_neutre_derive + amplitude_derive));
    }
  }
}

void loop() {

  PayloadStruct payload;

  if (_radio.hasData()) {
    _radio.readData(&payload);
    dernierMessageRecu = millis();

    controlAvion(payload);
  }

  static unsigned long dernierPrint = 0;

  if (millis() - dernierMessageRecu > 250) {

    empennage.write(pos_neutre_empennage);

    derive.write(pos_neutre_derive);

    pal_droite.write(pos_neutre_pal_droite);
    pal_gauche.write(pos_neutre_pal_gauche);

    esc_motor.write(1000);

    if (millis() - dernierPrint > 300) {
      Serial.println("perte de signal!!");
      dernierPrint = millis();
    }
  }
}
