// First version
// Grapefruit Tree auto watering with data recording
#include <Servo.h>
Servo pipe;
int pos1 = 70;
int pos2 = 110;
//Plant's Name
const String PLANT_NAME = "grapefruit";
// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN 2
// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
//valve control, this pin control water valve
#define VALVEPIN 3
//moistrue sensor pin
#define MOISTUREPIN A0
#define MOISTUREPIN2 A1
#define MINUTE 60000
//Desired moisture level
int std_moisture = 300;
//Global data proccessing variables
String global_keys[] = {"moisture", "addWater"} ;
String global_values[] = {"", ""};
bool tank_is_full = true;

void setup() {
  Serial.begin(9600);
  pinMode(VALVEPIN, OUTPUT);
  //initialize flowmeter
  pinMode(FLOWSENSORPIN, INPUT);
  pipe.attach(6);
  digitalWrite(FLOWSENSORPIN, HIGH);
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
  useInterrupt(true);
  digitalWrite(VALVEPIN, LOW);
  delay(5000);
}

void loop() {

  auto_water(0.1);
  for(int i = 0;i<30;i++){
  delay(MINUTE);
  }



}





//============================Functions==========================
//watering for grapefruit tree
void auto_water(float volume) {
  String keys[] = {"moisture", "addWater"} ;
  String data[] = {"0", "0"};
  int moisture = analogRead(MOISTUREPIN);
  data[0] = String(moisture);
  if (moisture <  std_moisture) {
    if (tank_is_full) {
      if (add_water(volume)) {
        data[1] = String(volume);
      }
    }
  }
  String temp = json_data(keys, data);
  Serial.println(PLANT_NAME + temp);
  
  data[1] = String(0);
}

//clear global_values data
void clear_data() {
  for (int i = 0; i < sizeof(global_values); i++) {
    global_values[i] = "";
  }
}

//turn pipe servo
void turnPipe(int pos) {
  if (pos == 1) {
    pipe.write(pos1);
  }
  if (pos == 2) {
    pipe.write(pos2);
  }
}

//add a certain amount of water
void light_on(int plug) {
  digitalWrite(plug, HIGH);
}
void light_off(int plug) {
  digitalWrite(plug, LOW);
}

bool add_water(float liter) {
  liter *= 0.75;
  reset_flowmeter();
  int count = 0;
  while (get_flowmeter() < liter) {
    digitalWrite(VALVEPIN, HIGH);
    delay(100);
    count++;
    if (count > 120) {
      Serial.println(PLANT_NAME+"{\"error\":\"water tank empty\"}");
      tank_is_full = false;
      digitalWrite(VALVEPIN, LOW);
      return false;
    }
  }
  digitalWrite(VALVEPIN, LOW);
  reset_flowmeter();
  return true;
}
//update data
void update_data(String key, String value) {
  for (int i = 0; i < sizeof(global_keys); i++) {
    if (global_keys[i] == key) {
      global_values[i] = value;
    }
  }
}

//encoding json
String json_data(String keys[], String values[]) {
  //{"key1":1.0,"key2":"str","key3":1}
  String json = "{\"";
  for (int i = 0; i < sizeof(keys); i++) {
    if (i != sizeof(keys) - 1) {
      if (values[i] != "") {
        json += (keys[i] + "\":" + values[i] + ",\"");
      } else {
        json += (keys[i] + "\":" + "0" + ",\"");
      }
    } else {
      if (values[i] != "") {
        json += (keys[i] + "\":" + values[i] + "}");
      } else {
        json += (keys[i] + "\":" + "0" + "}");
      }
    }
  }
  return json;
}
//read the flowmeter value
float get_flowmeter() {
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
  return liters;
}

//reset the flowmeter value
void reset_flowmeter() {
  pulses = 0;
}

// Interrupt is called once a millisecond, looks for any pulses from the sensor!
SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);

  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }

  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

