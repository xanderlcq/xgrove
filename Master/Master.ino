#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SPI.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
float b1_draining_time = 0;
float b2_draining_time = 0;
float b3_draining_time = 0;
Adafruit_BME280 bme; // I2C
boolean debug = false;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Boolean swtiches
boolean bed1_cycling = true;
boolean bed2_cycling = true;
boolean bed3_cycling = true;
boolean bed1_light_on = false;
boolean bed2_light_on = false;
boolean bed3_light_on = false;


//Thresholds for water levels

int bed_state = 1;
float bed1_dry = 2100;
float bed2_dry = 1600;
float bed3_dry = 1300;//R is high when water level is low
float bed1_full = 1000;
float bed2_full = 840;
float bed3_full = 670; //R is low when water level is high
boolean b1_filling = true;
boolean b2_filling = true;
boolean b3_filling = true;
//Water level sensors
#define Water_Lv_1 A0
#define Water_Lv_2 A1
#define Water_Lv_3 A2
float SERIESRESISTOR = 560.0;
#define aref_voltage 5.0
//Out put
//Lights
int light1 = 5;
int light2 = 6;
int light3 = 7;
//Pumps
int pump1 = 2;
int pump2 = 3;
int pump3 = 4;
//Valves
int valve1 = 35;
int valve2 = 36;
int valve3 = 37;
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
  Serial.begin(250000);
  // Start up the library
  //water_temp_sensors.begin();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  //analogReference(EXTERNAL);
  halt_cycle();

  // init_light_sensor();
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  if (!bme.begin()) {
    //Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  }
  Serial.println("ready");
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
    int command = inputString.substring(0, 2).toInt();
    execute_command(command);
    //if (inputString == "get_all\n") {
    //Serial.println(get_all_sensors());
    //}
    inputString = "";
    stringComplete = false;
  }


  //always checking bed
  refresh_light();
  cycle_water();
  delay(1);
}

//====Communication==========
String get_all_sensors() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& dat = jsonBuffer.createObject();
  dat["water_temp"] = get_water_temp();
  dat["analog_temp"] = get_analog_temp();
  dat["light_intensity"] = get_light_sensor();
  dat["air_pressure"] = bme.readPressure() / 100.0F;
  dat["digital_temp"] = bme.readTemperature();
  dat["humidity"] = bme.readHumidity();

  dat["bed_cycling"] = bed_state;
  if (bed_state == 1) dat["bed_filling"] = b1_filling;
  if (bed_state == 2) dat["bed_filling"] = b2_filling;
  if (bed_state == 3) dat["bed_filling"] = b3_filling;

  char buffer[256];
  dat.printTo(buffer, sizeof(buffer));
  return buffer;
}

void execute_command(int input) {

  switch (input) {
    case 00:
      debug = true;
      break;
    case 01:
      debug = false;
      break;
    case 02:
      Serial.println(get_next_bed(-1));
      Serial.println(get_next_bed(1));
      Serial.println(get_next_bed(2));
      Serial.println(get_next_bed(3));
      Serial.println("===");
      break;
    case 11:
      Serial.println(get_all_sensors());
      break;
    case 12:
      //switch to real time control
      break;
    case 13:
      //switch to auto
      break;
    case 21:
      bed1_light_on = true;
      break;
    case 22:
      bed1_light_on = false;
      break;
    case 23:
      bed2_light_on = true;
      break;
    case 24:
      bed2_light_on = false;
      break;
    case 25:
      bed3_light_on = true;
      break;
    case 26:
      bed3_light_on = false;
      break;
    case 31:
      bed1_cycling = true;
      break;
    case 32:
      bed1_cycling = false;
      break;
    case 33:
      bed2_cycling = true;
      break;
    case 34:
      bed2_cycling = false;
      break;
    case 35:
      bed3_cycling = true;
      break;
    case 36:
      bed3_cycling = false;
      break;
  }
}
//=============Controls============================
//=============Water level control=================
void cycle_water() {
  if (debug)print_all_beds();
  Serial.print(b1_filling);
  Serial.print(b2_filling);
  Serial.print(b3_filling);
  Serial.print("||");
  Serial.print(bed1_cycling);
  Serial.print(bed2_cycling);
  Serial.print(bed3_cycling);
  Serial.print("||");
  Serial.println(bed_state);
  if (bed_state == -1) bed_state = get_next_bed(1);
  if (bed_state == 1) {
    if (bed1_cycling) {
      if (b1_filling && is_full(1)) {
        b1_filling = false;
        reset_drain_timer(1);
      }
      if ((!b1_filling) && is_empty(1)) {
        b1_filling = true;
        bed_state = get_next_bed(1);
      }
    } else {
      bed_state = get_next_bed(1);
    }
  } else if (bed_state == 2) {
    if (bed2_cycling) {
      if (b2_filling && is_full(2)) {
        Serial.println("!!!!!!");
        b2_filling = false;
        reset_drain_timer(2);
      }
      if ((!b2_filling) && is_empty(2)) {
        b2_filling = true;
        bed_state = get_next_bed(2);
      }
    } else {
      bed_state = get_next_bed(2);
    }
  } else if (bed_state == 3) {
    if (bed3_cycling) {
      if (b3_filling && is_full(3)) {
        b3_filling = false;
        reset_drain_timer(3);
      }
      if ((!b3_filling) && is_empty(3)) {
        b3_filling = true;
        bed_state = get_next_bed(3);
      }
    } else {
      bed_state = get_next_bed(3);
    }
  }
  refresh_bed();


}
boolean is_full(int bed) {
  float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  if (bed == 1) return bed1 < bed1_full;
  if (bed == 2) return bed2 < bed2_full;
  if (bed == 3) return bed3 < bed3_full;
}
boolean is_empty(int bed) {
  float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  if (bed == 1) return (bed1 > bed1_dry || millis() - b1_draining_time > 120000/2);
  if (bed == 2) return (bed2 > bed2_dry || millis() - b2_draining_time > 90000/2);
  if (bed == 3) return (bed3 > bed3_dry || millis() - b3_draining_time > 90000/2);
}
void print_all_beds() {
  float bed1 = (aref_voltage / ((analogRead(Water_Lv_1) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed2 = (aref_voltage / ((analogRead(Water_Lv_2) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  float bed3 = (aref_voltage / ((analogRead(Water_Lv_3) * aref_voltage) / 1024.0) - 1) * SERIESRESISTOR;
  Serial.print("Cyc: Bed1: " + String(bed1) + ";" + String(millis() - b1_draining_time));
  Serial.print("; Bed2: " + String(bed2) + ";" + String(millis() - b2_draining_time));
  Serial.println("; Bed3: " + String(bed3) + ";" + String(millis() - b3_draining_time));
}

void reset_drain_timer(int bed) {
  if (bed == 1) b1_draining_time = millis();
  if (bed == 2) b2_draining_time = millis();
  if (bed == 3) b3_draining_time = millis();
}
int get_next_bed(int current) {
  if (!bed1_cycling && !bed2_cycling && !bed3_cycling) return -1;
  int beds[] = {bed1_cycling, bed2_cycling, bed3_cycling};
  if (current == 1) {
    if (bed2_cycling == true) return 2;
    else if (bed3_cycling == true) return 3;
    else return 1;
  }
  if (current == 2) {
    if (bed3_cycling == true) return 3;
    else if (bed1_cycling == true) return 1;
    else return 2;
  }
  if (current == 3) {
    if (bed1_cycling == true) return 1;
    else if (bed2_cycling == true) return 2;
    else return 3;
  }
  if (current == -1) {
    if (bed1_cycling == true) return 1;
    else if (bed2_cycling == true) return 2;
    else return 3;
  }
  /*
    int i = current;
    while (true) {
    if (i >= sizeof(beds)) i = 0;
    if (beds[i] == 1) {
      i++;
      return i;
    }
    i++;
    }*/
}

void refresh_bed() {
  if (b1_filling && bed_state == 1) {
    fill_bed(1);
  } else {
    drain_bed(1);
  }
  if (b2_filling && bed_state == 2) {
    fill_bed(2);
  } else {
    drain_bed(2);
  }
  if (b3_filling && bed_state == 3) {
    fill_bed(3);
  } else {
    drain_bed(3);
  }
}
void refresh_light() {
  if (bed1_light_on) {
    digitalWrite(light1, LOW);
  } else {
    digitalWrite(light1, HIGH);
  }
  if (bed2_light_on) {
    digitalWrite(light2, LOW);
  } else {
    digitalWrite(light2, HIGH);
  }
  if (bed3_light_on) {
    digitalWrite(light3, LOW);
  } else {
    digitalWrite(light3, HIGH);
  }


}
void halt_cycle() {
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
void drain_bed(int bed) {
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
void fill_bed(int bed) {
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
void display_light_sensor_details() {
  sensor_t sensor;
  tsl.getSensor(&sensor);/*
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");*/
  delay(50);
}

void configure_light_sensor() {
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  /*Serial.println("------------------------------------");
    Serial.print  ("Gain:         "); Serial.println("Auto");
    Serial.print  ("Timing:       "); Serial.println("13 ms");
    Serial.println("------------------------------------");*/
}

void init_light_sensor() {
  /* Initialise the sensor */
  if (!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  /* Display some basic information on this sensor */
  display_light_sensor_details();

  /* Setup the sensor gain and integration time */
  configure_light_sensor();

  /* We're ready to go! */

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
