// Grow bed auto water cycling
// Event driven, filling and drainging
//Thresholds for water levels
float bed1_dry = 1500;
float bed2_dry = 1500;
float bed3_dry = 2200;//R is high when water level is low
float bed1_full = 850;
float bed2_full = 800;
float bed3_full = 650;//R is low when water level is high
//Water level sensors
#define Water_Lv_1 A0
#define Water_Lv_2 A1
#define Water_Lv_3 A2
float SERIESRESISTOR = 560.0;
//Out put
//Pumps
int pump1 = 2;
int pump2 = 3;
int pump3 = 4;
//Valves
int valve1 = 5;
int valve2 = 6;
int valve3 = 7;
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  halt();
  initialize();
}
void loop() {
  checkBedsLv();
  delay(50);
}
void checkBedsLv() {
  float bed1 = (5.0 / ((analogRead(Water_Lv_1) * 5.0) / 1024.0) - 1) * 560;
  float bed2 = (5.0 / ((analogRead(Water_Lv_2) * 5.0) / 1024.0) - 1) * 560;
  float bed3 = (5.0 / ((analogRead(Water_Lv_3) * 5.0) / 1024.0) - 1) * 560;
  //Serial.print(analogRead(A0));
  Serial.print("Bed1: " + String(bed1));
  Serial.print("; Bed2: " + String(bed2));
  Serial.println("; Bed3: " + String(bed3));

  if (bed1 < bed1_full) {
    drain(1);
  }
  if (bed2 < bed2_full) {
    drain(2);
  }
  if (bed3 < bed3_full) {
    drain(3);
  }
  if (bed1 > bed1_dry) {
    fill(1);
  }
  if (bed2 > bed2_dry) {
    fill(2);
  }
  if (bed3 > bed3_dry) {
    fill(3);
  }
}
void initialize() {
  drain(1);
  fill(2);
  fill(3);
  Serial.println("initializing");
  while (true) {
    float bed1 = (5.0 / ((analogRead(Water_Lv_1) * 5.0) / 1024.0) - 1) * 560;
    float bed2 = (5.0 / ((analogRead(Water_Lv_2) * 5.0) / 1024.0) - 1) * 560;
    float bed3 = (5.0 / ((analogRead(Water_Lv_3) * 5.0) / 1024.0) - 1) * 560;
    //Serial.print(analogRead(A0));
    Serial.print("Bed1: " + String(bed1));
    Serial.print("; Bed2: " + String(bed2));
    Serial.println("; Bed3: " + String(bed3));
    if (bed2 < (bed2_full * 1.5)) {
      pause_cycle(2);
    }
    if (bed3 < bed3_full) {
      pause_cycle(3);
    }
    if ((bed2 < (bed2_full * 1.5)) && (bed3 < bed3_full)) {
      halt();
      break;
    }
    delay(50);
  }
}
void halt() {
  pause_cycle(1);
  pause_cycle(2);
  pause_cycle(3);

}
void pause_cycle(int bed) {
  if (bed == 1) {
    digitalWrite(valve1, HIGH); //close valve
    digitalWrite(pump1, HIGH); //turn off pump
    return;
  }
  if (bed == 2) {
    digitalWrite(valve2, HIGH); //close valve
    digitalWrite(pump2, HIGH); //turn off pump
    return;
  }
  if (bed == 3) {
    digitalWrite(valve3, HIGH); //close valve
    digitalWrite(pump3, HIGH); //turn off pump
    return;
  }
}

void drain(int bed) {
  if (bed == 1) {
    digitalWrite(valve1, LOW); //open valve
    digitalWrite(pump1, HIGH); //turn off pump
    return;
  }
  if (bed == 2) {
    digitalWrite(valve2, LOW); //open valve
    digitalWrite(pump2, HIGH); //turn off pump
    return;
  }
  if (bed == 3) {
    digitalWrite(valve3, LOW); //open valve
    digitalWrite(pump3, HIGH); //turn off pump
    return;
  }
}
void fill(int bed) {
  if (bed == 1) {
    digitalWrite(valve1, HIGH); //shut off valve
    digitalWrite(pump1, LOW); //turn on pump
    return;
  }
  if (bed == 2) {
    digitalWrite(valve2, HIGH); //shut off valve
    digitalWrite(pump2, LOW); //turn on pump
    return;
  }
  if (bed == 3) {
    digitalWrite(valve3, HIGH); //shut off valve
    digitalWrite(pump3, LOW); //turn on pump
    return;
  }
}

