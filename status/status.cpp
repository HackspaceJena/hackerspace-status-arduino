#include <cstddef>
#include <climits>
#include "Arduino.h"

void setup();
void testLeds();
void loop();
void changeStateTo(char state_new);
bool transition();
void sendState();
unsigned long calcStateTime();
/*
 * es gibt folgende Zustände:
 * 0 - Aus
 * 1 - An, aber auf dem weg zu aus
 * 2 - An
 */
constexpr char STATE_OFF = 3;
constexpr char STATE_HALF = 1;
constexpr char STATE_ON = 2;

/*
 * Zeit wie lange in einem Zustände verharrt werden soll
 * bis zum nächsten umgeschaltet wird in Millisekunden.
 * TIME_HALF - Zeitspanne von Zustand 2 bis Wechsel zu Zustand 1
 * TIME_OFF  - Zeitspanne von Zustand 2 bis Wechsel zu Zustand 0
 */
constexpr int TIME_HALF = 5400000; // 1,5h
constexpr int TIME_OFF = 7200000; // 2h

// Ein-/Ausgänge Bezeichnen
constexpr int BTN_ON = 2;  // Einschalter
constexpr int BTN_OFF = 3; // Ausschalter
constexpr int LED_G = 9;   // grüne LED
constexpr int LED_Y = 8;   // gelbe LED
constexpr int LED_R = 7;   // rote LED

// hier wird der aktuelle und vorherige Zustand gespeichert
char state_current = STATE_OFF;
char state_previous = STATE_OFF;

// hier wird der Beginn des aktuellen Zustand gespeichert in Millisekunden nach Uptime.
unsigned long stateBegan;

// Debouncer
class Debounce
{
  public:
    Debounce(int pin);
    bool update();
    int read();
  private:
    int _pin;
    int _state;
    int _time;
    int _delay;
};

Debounce debounceBtnOn(BTN_ON);
Debounce debounceBtnOff(BTN_OFF);

// wird einmalig beim Start des Arduinos ausgeführt
void setup() {
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  Serial.begin(9600);
  testLeds();
  changeStateTo(STATE_OFF);
}

// Schaltet alle LEDs nacheinander an
void testLeds() {
  digitalWrite(LED_R, HIGH);
  delay(1000);
  digitalWrite(LED_Y, HIGH);
  delay(1000);
  digitalWrite(LED_G, HIGH);
  delay(1000);
}

// wechselt zu neuen Zustand
void changeStateTo(char state_new) {
  state_previous = state_current;
  state_current = state_new;
  transition();
}

// behandelt die Zustandübergänge
bool transition() {
  if (state_previous == STATE_OFF && state_current == STATE_ON) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    stateBegan = millis();
    return true;
  }
  if (state_previous == STATE_ON && state_current == STATE_ON) { // STATE_ON ist reflexiv
    stateBegan = millis();
    return true;
  }
  if (state_previous == STATE_ON && state_current == STATE_HALF) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, HIGH);
    return true;
  }
  if (state_previous == STATE_ON && state_current == STATE_OFF) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_R, HIGH);
    return true;
  }
  if (state_previous == STATE_HALF && state_current == STATE_OFF) {
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, HIGH);
    return true;
  }
  if (state_previous == STATE_HALF && state_current == STATE_ON) {
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_G, HIGH);
    stateBegan = millis();
    return true;
  }
  if (state_previous == NULL && state_current == STATE_OFF) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, HIGH);
    return true;
  }
  return false;
}

// information über aktuellen Zustand auf die Serielle Verbindung schreiben
void sendState() {
  if (state_current == STATE_ON || state_current == STATE_HALF) {
      Serial.print("1");
  } else {
      Serial.print("0");
  }
}

unsigned long calcStateTime() {
  // Variablen überlauf von millis erkennen
  unsigned long current_uptime = millis();
  // kein überlauf
  if (current_uptime > stateBegan) {
    return current_uptime - stateBegan;
  }
  return current_uptime + (ULONG_MAX - stateBegan);
}

// wird nach dem Starten dauerhaft ausgeführt
void loop() {  
  // Einschalter auslesen
  if (debounceBtnOn.update() && debounceBtnOn.read()) {
    changeStateTo(STATE_ON);
  }
  // Ausschalter auslesen
  if (debounceBtnOff.update() && debounceBtnOff.read()) {
    changeStateTo(STATE_OFF);
  }

  // Auswertung des aktuellen Zustandes
  // ggf Zustand wechseln
  if (state_current == STATE_ON) {
    if (calcStateTime() >= TIME_HALF) {
      changeStateTo(STATE_HALF);
    }
  } else if (state_current == STATE_HALF && calcStateTime() >= TIME_OFF) {
    changeStateTo(STATE_OFF);
  }

  // kommunizieren
  sendState();
  delay(10);
}

// Debouncer Klasse
Debounce::Debounce(int pin)
{
  pinMode(pin, INPUT);
  this->_pin = pin;
  this->_time = 0;
  this->_state = LOW;
  this->_delay = 50;
}
bool Debounce::update()
{
  if (millis() - this->_time >= this->_delay) {
    int reading = digitalRead(this->_pin);
    if (reading != this->_state) {
      this->_time = millis();
      this->_state = reading;
      return true;
    }
  }
  return false;
}
int Debounce::read()
{
  return this->_state;
}
