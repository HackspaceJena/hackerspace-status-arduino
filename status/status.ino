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
#define INTERRUPT_NAME_BTN_ON 0
const int BTN_OFF = 3; // Ausschalter
#define INTERRUPT_NAME_BTN_OFF 1
const int LED_G = 9;   // grüne LED
const int LED_Y = 8;   // gelbe LED
const int LED_R = 7;   // rote LED

// hier wird der aktuelle Zustand gespeichert
volatile byte state = STATE_OFF;

// hier wird der Beginn des aktuellen Zustand gespeichert in Millisekunden nach Uptime.
volatile unsigned long stateBegan;

const long debouncing_time = 50; //Debouncing Time in Milliseconds
volatile unsigned long last_buttonOnPressed;
volatile unsigned long last_buttonOffPressed;

// wird einmalig beim Start des Arduinos ausgeführt
void setup() {
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(INTERRUPT_NAME_BTN_ON, buttonOnPressedDebounce, RISING);
  attachInterrupt(INTERRUPT_NAME_BTN_OFF, buttonOffPressedDebounce, RISING);
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
  setStateOnLeds();
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
  if (state == STATE_ON || state == STATE_HALF) {
      Serial.print("1");
  } else {
      Serial.print("0");
  }
  delay(10);
}

void buttonOnPressedDebounce() {
  if (millis() - last_buttonOnPressed >= debouncing_time) {
    buttonOnPressed();
    last_buttonOnPressed = millis();
  }
}

void buttonOnPressed() {
  state = STATE_ON;
  stateBegan = millis();
}

void buttonOffPressedDebounce() {
  if (millis() - last_buttonOffPressed >= debouncing_time) {
    buttonOffPressed();
    last_buttonOffPressed = millis();
  }
}

void buttonOffPressed() {
  state = STATE_OFF;
}
