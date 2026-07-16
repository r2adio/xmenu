#define TB_IMPL
#include "termbox2.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF          30
#define INITIAL_CAPACITY 16

char **get_inputs(size_t *out_count) {
  char **opts = NULL;
  size_t cap = INITIAL_CAPACITY;
  size_t count = 0;
  opts = malloc(cap * sizeof(char *));
  if (!opts) err(EXIT_FAILURE, "malloc failed");

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';

    if (count >= cap) {
      cap *= 2;
      char **tmp = realloc(opts, cap * sizeof(char *));
      if (!tmp) {
        free(line);
        free(opts);
        err(EXIT_FAILURE, "realloc failed");
      }
      opts = tmp;
    }

    opts[count] = strdup(line);
    count++;
  }
  free(line); // cleanup getline's buffer

  opts[count] = NULL;
  *out_count = count;
  return opts;
}

void free_inputs(char **inputs, size_t count) {
  for (size_t i = 0; i < count; i++)
    free(inputs[i]);
  free(inputs);
}

int main() {
  size_t input_count = 0;
  char **inputs = get_inputs(&input_count);

  if (input_count == 0) {
    free_inputs(inputs, input_count);
    return EXIT_FAILURE;
  }

  if (tb_init() != 0) {
    free_inputs(inputs, input_count);
    return EXIT_FAILURE;
  }

  char buf[MAX_BUF + 1] = {0};
  uint len = 0;
  uint selected = 0;

  while (1) {
    tb_clear();
    for (int i = 0; i < len; i++)
      tb_set_cell(i, 0, buf[i], TB_BLUE | TB_BOLD, TB_DEFAULT);

    int x = MAX_BUF + 1;
    for (size_t i = 0; i < input_count; i++) {
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
      else if (ev.key == TB_KEY_ARROW_RIGHT && selected + 1 < input_count)
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
  free_inputs(inputs, input_count);
  return EXIT_SUCCESS;
}
