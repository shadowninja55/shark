#include "util.h"
#include "printf.h"

int spacebar;
int letters[26];
char *key_lookup = "qwertyuiopasdfghjkl zxcvbnm";

int queue_len;
char queue[32];

void keyboard_trigger(void) {
	uint8_t code = inb(0x60);
	int down = !(code &  128); // bit 8 is for keypress/release
	int key  =   code & ~128;

	if (key != 9 && (key < 16 || (25 < key && key < 30) || (38 < key && key < 44) || 50 < key)) return; // not a letter/space

	char ch;
	if (code == 9) spacebar = down;
	else {
		key -= 16;
		ch = key_lookup[key - key/14*4];
		letters[ch-'a'] = down;
	}
	if (down && queue_len < 32) queue[queue_len++] = ch;
}

int keyboard_get(char c) {
	if (c == ' ') return spacebar;
	return letters[c-'a'];
}

// out should be at least 33 bytes, and will be null-terminated.
void keyboard_get_queue(char *out) {
	memcpy(out, queue, queue_len);
	out[queue_len] = 0;
	queue_len = 0;
}
