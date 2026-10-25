const int PIN_AND_OUT = 5;
const int LED_PINS[3] = {4, 3, 2};

int activeLed = 0;

void setup() {
  pinMode(PIN_AND_OUT, INPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

void loop() {
  if (digitalRead(PIN_AND_OUT) == HIGH) {
    digitalWrite(LED_PINS[activeLed], LOW);
    activeLed = (activeLed + 1) % 3;
    digitalWrite(LED_PINS[activeLed], HIGH);
    delay(200);
  } else {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
}