// This #include statement was automatically added by the Spark IDE.
#include "SortedLookupTable.h"

#include <limits>

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
// an N-point moving average filter on a float value
#include "AveragingFloatBuffer.h"

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
IntervalTimer statusTimer;
// Thermistor LUT
Thermistor thermistor;
// the way spark works, we can only return data to the user via the Spark.variable command
double temperatureInCelsius_head = 0;
double temperatureInCelsius_ambient = 0;
AveragingFloatBuffer<10> headTemperatureHistory;
AveragingFloatBuffer<10> ambientTemperatureHistory;
// pump controller
PumpProgram<5> pump;
// variables which get triggered by the hardware timers
volatile bool runHeating = false;
volatile bool runPump = false;
volatile bool runStatus = false;

const uint16_t STATUS_AS_JSON_LENGTH = 128;
char statusAsJson[STATUS_AS_JSON_LENGTH];

// Alarm functions
const uint32_t INACTIVITY_SHUTOFF_IN_MILLISECONDS = 45*60*1000; // 45 minutes
const uint16_t MINUTES_IN_DAY = 60*24;
uint32_t time_last_command = 0;
uint16_t alarm_waketime_minutes = std::numeric_limits<uint16_t>::max();


// BEGIN CODE
void setup() {
    Serial.begin(9600);
    //Register our Spark function here
    Spark.function("heatTarget", heatTarget);
    Spark.function("heatToggle", heatToggle);
    Spark.function("pumpControl", pumpCommand);
    Spark.function("toggleAlarm", alarmCommand);

    // I would much rather this be a float, but they only offer DOUBLEs
    Spark.variable("headTemp", &temperatureInCelsius_head,DOUBLE);
    Spark.variable("ambientTemp", &temperatureInCelsius_ambient,DOUBLE);

    // setup the c-str to initially be empty
    sprintf(statusAsJson,"init");
    Spark.variable("status", statusAsJson, STRING);

    configurePins();
    // setup hardware timer which controls heating loop and pump loop
    setupTimers();
}
void configurePins() {
    // Configure OUTPUTS
   pinMode(PUMP_PIN, OUTPUT);
   pinMode(VALVE_PIN, OUTPUT);
   pinMode(HEATING_RELAY_PIN, OUTPUT);
   pinMode(HEATING_THYRISTOR_PIN, OUTPUT);
   // Configure INPUTS
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
    heatingTimer.begin(interruptHeat, 100, hmSec);
    // called every 100ms
    pumpTimer.begin(interruptPump, 200, hmSec);
    // called every 1000ms
    statusTimer.begin(interruptStatus, 2000, hmSec);
}

void loop() {
  if( runHeating ) {
    heatingLoop();
    runHeating = false;
  }
  if( runPump ) {
    pumpLoop();
    runPump = false;
  }
  if( runStatus ) {
    updateStatusString();
    runStatus = false;
  }
  // monitor the dead-mans heating cutoff
  uint32_t currentTime = millis();
  if( heater.isHeating()) {
    uint32_t deltaTime = 0;
    if( currentTime < time_last_command ) {
      // NOTE: for the math below, we will always reach the cutoff time before this
      // would overflow (because the cutoff interval itself is a uint32_t)
      deltaTime = std::numeric_limits<uint32_t>::max() - time_last_command + currentTime;
    }
    else {
      deltaTime = currentTime - time_last_command;
    }
    if( deltaTime >= INACTIVITY_SHUTOFF_IN_MILLISECONDS) {
      heater.enableHeating(false);
    }
  }
  // monitor the wake alarm
  // to disable wake-alarm simply set alarm to value > minutesInADay
  uint16_t currentMinutes = Time.hour()*60 + Time.minute();
  if( currentMinutes == alarm_waketime_minutes ) {
    time_last_command = currentTime;
    heater.enableHeating(true);
  }
}


// the hardware timers are used this way as it is apparent that functions
// triggered by the hardware timers must use volatile variables (which don't
// play nicely with some of the spark.io library functions) and also you cannot
// use delay in these functions, and I don't want to guarantee that in the whole
// execution chain.
void interruptHeat() {
  runHeating = true;
}
void interruptPump() {
  runPump = true;
}
void interruptStatus() {
  runStatus = true;
}

void heatingLoop() {
    temperatureUpdate();
    heater.updateCurrentTemp(temperatureInCelsius_head);
    // should we be using the relay OR are we brewing coffee?
    bool runRelay = (heater.runRelay() || pump.isValveOpenAt(millis()));
    // check the relay
    if( runRelay ) {
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
void temperatureUpdate() {
  delay(2); // wait 2 ms for ADC to recharge
  headTemperatureHistory.add(thermistor.getTemperature( analogRead(THERMISTOR_PIN_HEAD) ));
  temperatureInCelsius_head = headTemperatureHistory.average();
  delay(2); // wait 2 ms for ADC to recharge
  ambientTemperatureHistory.add(thermistor.getTemperature( analogRead(THERMISTOR_PIN_AMBIENT) ));
  temperatureInCelsius_ambient = ambientTemperatureHistory.average();
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
  if( command.length() >= 1 ) {
    heater.enableHeating( command.charAt(0) == '1' );
    return 1;
  }
  else {
    return -1;
  }
}

int pumpCommand(String command) {
  // pump commands come in CSV, e.g. 32,44,33,22,0,0,0,33
  // as onFor,offFor,onFor,offFor.... up to the max limit set by PumpControl
  // and they come in with units of 100ms per unit.
#ifdef SERIAL_DEBUG
  Serial.println("pumpCommand!");
#endif
  // did they send any parameters?
  if(command.length() == 0) {
    return -1;
  }
  uint16_t curChar = 0;
  uint16_t numLoop = 0;
  decltype(pump)::PUMP_TIME_TYPE onForMillis[pump.PUMP_STEPS];
  decltype(pump)::PUMP_TIME_TYPE offForMillis[pump.PUMP_STEPS];
  memset(onForMillis, 0, sizeof(decltype(pump)::PUMP_TIME_TYPE)*pump.PUMP_STEPS);
  memset(offForMillis, 0, sizeof(decltype(pump)::PUMP_TIME_TYPE)*pump.PUMP_STEPS);

  while( (numLoop < pump.PUMP_STEPS) ) {
    int value;
    curChar = wibean::utils::takeNext(command, curChar, value) + 1;
#ifdef SERIAL_DEBUG
    Serial.print("curChar: ");
    Serial.println(curChar);
#endif
    if( curChar == 0 ) { break; }
    onForMillis[numLoop] = value*100; // values come as 100ms ticks, make ours millis
    // 0 because we added one
    curChar = wibean::utils::takeNext(command, curChar, value) + 1;
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
  wibean::utils::printArray(pump.PUMP_STEPS, onForMillis);
  wibean::utils::printArray(pump.PUMP_STEPS, offForMillis);
  wibean::utils::printArray(pump.PUMP_STEPS, onTimes);
  wibean::utils::printArray(pump.PUMP_STEPS, offTimes);
//#endif
  pump.setProgram(pump.PUMP_STEPS, onTimes, offTimes);
  return 1;
};


int alarmCommand(String command)
{
  // alarm is set as minutesAfterStartOfDay.  To disable, simply set a value
  // greater than number of minutes in day
  int minutesAfterDayStart = command.toInt();
  if( (minutesAfterDayStart < 0) || (minutesAfterDayStart > std::numeric_limits<uint16_t>::max()) ) {
    return -1;
  }
  else {
    alarm_waketime_minutes = minutesAfterDayStart;
    return 1;
  }
}


void updateStatusString() {
  // include a global declaration so that the String(value, BASE) constructor works correctly
  unsigned char DEC_BASE = static_cast<unsigned char>(DEC);
  String tempBuilder = "";
  int tempValue = 0;
  // status string is json with following fields
  tempBuilder = "{";
  // is the alarm active
  tempBuilder += "\"ala\": " + wibean::utils::boolToString(alarm_waketime_minutes < MINUTES_IN_DAY) + ",";
  // when is the alarm set for
  tempValue = alarm_waketime_minutes;
  tempBuilder += "\"alt\": " + String(tempValue,DEC_BASE) + ",";
  // is the machine brewing right now?;
  tempBuilder += "\"b\": " + wibean::utils::boolToString(pump.isValveOpenAt(millis())) + ",";
  // is the machine heating right now?
  tempBuilder += "\"h\": " + String(heater.getState(),DEC_BASE) + ",";
  // what is the head temp right now?
  tempBuilder += "\"t_h\": " + wibean::utils::floatToString(temperatureInCelsius_head) + ",";
  // what is the ambient temp right now?
  tempBuilder += "\"t_a\": " + wibean::utils::floatToString(temperatureInCelsius_ambient) + "";
  // end JSON
  tempBuilder += "}";

  tempBuilder.toCharArray(statusAsJson,STATUS_AS_JSON_LENGTH);
};
