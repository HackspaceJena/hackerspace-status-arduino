/*
 * es gibt folgende Zustände:
 * 0 - Aus
 * 1 - An, aber auf dem weg zu aus
 * 2 - An
 */
#define STATE_OFF 3
#define STATE_HALF 1
#define STATE_ON 2

/*
 * Zeit wie lange in einem Zustände verharrt werden soll
 * bis zum nächsten umgeschaltet wird in Millisekunden.
 * TIME_HALF - Zeitspanne von Zustand 2 bis Wechsel zu Zustand 1
 * TIME_OFF  - Zeitspanne von Zustand 2 bis Wechsel zu Zustand 0
 */
#define TIME_HALF 5400000 // 1,5h
#define TIME_OFF 7200000 // 2h

// für Variablen Überlauf in calcStateTime
#define MAX_LONG 4294967295

// Ein-/Ausgänge Bezeichnen
const int BTN_ON = 2;  // Einschalter
const int BTN_OFF = 3; // Ausschalter
const int LED_G = 9;   // grüne LED
const int LED_Y = 8;   // gelbe LED
const int LED_R = 7;   // rote LED

// hier wird der aktuelle und vorherige Zustand gespeichert
byte state_current = NULL;
byte state_previous = NULL;

// hier wird der Beginn des aktuellen Zustand gespeichert in Millisekunden nach Uptime.
unsigned long stateBegan;

// Debouncer
class Debounce
{
  public:
    Debounce(int pin);
    boolean update();
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
void changeStateTo(byte state_new) {
  state_previous = state_current;
  state_current = state_new;
  transition();
}

// behandelt die Zustandübergänge
boolean transition() {
  if (state_previous == STATE_OFF && state_current == STATE_ON) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    Serial.println("ON");
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
    Serial.println("HALF");
    return true;
  }
  if (state_previous == STATE_ON && state_current == STATE_OFF) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_R, HIGH);
    Serial.println("OFF");
    return true;
  }
  if (state_previous == STATE_HALF && state_current == STATE_OFF) {
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, HIGH);
    Serial.println("OFF");
    return true;
  }
  if (state_previous == NULL && state_current == STATE_OFF) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, HIGH);
    Serial.println("OFF");
    return true;
  }
  return false;
}


unsigned long calcStateTime() {
  // Variablen überlauf von millis erkennen
  if (millis() - stateBegan >= 0) {
    return millis() - stateBegan;
  } else {
    return millis() + (MAX_LONG - stateBegan);
  }
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
boolean Debounce::update()
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
