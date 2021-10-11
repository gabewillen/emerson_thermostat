//
// Created by gwillen on 10/9/21.
//

#include "statemachine.h"


state_t *
execute_transition(statemachine_t *statemachine, state_t *source, state_t *target, transition_t *transition);

state_t *statemachine_get_active_state(state_t *state) {
    state_t *active = NULL;
    if (state->active) {
        for (state_t *substate=state->substates; substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
            if ((active = statemachine_get_active_state(substate)) != NULL) return active;
        }
        return state;
    }
    return active;
}
/**
 * is the substate a descendant of the ancestor state
 * @param state
 * @param substate
 * @return true if the substate is a descendant of the ancestor state
 */
char is_descendant(const state_t *state, const state_t *substate) {
    if (state == substate) return 0;
    for (state_t *iter = state->substates; iter != NULL && iter->id != NULL_ELEMENT_ID; iter++) {
        if (iter == substate || is_descendant(iter, substate)) {
            return 1;
        }
    }
    return 0;
}

/**
 * get a substate given its id
 * @param state
 * @param id
 * @return NULL if the id isn't a substate of this state
 */
state_t *get_state_by_id(state_t *state, short id) {
    if (state->id == id) return state;
    for (state_t *substate = state->substates; substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
        if (substate->id == id) {
            return substate;
        } else if ((state = get_state_by_id(substate, id)) != NULL) {
            return state;
        }
    }
    return NULL;
}
/**
 * Get the ancestor of this state by traversing down from the root state
 * @param ancestor
 * @param descendant
 * @return the ancestor unless descendant is a top level state it will return NULL
 */
state_t *get_ancestor_state(state_t *ancestor, state_t *descendant) {
    for (state_t *substate = ancestor->substates;
         substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
        if (substate == descendant) {
            return ancestor;
        } else if (is_descendant(substate, descendant)) {
            return get_ancestor_state(substate, descendant);
        }
    }
    return NULL;
}


/**
 * Exit a state by first recursively exiting its active substates. Then invoke its exit action
 * @param statemachine
 * @param state
 * @param trigger
 * @return
 */
state_t *exit_state(statemachine_t *statemachine, state_t *state, trigger_t *trigger) {
    if (state->active) {
        for (state_t *substate = state->substates;
             substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
            exit_state(statemachine, substate, trigger);
        }
        if (state->exit != NULL) {
            state->exit(statemachine, state, trigger);
        }
        state->active = 0;
        return state;
    }
    return NULL;
}

state_t *enter_state(statemachine_t *statemachine, state_t *current, state_t *target, trigger_t *trigger) {
    // if the current state isn't active lets activate it and preform its entry behavior
    if (!current->active) {
        current->active = 1;
        if (current->entry != NULL) {
            current->entry(statemachine, current, trigger);
        }
    }
    // If this is a compl
    if (target == NULL || current == target) {
        if (current->initial.target != NULL_ELEMENT_ID) {
            target = get_state_by_id((state_t *)statemachine, current->initial.target);
            return execute_transition(statemachine, current, target, &current->initial);
        }
    } else if (is_descendant(current, target)) {
        for (state_t *substate = current->substates;
             substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
            if (substate == target || is_descendant(substate, target)) {
                return enter_state(statemachine, substate, target, trigger);
            }
        }
    }
    return current;
}



void execute_transition_effect(statemachine_t *statemachine, transition_t *transition) {
    if (transition->effect != NULL) {
        transition->effect(statemachine, transition);
    }
}

state_t *
execute_transition(statemachine_t *statemachine, state_t *source, state_t *target, transition_t *transition) {
    if (target == NULL) {
        execute_transition_effect(statemachine, transition);
        return source;
    } else {
        for (state_t *substate = source->substates;
             substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
            if (exit_state(statemachine, substate, &transition->trigger) != NULL) break;
        }
        if (source == target || !is_descendant(source, target)) {
            exit_state(statemachine, source, &transition->trigger);
            source = get_ancestor_state((state_t *) statemachine, source);
        }
        execute_transition_effect(statemachine, transition);
        return enter_state(statemachine, source, target, &transition->trigger);
    }
}

int evaluate_transition(statemachine_t *statemachine, transition_t *transition) {
    if (transition->guard != NULL) {
        return transition->guard(statemachine, transition);
    }
    return 1;
}

state_t *process_completion_transitions(statemachine_t *statemachine, state_t *current) {
    for (transition_t *transition=statemachine->transitions; transition != NULL && transition->source != NULL_ELEMENT_ID; transition++) {
        if (transition->source == current->id) {
            if (transition->trigger.event == NULL_ELEMENT_ID) {
                state_t *target = get_state_by_id((state_t *)statemachine, transition->target);
                if (target != NULL) {
                    if (!target->active || is_descendant(target, current)) {
                        if (evaluate_transition(statemachine, transition)) {
                            return execute_transition(statemachine, current, target, transition);
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

state_t *process(statemachine_t *statemachine, state_t *current, trigger_t *trigger) {
    state_t *state;
    if (current->active) {
        // first make sure a substate won't consume this event
        for (state_t *substate = current->substates;
             substate != NULL && substate->id != NULL_ELEMENT_ID; substate++) {
            if (substate->active) {
                if ((state = process(statemachine, substate, trigger))) return state;
                break;
            }
        }
        if ((state = process_completion_transitions(statemachine, current)) != NULL) return state;
        for (transition_t *transition=statemachine->transitions; transition != NULL && transition->source != NULL_ELEMENT_ID; transition++) {
            if (trigger == NULL) trigger = &transition->trigger;
            if (current->id == transition->source && transition->trigger.event == trigger->event && trigger->active) {
                transition->trigger = *trigger;
                if (evaluate_transition(statemachine, transition)) {
                    state_t *target = get_state_by_id((state_t *)statemachine, transition->target);
                    state = execute_transition(statemachine, current, target, transition);
                    trigger->active = 0;
                    return state;
                }
            }
        }

    }

    return NULL;
}


state_t *statemachine_process(statemachine_t *statemachine, trigger_t *trigger) {
    statemachine->processing = 1;
    state_t *state = process(statemachine, (state_t *)statemachine, trigger);
    statemachine->processing = 0;
    return state;
}

state_t *statemachine_dispatch(statemachine_t *this, event_t event, void *data) {
    // if the statemaching is in the middle of processing a trigger add it to the pool
    for (transition_t *transition = this->transitions;
         transition != NULL && transition->source != NULL_ELEMENT_ID; transition++) {
        if (!transition->trigger.active && transition->trigger.event == event) {
            transition->trigger.data = data;
            transition->trigger.active = 1;
            if (!this->processing) {
                return statemachine_process(this, &transition->trigger);
            }
        }
    }
    return NULL;
}


state_t *statemachine_step(statemachine_t *statemachine) {
    return statemachine_process(statemachine, NULL);
}

state_t *statemachine_init(statemachine_t *this) {
    return enter_state(this, (state_t *) this, NULL, NULL);
}

void statemachine_terminate(statemachine_t *this) {
    exit_state(this, (state_t *) this, NULL);
}