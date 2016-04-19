/* Sensor test sketch
  for more information see http://www.ladyada.net/make/logshield/lighttemp.html
  */
 
#define aref_voltage 5.0         // we tie 3.3V to ARef and measure it with a multimeter!
 
 
 
 
//TMP36 Pin Variables
int tempPin = A3;        //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
                        // the analog reading from the sensor
 
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
 
  // If you want to set the aref to something other than 5v
  //analogReference(EXTERNAL);
}
float get_analog_temp(){
  int tempReading = analogRead(tempPin);  
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * aref_voltage;
  voltage /= 1024.0; 
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((volatge - 500mV) times 100)
  return temperatureC;
}
 
void loop(void) {
Serial.println(get_analog_temp());
delay(50);
}
