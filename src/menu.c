//
// Created by gwillen on 10/10/21.
//

#include "menu.h"

void menu_put_divider() {
    for (int i=0; i < MENU_WIDTH; i++) {
        putchar('-');
    }
    putchar('\n');
}


void menu_put_aligned(short left_margin, const char *string, short right_margin) {
    size_t len = strlen(string);
    putchar('|');
    for (int i=0; i < left_margin; i++) putchar(' ');
    for (int i=0; i < len; i++) putchar(string[i]);
    for (int i=0; i < right_margin; i++) putchar(' ');
    putchar('|');
    putchar('\n');
}


void menu_put_centered(char *string) {
    size_t len = strlen(string);
    size_t margin = (MENU_WIDTH - 2 - len) / 2;
    menu_put_aligned(margin, string, margin + (MENU_WIDTH - margin - len - margin - 2));
}

void menu_put_option(char option, char *string) {
    char buffer[MENU_WIDTH];
    sprintf(buffer, "[%c] %s", option, string);
    size_t len = strlen(buffer);
    menu_put_aligned(1, buffer, (MENU_WIDTH - 3 - len));
}

void menu_get_cmd(char *buffer) {
    menu_put_divider();
    printf("cmd: ");
    fgets(buffer, MENU_WIDTH, stdin);
}