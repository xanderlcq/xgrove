#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//Thresholds for water levels
float bed1_dry = 2300;
float bed2_dry = 1530;
float bed3_dry = 1300;//R is high when water level is low
float bed1_full = 950;
float bed2_full = 750;
float bed3_full = 720; //R is low when water level is high
boolean b1_filling = false;
boolean b2_filling = false;
boolean b3_filling = false;
//Water level sensors
#define Water_Lv_1 A0
#define Water_Lv_2 A1
#define Water_Lv_3 A2
float SERIESRESISTOR = 560.0;
#define aref_voltage 5.0
//Out put
//Pumps
int pump1 = 2;
int pump2 = 3;
int pump3 = 4;
//Valves
int valve1 = 5;
int valve2 = 6;
int valve3 = 7;
//Sensros
#define tempPin A3
//Lux/light sensor
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
// Data wire is plugged into port 10 on the Arduino
#define ONE_WIRE_BUS 10
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature water_temp_sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  // Start up the library
  water_temp_sensors.begin();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  //analogReference(EXTERNAL);
  halt();
  //initialize_water();
  init_light_sensor();
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
void loop() {
  // When commands come in hot
  if (stringComplete) {
    //Serial.println(inputString);
    if (inputString == "get_all") {
      Serial.println(getAllSensors());
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  //Always checking
  checkBedsLv();
  //Serial.println(get_light_sensor());
  //delay(5);
}
//analogTemp,digitalTemp,waterTemp,moisture,humidity,ph,mass,sound,airPressure,
//====Communication==========
String getAllSensors() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& dat = jsonBuffer.createObject();
  dat["waterTemp"] = get_water_temp();
  dat["analogTemp"] = get_analog_temp();
  dat["lightIntensity"] = get_light_sensor();
  dat["devices"] = "name";
  char buffer[256];
  dat.printTo(buffer, sizeof(buffer));
  return buffer;
}
//=============Controls============================
//=============Water level control=================
int b1draining = 0;
int b2draining = 0;
int b3draining = 0;
void checkBedsLv() {
  float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  //Serial.print(analogRead(A0));
  Serial.print("Cyc: Bed1: " + String(bed1)+";"+String(b1draining));
  Serial.print("; Bed2: " + String(bed2)+";"+String(b2draining));
  Serial.println("; Bed3: " + String(bed3)+";"+String(b3draining));

  if (bed1 < bed1_full) {
    b1_filling = false;
    b1draining = 0;
  }
  if (bed2 < bed2_full) {
    b2_filling = false;
    b2draining = 0;
  }
  if (bed3 < bed3_full) {
    b3_filling = false;
    b3draining = 0;
  }
  if (bed1 > bed1_dry) {
    b1_filling = true;
  }
  if (bed2 > bed2_dry) {
    b2_filling = true;
  }
  if (bed3 > bed3_dry) {
    b3_filling = true;
  }
  if(b1draining > 2500){
    b1_filling = true;
    b1draining = 0;
  }
  if(b2draining > 1000){
    b2_filling = true;
    b2draining = 0;
  }
  if(b3draining > 1000){
    b3_filling = true;
    b3draining=0;
  }
  
  if (b1_filling) {
    fill(1);
  } else {
    b1draining++;
    drain(1);
  }
  if (b2_filling) {
    
    fill(2);
  } else {
    b2draining++;
    drain(2);
  }
  if (b3_filling) {
    fill(3);
  } else {
    b3draining++;
    drain(3);
  }
  
}
void initialize_water() {
  drain(1);
  fill(2);
  fill(3);
  Serial.println("initializing");
  boolean b2 = false;
  boolean b3 = false;
  while (true) {
    float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
    float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
    float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
    Serial.print("Bed1: " + String(bed1));
    Serial.print("; Bed2: " + String(bed2));
    Serial.println("; Bed3: " + String(bed3));
    if (bed2 < (bed2_full * 1.3)) {
      pause_cycle(2);
      b2 = true;
    }
    if (bed3 < bed3_full) {
      pause_cycle(3);
      b3 = true;
    }
    if (b2 && b3 && (bed1 > bed1_dry * 0.9)) {
      halt();
      return;
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
//================For sensors===============
void displayLightSensorDetails(void) {
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void configureLightSensor(void) {
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void init_light_sensor(void) {
  /* Initialise the sensor */
  if (!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  /* Display some basic information on this sensor */
  displayLightSensorDetails();

  /* Setup the sensor gain and integration time */
  configureLightSensor();

  /* We're ready to go! */
  Serial.println("");

}

float get_light_sensor() {
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    float val = event.light;
    //Serial.print(val); Serial.println(" lux");
    return val;
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    //Serial.println("Sensor overload");
    return -1;
  }
}

float get_analog_temp() {
  int tempReading = analogRead(tempPin);
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * aref_voltage;
  voltage /= 1024.0;
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)
  return temperatureC;
}

float get_water_temp() {
  water_temp_sensors.requestTemperatures();// Send the command to get temperatures
  return water_temp_sensors.getTempCByIndex(0);
}
