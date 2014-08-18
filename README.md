microcode-v2
============

Firmware to power the WiBean using a Spark.io module


API Commands
============

Spark.function("heatTarget", heatTarget);
-----------------------------------------
This command set the target or "goal" temperature to be acheived by the heating loop.  This command does not enable or disable heat, it merely sets the goal temperature.

Spark.function("heatToggle", heatToggle);
-----------------------------------------

Spark.function("pumpControl", pumpCommand);
-------------------------------------------

Spark.function("toggleAlarm", alarmCommand);
--------------------------------------------

Spark.variable("headTemp", ,DOUBLE);
--------------------------------------------------------------
Tracks the thermistor which should be connected to the brewing head.

Spark.variable("ambientTemp", ,DOUBLE);
--------------------------------------------------------------------
Tracks the thermistor which can be connected to an "ambient" position.  Not required.

Spark.variable("status", , STRING);
-----------------------------------------------
A combined variable which tracks many parameters.  The string is updated at 1Hz and is itself valid JSON.  It contains the following...

    {  
       "ala":false, # Alarm Active : true/false
       "alt":65535, # Alarm Time : uint16_t set as minutes after midnight
       "b":false,   # Brewing coffee right now? : true/false
       "h":2,       # State of the heating loop, see below
       "t_h":11.0,  # Current temperature head in degrees C, to the 10th of a degree
       "t_a":11.8   # Current ambient temperature in degrees C, to the 10th of a degree
    }
    
The heating state machine currently has the following potential states...

    HIBERNATE,          # 0
    DETERMINE_STATE,    # 1
    HEATING_RELAY,      # 2
    HEATING_PULSE,      # 3
    COOLING,            # 4
    PUMPING             # 5