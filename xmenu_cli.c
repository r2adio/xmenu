#define TB_IMPL
#include "termbox2.h"
#include "xmenu.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF          30
#define INITIAL_CAPACITY 16

static char **read_items(size_t *out_count) {
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
  free(line);

  opts[count] = NULL;
  *out_count = count;
  return opts;
}

int main() {
  size_t count = 0;
  char **items = read_items(&count);
  if (count == 0) return EXIT_FAILURE;

  xmenu_Model *m = xmenu_new(items, count);
  if (!m) err(EXIT_FAILURE, "xmenu_new failed");

  for (size_t i = 0; i < count; i++) free(items[i]);
  free(items);

  if (tb_init() != 0) {
    xmenu_free(m);
    return EXIT_FAILURE;
  }

  char buf[MAX_BUF + 1] = {0};
  uint len = 0;

  while (1) {
    tb_clear();

    for (int i = 0; i < len; i++)
      tb_set_cell(i, 0, buf[i], TB_BLUE | TB_BOLD, TB_DEFAULT);

    int x = MAX_BUF + 1;
    for (size_t i = 0; i < m->count; i++) {
      if (i > 0) {
        tb_printf(x, 0, TB_BLACK, TB_WHITE, "|");
        x += 1;
      }
      if (i == m->selected)
        tb_printf(x, 0, TB_BLACK, TB_RED, " %s ", m->items[i]);
      else
        tb_printf(x, 0, TB_BLACK, TB_WHITE, " %s ", m->items[i]);
      x += strlen(m->items[i]) + 2;
    }

    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_ENTER) break;

      if (ev.key == TB_KEY_ARROW_LEFT)
        xmenu_select_prev(m);
      else if (ev.key == TB_KEY_ARROW_RIGHT)
        xmenu_select_next(m);

      if (ev.key == TB_KEY_BACKSPACE || ev.key == TB_KEY_BACKSPACE2 ||
          ev.key == TB_KEY_CTRL_H) {
        if (len > 0) buf[--len] = '\0';
      } else if (ev.ch != 0 && len < MAX_BUF) {
        buf[len++] = (char)ev.ch;
        buf[len] = '\0';
      }
    }
  }

  tb_shutdown();
  printf("input: %s\n", buf);
  printf("selected: %s\n", m->items[m->selected]);
  xmenu_free(m);
  return EXIT_SUCCESS;
}
