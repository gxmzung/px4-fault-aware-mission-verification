# Mission Flow

## Normal Flow

```text
IDLE -> ARMING -> TAKEOFF -> MISSION -> RETURN_HOME -> LANDING -> COMPLETE

Failsafe Flow

Any state can transition to:

FAILSAFE
Failsafe Conditions
Condition	Rule
Low battery	batteryPercent < 25
Low signal	signalStrength < 40
GPS unsafe	gpsSatellites < 8
High temperature	temperature > 75