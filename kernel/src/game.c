#include <stddef.h>
#include "game.h"
#include "printf.h"
#include "clock.h"
#include "random.h"
#include "video.h"
#include "keyboard.h"

#define WIDTH         17
#define HEIGHT        15
#define COLOR_EMPTY_A 0xaad751
#define COLOR_EMPTY_B 0xa2d149
#define COLOR_SNAKE   0x4e7cf6
#define COLOR_APPLE   0xe7471d
#define SNAKE_CAP     (WIDTH * HEIGHT)

enum tile {
  EMPTY,
  SNAKE,
  APPLE,
};

struct v2 {
  int x, y;
};

static int scale;
static bool game_over;
static struct v2 snake[SNAKE_CAP];
static size_t snake_len;
static size_t snake_head;
static struct v2 apple;
static struct v2 dir;

struct v2 add(struct v2 u, struct v2 v) {
  return (struct v2) { u.x + v.x, u.y + v.y };
}

bool equals(struct v2 u, struct v2 v) {
  return u.x == v.x && u.y == v.y;
}

bool in_body(struct v2 p) {
  for (int i = 0; i < snake_len; i++) {
    if (equals(snake[(snake_head + i) % SNAKE_CAP], p)) {
      return true;
    }
  }

  return false;
}

void draw_tile(struct v2 p, enum tile t) {
  uint32_t c;
  switch (t) {
    case EMPTY:
      c = (p.x + p.y) % 2 ? COLOR_EMPTY_B : COLOR_EMPTY_A;
      break;
    case SNAKE:
      c = COLOR_SNAKE;
      break;
    case APPLE:
      c = COLOR_APPLE;
      break;
  }

  for (int dy = 0; dy < scale; dy++) {
    for (int dx = 0; dx < scale; dx++) {
      video_set(p.x * scale + dx, p.y * scale + dy, c);
    }
  }
}

void game_init(void) {
  srand(clock_get());

  // state
  int sx = video_width() / WIDTH;
  int sy = video_height() / HEIGHT;
  scale = sx < sy ? sx : sy;

  game_over = false;

  for (int i = 0; i < 3; i++) {
    snake[i] = (struct v2) { i + 1, HEIGHT / 2 };
  }
  snake_len = 3;
  snake_head = 0;

  apple = (struct v2) { WIDTH - 5, HEIGHT / 2 }; 

  dir = (struct v2) { 1, 0 };

  // drawing
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      struct v2 p = { x, y };
      draw_tile(p, EMPTY);
    }
  }

  for (int i = 0; i < snake_len; i++) {
    draw_tile(snake[(snake_head + i) % SNAKE_CAP], SNAKE);
  }

  draw_tile(apple, APPLE);
};

void game_trigger(void) {
  if (game_over) {
    if (keyboard_get_next() == 'r') game_init();
    return;
  }

  switch (keyboard_get_next()) {
    case 'w':
      dir = (struct v2) { 0, -1 };
      break;
    case 'a':
      dir = (struct v2) { -1, 0 };
      break;
    case 's':
      dir = (struct v2) { 0, 1 };
      break;
    case 'd':
      dir = (struct v2) { 1, 0 };
      break;
  }

  struct v2 head = add(snake[(snake_head + snake_len - 1) % SNAKE_CAP], dir);

  if (in_body(head) || head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
    printf("[game] game over\n");
    game_over = true;
    return;
  }

  snake[(snake_head + snake_len) % SNAKE_CAP] = head;
  draw_tile(head, SNAKE);

  if (equals(head, apple)) {
    snake_len++;
    do {
      apple = (struct v2) { rand() % WIDTH, rand() % HEIGHT };
    } while (in_body(apple));
    draw_tile(apple, APPLE);
  } else {
    struct v2 tail = snake[snake_head];
    snake_head = (snake_head + 1) % SNAKE_CAP;
    draw_tile(tail, EMPTY);
  }
}
