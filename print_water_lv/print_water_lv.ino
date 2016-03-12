// Print all water level
//Thresholds
float bed1_dry = 1500;
float bed2_dry = 1500;
float bed3_dry = 2200;//R is high when water level is low
float bed1_full = 400;
float bed2_full = 400;
float bed3_full = 400;//R is low when water level is high
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

}

void loop() {
  float bed1 = (5.0 / ((analogRead(Water_Lv_1) * 5.0) / 1024.0) - 1) * 560;
  float bed2 = (5.0 / ((analogRead(Water_Lv_2) * 5.0) / 1024.0) - 1) * 560;
  float bed3 = (5.0 / ((analogRead(Water_Lv_3) * 5.0) / 1024.0) - 1) * 560;
  //Serial.print(analogRead(A0));
  Serial.print("Bed1: " + String(bed1));
  Serial.print("; Bed2: " + String(bed2));
  Serial.println("; Bed3: " + String(bed3));

}
