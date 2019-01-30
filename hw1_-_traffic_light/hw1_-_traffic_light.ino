#define NS_PED_RED 22
#define NS_PED_GRN 23
#define NS_TRAF_RED 30
#define NS_TRAF_YEL 31
#define NS_TRAF_GRN 32

#define EW_PED_RED 50
#define EW_PED_GRN 51
#define EW_TRAF_RED 40
#define EW_TRAF_YEL 41
#define EW_TRAF_GRN 42

#define NS_BTN 3
#define EW_BTN 2

const int traf_lights[] = {NS_TRAF_RED, NS_TRAF_YEL, NS_TRAF_GRN, EW_TRAF_RED, EW_TRAF_YEL, EW_TRAF_GRN};
const int ped_lights[] = {NS_PED_RED, NS_PED_GRN, EW_PED_RED, EW_PED_GRN};

#define NUM_TRAF_LIGHTS sizeof(traf_lights)/sizeof(const int)
#define NUM_PED_LIGHTS sizeof(ped_lights)/sizeof(const int)

enum trafLightState {
  NS_GREEN,NS_YELLOW,NS_RED,EW_GREEN,EW_YELLOW,EW_RED
};

enum pedLightState {
  OFF,NS,EW
};

unsigned long last_traf_change;
trafLightState traf_state;
bool traf_state_changed;

unsigned long last_ped_change;
pedLightState ped_state;
bool ped_state_changed;

unsigned long last_ns_push;
unsigned long last_ew_push;

void ewButton() {
  //todo debounce
  last_ew_push = millis();
  Serial.println("ew down");
  request_ped_transition(EW);
}

void nsButton() {
  //todo debounce
  last_ns_push = millis();
  Serial.println("ns down");
  request_ped_transition(NS);
}

void setup() {
  Serial.begin(9600);
  pinMode(EW_BTN, INPUT_PULLUP);
  pinMode(NS_BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(EW_BTN), ewButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(NS_BTN), nsButton, FALLING);

  for (int i = 0; i < NUM_TRAF_LIGHTS; i++) {
    pinMode(traf_lights[i], OUTPUT);
  }
  for (int i = 0; i < NUM_PED_LIGHTS; i++) {
    pinMode(ped_lights[i], OUTPUT);
  }
  reset_traf_lights();
  reset_ped_lights();

  // initial state
  transition_traf(NS_GREEN);
  transition_ped(OFF);

  update_lights();
}

void loop() {
  do_transition();
  update_lights();
}

void update_lights() {
  if (traf_state_changed) {
    // todo don't reset everything on transitions
    reset_traf_lights();
  
    Serial.println("traf light state: " + String(traf_state));
    
    switch(traf_state) {
      case NS_GREEN:
        digitalWrite(NS_TRAF_GRN, HIGH);
        digitalWrite(EW_TRAF_RED, HIGH);
        break;
      case NS_YELLOW:
        digitalWrite(NS_TRAF_YEL, HIGH);
        digitalWrite(EW_TRAF_RED, HIGH);
        break;      
      case NS_RED:
        digitalWrite(NS_TRAF_RED, HIGH);
        digitalWrite(EW_TRAF_RED, HIGH);
        break;
      case EW_GREEN:
        digitalWrite(NS_TRAF_RED, HIGH);
        digitalWrite(EW_TRAF_GRN, HIGH);
        break;
      case EW_YELLOW:
        digitalWrite(NS_TRAF_RED, HIGH);
        digitalWrite(EW_TRAF_YEL, HIGH);
        break;
      case EW_RED:
        digitalWrite(NS_TRAF_RED, HIGH);
        digitalWrite(EW_TRAF_RED, HIGH);
        break;
    }
    traf_state_changed = false;
  }

  if (ped_state_changed) {
    // todo don't reset everything on transitions
    reset_ped_lights();

    Serial.println("ped light state: " + String(ped_state));

    switch(ped_state) {
      case OFF:
        digitalWrite(NS_PED_RED, HIGH);
        digitalWrite(EW_PED_RED, HIGH);
        break;
      case NS:
        digitalWrite(NS_PED_GRN, HIGH);
        digitalWrite(EW_PED_RED, HIGH);
        break;
      case EW:
        digitalWrite(NS_PED_RED, HIGH);
        digitalWrite(EW_PED_GRN, HIGH);
        break;
    }
    ped_state_changed = false;
  }
}

void request_ped_transition(pedLightState new_state) {
  Serial.println("requesting crossing. ped: " + String(new_state) + ", traf_state: " + String(traf_state));

  if (new_state == NS && traf_state == NS_GREEN) {
    Serial.println("turning ns ped on");
    transition_ped(new_state);
    return;
  }
  if (new_state == EW && traf_state == EW_GREEN) {
    Serial.println("turning ew ped on");
    transition_ped(new_state);
    return;
  }
}

void transition_traf(trafLightState new_state) {
  traf_state = new_state;
  traf_state_changed = true;
  last_traf_change = millis();
  
  Serial.println("New traf state: " + String(new_state) + " at " + last_traf_change);
}

void transition_ped(pedLightState new_state) {
  ped_state = new_state;
  ped_state_changed = true;
  last_ped_change = millis();

  Serial.println("New ped state: " + String(new_state) + " at " + last_ped_change);
}

void do_transition() {
  switch(traf_state){
    case NS_GREEN:
      if (millis() - last_traf_change > 10000) {
        transition_traf(NS_YELLOW);
        transition_ped(OFF);
      }
      break;
    case NS_YELLOW:
      if (millis() - last_traf_change > 1000) {
        transition_traf(NS_RED);
        transition_ped(OFF);
      }
      break;
    case NS_RED:
      if (millis() - last_traf_change > 1000) {
        transition_traf(EW_GREEN);
        //transition_ped(NS);
        transition_ped(OFF);
      }
      break;
    case EW_GREEN:
      if (millis() - last_traf_change > 10000) {
        transition_traf(EW_YELLOW);
        transition_ped(OFF);
      }
      break;
    case EW_YELLOW:
      if (millis() - last_traf_change > 1000) {
        transition_traf(EW_RED);
        transition_ped(OFF);
      }
      break;
    case EW_RED:
      if (millis() - last_traf_change > 1000) {
        transition_traf(NS_GREEN);
        //transition_ped(EW);
        transition_ped(OFF);
      }
      break;
  }
}

void reset_traf_lights() {
  for (int i = 0; i < NUM_TRAF_LIGHTS; i++) {
    digitalWrite(traf_lights[i], LOW);
  }
}

void reset_ped_lights() {
  for (int i = 0; i < NUM_PED_LIGHTS; i++) {
    digitalWrite(ped_lights[i], LOW);
  }
}
