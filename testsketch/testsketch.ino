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

const int all_lights[] = {NS_PED_RED, NS_PED_GRN, NS_TRAF_RED, NS_TRAF_YEL, 
                          NS_TRAF_GRN, EW_PED_RED, EW_PED_GRN, EW_TRAF_RED, 
                          EW_TRAF_YEL, EW_TRAF_GRN};

int num_lights() {
  return sizeof(all_lights)/sizeof(int);
}

void reset_all() {
  for (int i = 0; i < num_lights(); i++) {
    digitalWrite(all_lights[i], LOW);
  }
}

void setup() {
  for (int i = 0; i < num_lights(); i++) {
    pinMode(all_lights[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < num_lights(); i++) {
    reset_all();
    digitalWrite(all_lights[i], HIGH);
    delay(1000);
  }
}
