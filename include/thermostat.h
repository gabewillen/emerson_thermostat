/**
 * Emerson thermostat coding assignment for an explanation please see the project README.md
 * Author: Gabriel Willen
 */
#ifndef EMERSON_THERMOSTAT_THERMOSTAT_H
#define EMERSON_THERMOSTAT_THERMOSTAT_H


#include "statemachine.h"
#include <time.h>

/**
 * Thermostat Events
 */
enum {
    THERMOSTAT_SET_MODE_OFF = '0',
    THERMOSTAT_SET_MODE_HEAT = '1',
    THERMOSTAT_SET_MODE_COOL = '2',
    THERMOSTAT_SET_TEMPERATURE = '3',
    THERMOSTAT_SET_HEAT_SETPOINT = '4',
    THERMOSTAT_SET_COOL_SETPOINT = '5',
    THERMOSTAT_SET_MIN_ACTIVE_TIME = '6',
    THERMOSTAT_POWER_OFF = '9'
};


/**
 * State ID enumeration
 */
enum {
    THERMOSTAT_POWERED_ON = 1, // base state
    THERMOSTAT_OFF, // no mode selected
    THERMOSTAT_HEAT, // heating
    THERMOSTAT_HEATING,
    THERMOSTAT_COOL,
    THERMOSTAT_COOLING
};

/**
 * A mode is a thermostat state for heating, cooling, & off
 */
typedef struct {
    float setpoint; // set this to control when the mode activates
    time_t active_timestamp;
    char invert;
    float minimum_active_time; // set this to prevent short cycling the equipment in seconds
} thermostat_mode_data_t;


/**
 * The system region is responsible for responding to user input
 */
typedef struct {
    statemachine_t statemachine; // base struct
    float current_temperature; // current temperature reading
    struct {
        thermostat_mode_data_t *current;
        thermostat_mode_data_t *cool;
        thermostat_mode_data_t *heat;
    } mode;
} thermostat_t;

/**
 * Thermostat run
 */
void thermostat_run();




#endif //EMERSON_THERMOSTAT_THERMOSTAT_H
