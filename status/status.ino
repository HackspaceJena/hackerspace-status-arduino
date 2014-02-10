/*
 * es gibt folgende Zustände:
 * 0 - Aus
 * 1 - An, aber auf dem weg zu aus
 * 2 - An
 */
#define STATE_OFF 0
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

// hier wird der aktuelle Zustand gespeichert
byte state = STATE_OFF;

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
  setStateOnLeds();
}

// bildet den aktuellen Zustand auf die LEDs ab
void setStateOnLeds() {
  digitalWrite(LED_R, (state == STATE_OFF));
  digitalWrite(LED_Y, (state == STATE_HALF));
  digitalWrite(LED_G, (state == STATE_ON));
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
    state = STATE_ON;
    stateBegan = millis();
    setStateOnLeds();
  }
  // Ausschalter auslesen
  if (debounceBtnOff.update() && debounceBtnOff.read()) {
    state = STATE_OFF;
    setStateOnLeds();
  }

  // Auswertung des aktuellen Zustandes
  // ggf Zustand wechseln
  if (state == STATE_ON) {
    if (calcStateTime() >= TIME_HALF) {
      state = STATE_HALF;
      setStateOnLeds();
    }
  } else if (state == STATE_HALF && calcStateTime() >= TIME_OFF) {
    state = STATE_OFF;
    setStateOnLeds();
  }

  // aktuellen Zustand auf die Serielle Verbindung schreiben
  Serial.print((state == STATE_ON || state == STATE_HALF));
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
