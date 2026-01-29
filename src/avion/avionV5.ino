#include <Servo.h>
#include <SPI.h>
#include <NRFLite.h>

NRFLite _radio;

const static uint8_t RADIO_ID = 0;   //  identique à ton ancien code qui marche
const static uint8_t DESTINATION_RADIO_ID = 1;

const static uint8_t PIN_CE = 9;
const static uint8_t PIN_CSN = 10;

Servo empennage;
Servo derive;
Servo esc_motor;

// === Position neutre des servos ===
int pos_neutre_derive = 90;
int pos_neutre_empennage = 90;

// === Limites de mouvement ===
int max_derive = 30;     // amplitude max pour ailerons
int max_empennage = 25;   // amplitude max pour gouvernail

unsigned long dernierMessageRecu = 0;

String _radioData;
String dernierMessage = "";

struct PayloadStruct {
  char message[32];
};

void setup() {
  Serial.begin(9600);
  if (!_radio.init(RADIO_ID, PIN_CE, PIN_CSN)) {
    Serial.println("Impossible d'initialiser la radio !");
    while (1);
  }
  Serial.println("Connexion établie !");

  derive.attach(2);
  empennage.attach(3);

  esc_motor.attach(5);

  derive.write(pos_neutre_derive);
  empennage.write(pos_neutre_empennage);

  esc_motor.write(1000);

  delay(2000);
}

void controlAvion(String instruction) {
  instruction.trim(); // Enlever espaces parasites

  if (instruction.startsWith("gaz")) { // ===== GAZ =====
    int niveau = instruction.substring(3).toInt(); // Ex: "gaz7"  7
    niveau = constrain(niveau, 0, 13);
    int esc_val = map(niveau, 0, 13, 1000, 1500); // Signal ESC
    esc_motor.writeMicroseconds(esc_val);
    Serial.print("Gaz : ");
    Serial.println(niveau);
    return;
  }else if (instruction.startsWith("R") && instruction != "RL" && instruction != "RAL") { // ===== TOURNER À DROITE =====
    int force = instruction.substring(1).toInt(); // Ex: R3  3
    force = constrain(force, 1, 4);
    derive.write(pos_neutre_derive + force * (max_derive / 4));
    Serial.print("droite force ");
    Serial.println(force);
    return;
  }else if (instruction.startsWith("L")) { // ===== TOURNER À GAUCHE =====
    int force = instruction.substring(1).toInt(); // Ex: L2  2
    force = constrain(force, 1, 4);
    derive.write(pos_neutre_derive - force * (max_derive / 4));
    Serial.print("gauche force ");
    Serial.println(force);
    return;
  }else if (instruction == "RL") { // ===== TOUT DROIT =====
    derive.write(pos_neutre_derive);
    Serial.println("RL neutre");
    return;
  }else if (instruction.startsWith("M")) { // ===== MONTER =====
    int force = instruction.substring(1).toInt(); // Ex: M3 → 3
    force = constrain(force, 1, 4);
    empennage.write(pos_neutre_empennage + force * (max_empennage / 4));
    Serial.print("Monte force ");
    Serial.println(force);
    return;
  }else if (instruction.startsWith("D")) { // ===== DESCENDRE =====
    if (instruction == "DM") {
      empennage.write(pos_neutre_empennage);
      Serial.println("DM neutre");
      return;
    }
    int force = instruction.substring(1).toInt();
    force = constrain(force, 1, 4);
    empennage.write(pos_neutre_empennage - force * (max_empennage / 4));
    Serial.print("Descend force ");
    Serial.println(force);
    return;
  }
}

void loop() {
  PayloadStruct payload;

  if (_radio.hasData()) {
    _radio.readData(&payload);
    //Serial.println(payload.message);
    dernierMessageRecu = millis();
    String messageRecu = String(payload.message);
    controlAvion(messageRecu);
  }

  if (millis() - dernierMessageRecu > 250) {
    Serial.println("perte de singal!!");
    delay(300);
  } else {
    //signal OK
  }
}