//
// Created by gwillen on 10/10/21.
//

#include "thermostat.h"
#include <string.h>
#include "menu.h"

const char *state_id_map[] = {
        "",
        "POWERED ON", // base state
        "SYSTEM OFF", // no mode selected
        "SYSTEM HEAT", // heating
        "SYSTEM HEATING",
        "SYSTEM COOL",
        "SYSTEM COOLING"
};

/**
 * Handle user input from cmd line
 * @param thermostat
 */
void thermostat_cmd_handler(thermostat_t *thermostat) {
    char buffer[MENU_WIDTH];
    menu_get_cmd(buffer);
    if (buffer[0] != '\n') {
        statemachine_dispatch((statemachine_t *) thermostat, buffer[0], NULL);
    }
}
/**
 * The menu for this thermostat displays all important values and available commands
 * @param thermostat
 * @param mode
 */
void thermostat_menu(thermostat_t *thermostat, const char *mode) {
    char buffer[MENU_WIDTH];
    menu_put_divider();
    menu_put_centered("EMERSON THERMOSTAT");
    sprintf(buffer, "[MODE: %s]", mode);
    menu_put_centered(buffer);
    memset(buffer, '\0', MENU_WIDTH);
    sprintf(buffer, "[TEMPERATURE: %0.2f]", thermostat->current_temperature);
    menu_put_centered(buffer);
    sprintf(buffer, "[COOL SETPOINT: %0.2f, HEAT SETPOINT: %0.2f]", thermostat->mode.cool->setpoint,
            thermostat->mode.heat->setpoint);
    menu_put_centered(buffer);
    menu_put_divider();
    menu_put_option('0', "set off");
    menu_put_option('1', "set heat mode");
    menu_put_option('2', "set cool mode");
    menu_put_option('3', "set temperature");
    menu_put_option('4', "set heat setpoint");
    menu_put_option('5', "set cool setpoint");
    menu_put_option('6', "set cool minimum active time");
    menu_put_option('9', "power off");
}

/**
 * This logs each state entry to the console
 * @param statemachine
 * @param state
 * @param trigger
 */
void thermostat_log_entry(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    (void)(statemachine);
    (void)(trigger);
    printf("[THERMOSTAT] %s ENTRY\n", state_id_map[state->id]);
}
/**
 * This logs each state exit to the console
 * @param statemachine
 * @param state
 * @param trigger
 */
void thermostat_log_exit(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    (void)(statemachine);
    (void)(trigger);
    printf("[THERMOSTAT] %s EXIT\n", state_id_map[state->id]);
}
/**
 * Log transition effects to the console
 * @param statemachine
 * @param transition
 */
void thermostat_log_effect(statemachine_t *statemachine, transition_t *transition) {
    (void)(statemachine);
    printf("[THERMOSTAT] %s -> %s\n", state_id_map[transition->source], state_id_map[transition->target]);
}

/**
 * Thermostat has entered into a mode. Available modes are 'OFF', 'HEAT', 'COOL'
 * @param statemachine
 * @param state
 * @param trigger
 */
void thermostat_mode_entry(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    thermostat_log_entry(statemachine, state, trigger);
    ((thermostat_t *) statemachine)->mode.current = state->data;
    ((thermostat_mode_data_t *) state->data)->active_timestamp = time(NULL);
}
/**
 * Th
 * @param statemachine
 * @param state
 * @param trigger
 */
void thermostat_off_entry(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    thermostat_log_entry(statemachine, state, trigger);
}

void thermostat_power_off(statemachine_t *statemachine, transition_t *transition) {
    thermostat_log_effect(statemachine, transition);
    statemachine_terminate(statemachine);
}

void thermostat_mode_on_entry(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    thermostat_log_entry(statemachine, state, trigger);
}

int thermostat_mode_on_constraint(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition);
    thermostat_t *thermostat = (thermostat_t *) statemachine;
    if (thermostat->mode.current->invert) {
        return thermostat->current_temperature > thermostat->mode.current->setpoint;
    }
    return  thermostat->current_temperature < thermostat->mode.current->setpoint;

}


int thermostat_mode_active_constraint(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition); // avoiding compiler warnings
    thermostat_t *thermostat = (thermostat_t *) statemachine;
    time_t now = time(NULL);
    return (now - thermostat->mode.current->active_timestamp) > thermostat->mode.current->minimum_active_time;
}



int thermostat_mode_off_constraint(statemachine_t *statemachine, transition_t *transition) {
    if (thermostat_mode_active_constraint(statemachine, transition)) {
        thermostat_t *thermostat = (thermostat_t *) statemachine;
        if (thermostat->mode.current->invert) {
            return thermostat->current_temperature <= thermostat->mode.current->setpoint;
        }
        return  thermostat->current_temperature >= thermostat->mode.current->setpoint;
    }
     return 0;
}

void thermostat_set_float(float *value) {
    printf("enter value: ");
    scanf("%f", value);
    // clear the console of spaces and up until newline
    char c;
    while((c = getchar()) != '\n' && c != '\r');
}
/**
 * Set the thermostat current temperature
 * @param statemachine
 * @param transition
 */
void thermostat_set_temperature(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition); // avoid compiler warnings
    (void)(statemachine); // avoid compiler warnings
    thermostat_set_float(&((thermostat_t *) statemachine)->current_temperature);
}
/**
 * set the point at which the thermostat enters the cooling state
 * @param statemachine
 * @param transition
 */
void thermostat_set_cool_setpoint(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition); // avoid compiler warnings
    thermostat_set_float(&((thermostat_t *) statemachine)->mode.cool->setpoint);
}

void thermostat_set_heat_setpoint(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition); // avoid compiler warnings
    thermostat_set_float(&((thermostat_t *) statemachine)->mode.heat->setpoint);
}


void thermostat_set_minimum_active_time(statemachine_t *statemachine, transition_t *transition) {
    (void)(transition); // avoid compiler warnings
    thermostat_set_float(&((thermostat_t *) statemachine)->mode.cool->minimum_active_time);
}

/**
 * Heating substates
 */
state_t heating_substates[] = {
        {
                THERMOSTAT_HEATING,
                NULL,
                .entry = thermostat_mode_on_entry,
                .exit = thermostat_log_exit
        },
        NULL_ELEMENT
};
/**
 * Cooling substates
 */
state_t cooling_substates[] = {
        {
                THERMOSTAT_COOLING,
                NULL,
                .entry = thermostat_mode_on_entry,
                thermostat_log_exit
        },
        NULL_ELEMENT
};

thermostat_mode_data_t thermostat_mode_data[3] = {
        {0},
        {
                .setpoint=72
        },
        {
                .setpoint=72,
                .minimum_active_time=60,
                .invert=-1
        }
};

state_t thermostat_powered_on_states[] = {
        {
                THERMOSTAT_OFF,
                NULL,
                .entry = thermostat_off_entry,
                .exit = thermostat_log_exit,
                .data = &thermostat_mode_data[0]
        },
        {
                THERMOSTAT_HEAT,
                heating_substates,
                .entry = thermostat_mode_entry,
                .exit = thermostat_log_exit,
                .data = &thermostat_mode_data[1]
        },
        {
                THERMOSTAT_COOL,
                cooling_substates,
                .entry = thermostat_mode_entry,
                .exit = thermostat_log_exit,
                .data = &thermostat_mode_data[2]
        },
        NULL_ELEMENT
};


transition_t thermostat_transitions[] = {
        // from OFF to COOLING
        {
                THERMOSTAT_OFF,
                THERMOSTAT_COOL,
                {THERMOSTAT_SET_MODE_COOL},
                .effect = thermostat_log_effect
        },
        {
                THERMOSTAT_COOL,
                THERMOSTAT_OFF,
                .trigger.event=THERMOSTAT_SET_MODE_OFF,
                .guard = thermostat_mode_active_constraint
        },
        {
                THERMOSTAT_COOL,
                THERMOSTAT_HEAT,
                .trigger.event=THERMOSTAT_SET_MODE_HEAT,
                .guard = thermostat_mode_active_constraint
        },
        {
                THERMOSTAT_OFF,
                THERMOSTAT_HEAT,
                THERMOSTAT_SET_MODE_HEAT,
                .effect = thermostat_log_effect
        },
        {
                THERMOSTAT_HEAT,
                THERMOSTAT_COOL,
                THERMOSTAT_SET_MODE_COOL,
                .effect = thermostat_log_effect

        },
        {
                THERMOSTAT_HEAT,
                THERMOSTAT_OFF,
                THERMOSTAT_SET_MODE_OFF,
                .effect = thermostat_log_effect

        },
        {
                .source = THERMOSTAT_POWERED_ON,
                .trigger.event = THERMOSTAT_POWER_OFF,
                .effect = thermostat_power_off
        },
        {
                .source = THERMOSTAT_COOLING,
                .target = THERMOSTAT_COOL,
                .guard = thermostat_mode_off_constraint
        },
        {
                .source = THERMOSTAT_HEATING,
                .target = THERMOSTAT_HEAT,
                .guard = thermostat_mode_off_constraint
        },
        {
                .source = THERMOSTAT_COOL,
                .target = THERMOSTAT_COOLING,
                .guard = thermostat_mode_on_constraint
        },
        {
                .source = THERMOSTAT_HEAT,
                .target = THERMOSTAT_HEATING,
                .guard = thermostat_mode_on_constraint
        },
        {
                .source = THERMOSTAT_POWERED_ON,
                .trigger.event = THERMOSTAT_SET_TEMPERATURE,
                .effect = thermostat_set_temperature
        },
        {
                .source = THERMOSTAT_POWERED_ON,
                .trigger.event = THERMOSTAT_SET_HEAT_SETPOINT,
                .effect = thermostat_set_heat_setpoint
        },
        {
                .source = THERMOSTAT_POWERED_ON,
                .trigger.event = THERMOSTAT_SET_COOL_SETPOINT,
                .effect = thermostat_set_cool_setpoint
        },
        {
                .source = THERMOSTAT_POWERED_ON,
                .trigger.event = THERMOSTAT_SET_MIN_ACTIVE_TIME,
                .effect = thermostat_set_minimum_active_time
        },
        NULL_ELEMENT

};

thermostat_t thermostat = {
        {
                {
                        THERMOSTAT_POWERED_ON,
                        (state_t *) thermostat_powered_on_states,
                        .initial.target = THERMOSTAT_OFF,
                        .entry = thermostat_log_entry,
                        thermostat_log_exit,
                },
                thermostat_transitions,
        },
        .mode.heat = &thermostat_mode_data[1],
        .mode.cool = &thermostat_mode_data[2],
        .current_temperature = 72
};
/**
 * run the thermostat program
 */
void thermostat_run() {
    statemachine_init((statemachine_t *) &thermostat);
    while (thermostat.statemachine.root.active) {
        statemachine_step((statemachine_t *) &thermostat);
        state_t *active = statemachine_get_active_state((state_t *) &thermostat);
        thermostat_menu(&thermostat, state_id_map[active == NULL ? 0 : active->id]);
        thermostat_cmd_handler(&thermostat);
    }
    puts("[THERMOSTAT] POWERED OFF");
}