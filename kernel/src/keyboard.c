#include <stdbool.h>
#include "keyboard.h"
#include "util.h"
#include "printf.h"

#define SPACE_KEY 0x39

static bool spacebar = false;
static bool letters[26] = { false };
static char queue[QUEUE_CAP];
static size_t queue_len = 0;
const char *keys = "qwertyuiopasdfghjkl zxcvbnm";

void keyboard_trigger(void) {
	uint8_t code = inb(0x60);
	bool    down = !(code &  (1 << 7)); // bit 8 is for keypress / release
	uint8_t key  =   code & ~(1 << 7);

	if (key != SPACE_KEY && (key < 16 || (25 < key && key < 30) || (38 < key && key < 44) || 50 < key)) return; // not a letter / space

	char ch;
	if (key == SPACE_KEY) {
    ch = ' ';
    spacebar = down;
  } else {
		key -= 16;
		ch = keys[key - key / 14 * 4];
		letters[ch - 'a'] = down;
	}
	if (down && queue_len < QUEUE_CAP) queue[queue_len++] = ch;
}

bool keyboard_get(char c) {
	if (c == ' ') return spacebar;
	return letters[c - 'a'];
}

// out should be at least QUEUE_CAP + 1 bytes, and will be null-terminated
void keyboard_get_queue(char *out) {
	memcpy(out, queue, queue_len);
	out[queue_len] = 0;
	queue_len = 0;
}
