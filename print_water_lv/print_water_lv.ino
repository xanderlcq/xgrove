// Print all water level
#define aref_voltage 5.0
//Thresholds for water levels
float bed1_dry = 1400;
float bed2_dry = 1630;
float bed3_dry = 1630;//R is high when water level is low
float bed1_full = 900;
float bed2_full = 800;
float bed3_full = 650;//R is low when water level is high
boolean b1_filling = false;
boolean b2_filling = false;
boolean b3_filling = false;
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
 drain(1);
drain(2);
fill(3);

}

void loop() {
  float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  //Serial.print(analogRead(A0));
  Serial.print("Cyc: Bed1: " + String(bed1));
  Serial.print("; Bed2: " + String(bed2));
  Serial.println("; Bed3: " + String(bed3));


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

