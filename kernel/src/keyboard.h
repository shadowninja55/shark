#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_trigger(void);
int keyboard_get(char c);
void keyboard_get_queue(char *out);

#endif
