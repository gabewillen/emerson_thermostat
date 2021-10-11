# Emerson Thermostat Assignment

---
I will use this readme to document my thoughts as I design and develop this statemachine.

## Assignment

---
Build a simple state machine to represent a thermostat.  
* A thermostat can have 3 states THERMOSTAT_HEAT, THERMOSTAT_COOL, and THERMOSTAT_OFF.
* The application should allow the manipulation of the following data points: current temperature, heat setpoint, cool setpoint, operating mode.  Recommended implementation is a command line interface in a busy loop and a state machine evaluation busy loop.
* The thermostat should start up with a current state of THERMOSTAT_OFF.
* When a thermostat is in operating mode heat, and the current temperature is below the heat setpoint then the thermostat should enter the THERMOSTAT_HEAT state.
* When a thermostat is in operating mode cool, and the current temperature is above the cool setpoint then the thermostat should enter the THERMOSTAT_COOL state.
* If the thermostat is cooling, it should maintain the THERMOSTAT_COOL state for a minimum of 1 minute regardless of other changes.
* In all other scenarios the thermostat should remain in the THERMOSTAT_OFF state.
* When a thermostat transitions its state it should notify the user with a message to indicate what transition was made Example: “Transition from THERMOSTAT_OFF -> THERMOSTAT_COOL”

Your control interface should allow the user to change any of the data points without restarting the state machine.
The accuracy to a real thermostat is not being evaluated.  The only thing that matters is if the requirements above are met.  Use your discretion to protect against edge cases.

## Notes

---

1. It wasn't exactly clear what OS this should work on. I do all of my development on Linux and I wasn't able to test
    compilation on Windows. I don't forsee it being an issue as It's only using std library headers and no dependencies.
2. I'm not sure if I am allowed to use existing statemachine frameworks. So for sake of completeness I chose to roll my own.
3. I would have preferred to have user input and system operation running concurrently using threads or tasks. This wasn't explicitly stated, so I will assume they are unavailable on the target platform.
4. I won't be making any performance or code size optimizations. I will instead focus my time on ensuring compliance with assignment requirements.
5. All together with documenting code and writing this README it took 8 hours.

## Implementation

---

Here I will write my general observations and thoughts. There are several methods to accomplish the assignment, I am in no
way asserting my method is the most performant or correct approach. The easiest way this could have been done is a simple
switch statement based statemachine that fit the exact requirements of the assignments. I decided to write a more generic state machine library. While 
it might take additional time to develop. In the event I need to create another statemachine in C. I'll have this ready to go. Also I chose to use a stripped down hierarchical state machine.
It's similar to the original concept written by David Harel. 

For user interaction I created a crude ncurses like menu. It repaints to the console after input is entered or in the event a transaction is executed.
The menu unfortunately blocks the execution of the statemachine and would be much better in a thread or task.

## Usage

---

Below is the interface used to interact with the thermostat. The number inside `[]` corresponds to the command.
```
------------------------------------------------------------
|                    EMERSON THERMOSTAT                    |
|                    [MODE: SYSTEM OFF]                    |
|                   [TEMPERATURE: 73.00]                   |
|       [COOL SETPOINT: 72.00, HEAT SETPOINT: 72.00]       |
------------------------------------------------------------
| [0] set off                                              |
| [1] set heat mode                                        |
| [2] set cool mode                                        |
| [3] set temperature                                      |
| [4] set heat setpoint                                    |
| [5] set cool setpoint                                    |
| [6] set cool minimum active time                         |
| [9] power off                                            |
------------------------------------------------------------
cmd: 
```
When setting values you will be presented with an extra prompt. 
```
------------------------------------------------------------
|                    EMERSON THERMOSTAT                    |
|                    [MODE: SYSTEM OFF]                    |
|                   [TEMPERATURE: 73.00]                   |
|       [COOL SETPOINT: 72.00, HEAT SETPOINT: 72.00]       |
------------------------------------------------------------
| [0] set off                                              |
| [1] set heat mode                                        |
| [2] set cool mode                                        |
| [3] set temperature                                      |
| [4] set heat setpoint                                    |
| [5] set cool setpoint                                    |
| [6] set cool minimum active time                         |
| [9] power off                                            |
------------------------------------------------------------
cmd: 3
enter value: 74
```

After a command is entered that results in a statemachine transition the transition traversal and entry and exit
actions will be written to the console.

```
cmd: 2
[THERMOSTAT] SYSTEM HEAT EXIT
[THERMOSTAT] SYSTEM HEAT -> SYSTEM COOL
[THERMOSTAT] SYSTEM COOL ENTRY
[THERMOSTAT] SYSTEM COOLING ENTRY
```

Logs with the `->` represent transition effects

## Build

---

Clone or download the code. Unzip the file and navigate to the directory in your console.
```
cd emerson-thermostat
mkdir build
cd build
cmake ../
make
```
You should end up with a `emerson_thermostat` executable