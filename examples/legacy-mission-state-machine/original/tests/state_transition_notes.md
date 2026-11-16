# State Transition Test Notes

## Manual Test Cases

| Case | Expected Result |
| --- | --- |
| Normal telemetry | Mission proceeds to next state |
| Low battery | Mission enters FAILSAFE |
| Low signal | Mission enters FAILSAFE |
| GPS unsafe | Mission enters FAILSAFE |
| High temperature | Mission enters FAILSAFE |

## Future Test Ideas

- Unit test for every state transition
- Mission event timeline replay
- Invalid transition blocking
- Failsafe priority ordering