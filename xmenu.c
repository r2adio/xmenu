#define TB_IMPL
#include "termbox2.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF          30
#define INITIAL_CAPACITY 16

char **get_inputs() {
  char **opts = NULL;
  size_t cap = INITIAL_CAPACITY;
  size_t count = 0;
  opts = malloc(cap * sizeof(char *));
  if (!opts) err(EXIT_FAILURE, "malloc failed");

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  // read stdin until eof
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';

    if (count >= cap) {
      cap *= 2;
      char **tmp = realloc(opts, cap * sizeof(char *));
      if (!tmp) {
        free(line);
        err(EXIT_FAILURE, "realloc failed");
      }
      opts = tmp;
    }

    opts[count] = strdup(line);
    count++;
  }
  free(line); // cleanup getline's buffer

  opts[count] = NULL;
  return opts;
}

int main() {
  if (tb_init() != 0) return EXIT_FAILURE;

  char buf[MAX_BUF + 1] = {0};
  int len = 0;
  int selected = 0;
  char **inputs = get_inputs();

  while (1) {
    tb_clear();
    // NOTE: this loop set colors before user starts typing
    // for (int i = 0; i < MAX_BUF; i++)
    //   tb_set_cell(i, 0, ' ', TB_BLUE, TB_DEFAULT);
    for (int i = 0; i < len; i++) // input box
      tb_set_cell(i, 0, buf[i], TB_BLUE | TB_BOLD, TB_DEFAULT);

    int x = MAX_BUF + 1;
    for (int i = 0; inputs[i] != NULL; i++) { // prints std inputs
      if (i > 0) {
        tb_printf(x, 0, TB_BLACK, TB_WHITE, "|");
        x += 1;
      }
      if (i == selected)
        tb_printf(x, 0, TB_BLACK, TB_RED, " %s ", inputs[i]);
      else
        tb_printf(x, 0, TB_BLACK, TB_WHITE, " %s ", inputs[i]);
      x += strlen(inputs[i]) + 2;
    }
    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_ENTER) break;
      if (ev.key == TB_KEY_ARROW_LEFT && selected > 0)
        selected--;
      else if (ev.key == TB_KEY_ARROW_RIGHT && inputs[selected + 1] != NULL)
        selected++;
      if (ev.key == TB_KEY_BACKSPACE || ev.key == TB_KEY_BACKSPACE2 || ev.key == TB_KEY_CTRL_H) {
        if (len > 0) buf[--len] = '\0';
      } else if (ev.ch != 0 && len < MAX_BUF) {
        buf[len++] = (char)ev.ch;
        buf[len] = '\0';
      }
    }
  }

  tb_shutdown();
  printf("input: %s\n", buf);
  printf("selected: %s\n", inputs[selected]);
  return EXIT_SUCCESS;
}
