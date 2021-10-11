/**
 * A simple console menu
 * Author: Gabriel Willen
 */
#ifndef EMERSON_THERMOSTAT_MENU_H
#define EMERSON_THERMOSTAT_MENU_H

#include <stdio.h>
#include <string.h>

#define MENU_WIDTH 60

/**
 * Put a divider of '-' at specified MENU_WIDTH
 */
void menu_put_divider();
/**
 * Put message centered between '|' to create a frame
 * @param message
 */
void menu_put_centered(char *message);
/**
 * Create a menu option '[option] description'
 * @param option
 * @param description
 */
void menu_put_option(char option, char *description);
/**
 * Put `cmd: ` prompt on console and wait for input.
 * @param buffer
 */
void menu_get_cmd(char *buffer);


#endif //EMERSON_THERMOSTAT_MENU_H
