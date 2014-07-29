// This #include statement was automatically added by the Spark IDE.
#include "SortedLookupTable.h"


// this is used for access to hardware timers
//#include "SparkIntervalTimer/SparkIntervalTimer.h"
// for now, we have to put everything in one directory for the spark-cli
// compile command to work
#include "SparkIntervalTimer.h"

// Heating controller as a state machine
#include "HeatingSM.h"
// Maintains Thermistor LUT
#include "Thermistor.h"
// Maintains Pump control state
#include "PumpProgram.h"
// some utility functions
#include "Utilities.h"

#define PUMP_PIN D3
#define PUMP_ON_VALUE HIGH
#define PUMP_OFF_VALUE LOW

#define VALVE_PIN D2
#define VALVE_BREW_VALUE HIGH
#define VALVE_NOBREW_VALUE LOW

#define HEATING_RELAY_PIN D0
#define HEATING_RELAY_ON_VALUE LOW
#define HEATING_RELAY_OFF_VALUE HIGH

#define HEATING_THYRISTOR_PIN D1
#define HEATING_THYRISTOR_ON_VALUE HIGH
#define HEATING_THYRISTOR_OFF_VALUE LOW

#define THERMISTOR_PIN_HEAD A0
#define THERMISTOR_PIN_AMBIENT A1

#define PUMP_START_DELAY 100; //100ms delay

// heat controller state machine
HeatingSM heater;
// hardware timers
IntervalTimer heatingTimer;
IntervalTimer pumpTimer;
IntervalTimer temperatureTimer;
// Thermistor LUT
Thermistor thermistor;
// the way spark works, we can only return data to the user via the Spark.variable command
double temperatureInCelsius_head = 0;
double temperatureInCelsius_ambient = 0;
// pump controller
PumpProgram<5> pump;

void setup() {
    Serial.begin(9600);
    //Register our Spark function here
    Spark.function("heatTarget", heatTarget);
    Spark.function("heatToggle", heatToggle);
    Spark.function("pumpControl", pumpCommand);

    // I would much rather this be a float, but they only offer DOUBLEs
    Spark.variable("headTemp", &temperatureInCelsius_head,DOUBLE);
    Spark.variable("ambientTemp", &temperatureInCelsius_ambient,DOUBLE);

    configurePins();
    // setup hardware timer which controls heating loop and pump loop
    setupTimers();

    Serial.println("Hello WiBean!");
}

void loop() {
}


void configurePins() {
    // Configure the pins to be outputs
   pinMode(PUMP_PIN, OUTPUT);
   pinMode(VALVE_PIN, OUTPUT);
   pinMode(HEATING_RELAY_PIN, OUTPUT);
   pinMode(HEATING_THYRISTOR_PIN, OUTPUT);
   pinMode(THERMISTOR_PIN_HEAD,INPUT);
   pinMode(THERMISTOR_PIN_AMBIENT, INPUT);

   // turn everything off
   digitalWrite(HEATING_RELAY_PIN, HEATING_RELAY_OFF_VALUE);
   digitalWrite(HEATING_THYRISTOR_PIN, HEATING_THYRISTOR_OFF_VALUE);
   digitalWrite(PUMP_PIN, PUMP_OFF_VALUE);
   digitalWrite(VALVE_PIN, VALVE_NOBREW_VALUE);
}
void setupTimers() {
    // called every 50ms
    heater.setCycleLengthInMilliseconds(50);
    heater.setGoalTemperature(95); // hard coded init for now
    heatingTimer.begin(heatingLoop, 100, hmSec);
    // called every 100ms
    pumpTimer.begin(pumpLoop, 200, hmSec);
}

void heatingLoop() {
    temperatureUpdate();
    heater.updateCurrentTemp(temperatureInCelsius_head);
    // check the relay
    if( heater.runRelay() ) {
        digitalWrite(HEATING_RELAY_PIN, HEATING_RELAY_ON_VALUE);
    }
    else {
        digitalWrite(HEATING_RELAY_PIN, HEATING_RELAY_OFF_VALUE);
    }
    // check the thyristor
    if( heater.runThyristor() ) {
        digitalWrite(HEATING_THYRISTOR_PIN, HEATING_THYRISTOR_ON_VALUE);
    }
    else {
        digitalWrite(HEATING_THYRISTOR_PIN, HEATING_THYRISTOR_OFF_VALUE);
    }

}

void pumpLoop() {
  if( pump.isPumpingAt(millis()) ) {
    digitalWrite(PUMP_PIN, PUMP_ON_VALUE);
  }
  else {
    digitalWrite(PUMP_PIN, PUMP_OFF_VALUE);
  }
  // only open the valve once at the beginning and close once at the end
  if( pump.isValveOpenAt(millis()) ) {
    digitalWrite(VALVE_PIN, VALVE_BREW_VALUE);
  }
  else {
    digitalWrite(VALVE_PIN, VALVE_NOBREW_VALUE);
  }
}

void temperatureUpdate() {
  delay(3);
  temperatureInCelsius_head = thermistor.getTemperature( analogRead(THERMISTOR_PIN_HEAD) );
  delay(3);
  temperatureInCelsius_ambient = thermistor.getTemperature( analogRead(THERMISTOR_PIN_AMBIENT) );
}

int heatTarget(String command) {
  int targetAsInt = command.toInt();
  if( heater.setGoalTemperature(targetAsInt) ) {
    return 1;
  }
  else {
    return -1;
  }
}

int heatToggle(String command) {
  heater.enableHeating( command.charAt(0) == '1' );
  return 1;
}

int pumpCommand(String command) {
  // pump commands come in CSV, e.g. 32,44,33,22,0,0,0,33
  // as onFor,offFor,onFor,offFor.... up to the max limit set by PumpControl
  // and they come in with units of 100ms per unit.
  Serial.println("pumpCommand!");
  // did they send any parameters?
  if(command.length() == 0) {
    return -1;
  }
  int curChar = 0;
  uint16_t numLoop = 0;
  decltype(pump)::PUMP_TIME_TYPE onForMillis[pump.PUMP_STEPS];
  decltype(pump)::PUMP_TIME_TYPE offForMillis[pump.PUMP_STEPS];
  memset(onForMillis, 0, sizeof(int)*pump.PUMP_STEPS);
  memset(offForMillis, 0, sizeof(int)*pump.PUMP_STEPS);

  while( (numLoop < pump.PUMP_STEPS) ) {
    int value;
    curChar = takeNext(command, curChar, value) + 1;
#ifdef SERIAL_DEBUG
    Serial.print("curChar: ");
    Serial.println(curChar);
#endif
    if( curChar == 0 ) { break; }
    onForMillis[numLoop] = value*100; // values come as 100ms ticks, make ours millis
    // 0 because we added one
    curChar = takeNext(command, curChar, value) + 1;
#ifdef SERIAL_DEBUG
    Serial.print("curChar: ");
    Serial.println(curChar);
#endif
    if( curChar == 0 ) { break; }
    offForMillis[numLoop] = value*100; // values come as 100ms ticks, make ours millis
    ++numLoop;
  }
  // transfer our values from onFor/offFor, into onTimes and offTimes;
  decltype(pump)::PUMP_TIME_TYPE onTimes[pump.PUMP_STEPS];
  decltype(pump)::PUMP_TIME_TYPE offTimes[pump.PUMP_STEPS];
  onTimes[0] = millis()+PUMP_START_DELAY; // start soon
  offTimes[0] = onTimes[0] + onForMillis[0];
  for(uint32_t k=1;k<pump.PUMP_STEPS;++k) {
    onTimes[k] = offTimes[k-1] + offForMillis[k-1];
    offTimes[k] = onTimes[k] + onForMillis[k];
  }
//#ifdef DEBUG_OUTPUT
  printArray(pump.PUMP_STEPS, onForMillis);
  printArray(pump.PUMP_STEPS, offForMillis);
  printArray(pump.PUMP_STEPS, onTimes);
  printArray(pump.PUMP_STEPS, offTimes);
//#endif
  pump.setProgram(pump.PUMP_STEPS, onTimes, offTimes);
  return 1;
};

// returns the value taken off the string, as well as the next position
// returns -1 if there is no next position
int takeNext(String const& command, uint16_t const start, int & outValue)
{
  // check for 1-before the last character, in case the last character
  // is a ',' we can ignore it
  if( start >= command.length()-1 ) {
    return -1;
  }
  int outMark = command.indexOf(',',start);
  if( outMark == -1 ) {
    outValue = command.substring(start).toInt();
    return command.length(); // indicate that the value is good, but we're at the end
  }
  else {
    outValue = command.substring(start,outMark).toInt();
  }
  return outMark;
};
