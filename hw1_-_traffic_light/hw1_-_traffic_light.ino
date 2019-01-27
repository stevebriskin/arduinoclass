
#define RED_LIGHT 7
#define GREEN_LIGHT 8
#define CROSS_BTN 3

bool greenLight = true;
unsigned long lastChange;

void setup() {
  pinMode(CROSS_BTN, INPUT_PULLUP);
  pinMode(RED_LIGHT, OUTPUT);
  pinMode(GREEN_LIGHT, OUTPUT);

  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(GREEN_LIGHT, LOW);

  lastChange = millis();

  setLight();
}

void setLight() {
  if (greenLight) {
    digitalWrite(GREEN_LIGHT, HIGH);
    digitalWrite(RED_LIGHT, LOW);
  } else {
    digitalWrite(GREEN_LIGHT, LOW);
    digitalWrite(RED_LIGHT, HIGH);
  }
}

int readButton() {
  if (digitalRead(CROSS_BTN) == 0) {
    delay(10);
    return digitalRead(CROSS_BTN);
  }

  return 1;
}

void loop() {
  int btn = readButton();

  if (btn == 0 || millis() - lastChange > 3000) {
    greenLight = !greenLight;
    lastChange = millis();
  }
  setLight();
}
