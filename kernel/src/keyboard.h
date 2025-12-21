#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#define QUEUE_CAP 32

void keyboard_trigger(void);
bool keyboard_get(char c);
void keyboard_get_queue(char *out);

#endif
