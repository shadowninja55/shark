#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#define QUEUE_CAP 64

void keyboard_trigger(void);
bool keyboard_is_down(char c);
char keyboard_get_next(void);

#endif
