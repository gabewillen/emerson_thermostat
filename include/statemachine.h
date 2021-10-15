//
// Created by gwillen on 10/9/21.
//

#ifndef EMERSON_THERMOSTAT_STATEMACHINE_H
#define EMERSON_THERMOSTAT_STATEMACHINE_H

#include <stdint.h>
#include <stdio.h>

// change statemachine type if you need more than 256 events
typedef short event_t;

// A placeholder for arrays to allow omitting the array size
#define NULL_ELEMENT_ID (0)
#define NULL_ELEMENT {NULL_ELEMENT_ID}


/**
 * a Trigger is what drives the statemachine execution. Triggers can either be external or internal. They are
 * always associated with an event. They may have extra parameters however the first item in the structure must be
 * a trigger struct.
 */
typedef struct trigger {
    event_t event;
    char active;
    void *data;
} trigger_t;

struct statemachine;

/**
 * a transition is the path taken by the statemachine in response to a trigger. The below explanations assume that the
 * transition has a matching trigger and the guard has evaluated to true. Transition guards and effects should not have
 * any side effects. If an event is dispatched while preforming a transition it will be queued and dispatched on the next
 * execution step.
 *
 * A transition without a target is an internal transition. Internal transitions do not alter the statemachine configuration
 *
 * A transition with a source and target that are identical are self transitions. Self transitions exit and re-enter themselves.
 * effectively resetting the state.
 *
 * A transition with a target that is a descendant of its source is a local transition. Local transitions do not exit the source state. They
 * enter each state as they descend to the target.
 *
 * A transition with a target state that is not a descendant of its source is a external transition. An external transition will exit the source state
 * and invoke the entry behavior of every state as it descends to an external target.
 *
 */
typedef struct transition {
    short source;
    short target;
    trigger_t trigger;
    int (*guard)(struct statemachine *, struct transition *);
    void (*effect)(struct statemachine *, struct transition *);
} transition_t;

/**
 * A state is the core context of a statemachine. A statemachine must always settle on a state before continuing its execution.
 * A state has a unique ID that is used by a transition as its source and target properties.
 */
typedef struct state {
    short id;
    struct state *substates; // an array of substates
    transition_t initial; // optional initial transition to a substate
    void (*entry)(struct statemachine *, struct state *, trigger_t *trigger); // executed when the state is entered
    void (*exit)(struct statemachine *, struct state *, trigger_t *trigger); // executed when exiting the state
    char active;
    void *data;
} state_t ;


/**
 * The statemachine is itself a top level state. Events are processed starting from the most nested state to the top.
 */
typedef struct statemachine {
    state_t root;
    transition_t *transitions;
    volatile char processing;
} statemachine_t;

/**
 * dispatch an event to the statemachine and optionally attach data to its trigger. If the statemachine is already
 * processing an event it stores the event in the statemachines event pool (transitions) and processes on the next
 * step of execution
 * @param statemachine
 * @param event
 * @param data
 * @return state this statemachine settled on after dispatching the event
 */
state_t *statemachine_dispatch(statemachine_t *statemachine, event_t event, void *data);

/**
 * Get the most nested active state in the the state machine configuration
 * @param state
 * @return the active state unless the statemachine hasn't been initialized in which it will return NULL
 */
state_t *statemachine_get_active_state(state_t *state);
/**
 * Initialize the statemachine executing its initial transition
 * @param statemachine
 * @return the state in which the statemachine settled on
 */
state_t *statemachine_init(statemachine_t *statemachine);
/**
 * Terminate the statemachine and exit all of the nested states
 * @param statemachine
 */
void statemachine_terminate(statemachine_t *statemachine);
/**
 * Step through the statemachines triggers and if any are active process them
 * @param statemachine
 * @param triggger
 * @return the state it settled on if a trigger was ready to be processed
 */
state_t *statemachine_step(statemachine_t *statemachine);

#endif //EMERSON_THERMOSTAT_STATEMACHINE_H
