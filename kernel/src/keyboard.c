#include <stdbool.h>
#include "keyboard.h"
#include "util.h"
#include "printf.h"

#define SPACE_KEY 0x39

static bool spacebar = false;
static bool letters[26] = { false };

static char queue[QUEUE_CAP];
static size_t queue_len = 0;
static size_t queue_head = 0;

const char *keys = "qwertyuiopasdfghjkl zxcvbnm";

void keyboard_trigger(void) {
	uint8_t code = inb(0x60);
	bool    down = !(code &  (1 << 7)); // bit 8 is for keypress / release
	uint8_t key  =   code & ~(1 << 7);

	if (key != SPACE_KEY && (key < 16 || (25 < key && key < 30) || (38 < key && key < 44) || 50 < key)) return; // not a letter / space

	char c;
	if (key == SPACE_KEY) {
    c = ' ';
    spacebar = down;
  } else {
		key -= 16;
		c = keys[key - key / 14 * 4];
		letters[c - 'a'] = down;
	}

	if (down && queue_len < QUEUE_CAP) {
    queue[(queue_head + queue_len) % QUEUE_CAP] = c;
    queue_len++;
  }
}

bool keyboard_is_down(char c) {
  return c == ' ' ? spacebar : letters[c - 'a'];
}

// returns nullbyte if the queue is empty
char keyboard_get_next(void) {
  if (!queue_len) return '\0';

  char c = queue[queue_head];
  queue_head = (queue_head + 1) % QUEUE_CAP;
  queue_len--;
  return c;
}
