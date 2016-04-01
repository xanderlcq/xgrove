// MODIFIED FROM Library example, not original
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 10 on the Arduino
#define ONE_WIRE_BUS 10
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature water_temp_sensors(&oneWire);
#define aref_voltage 5.0

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Start up the library
  water_temp_sensors.begin();
}

void loop(void)
{
  Serial.println(getWaterTemp());
  delay(200);
  /*
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    water_temp_sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(water_temp_sensors.getTempCByIndex(0));
  */
}
float getWaterTemp() {
  water_temp_sensors.requestTemperatures();// Send the command to get temperatures
  return water_temp_sensors.getTempCByIndex(0);
}

