#define TB_OPT_ATTR_W 32
#define TB_IMPL
#include "termbox2.h"
#include "xmenu.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF          30
#define INITIAL_CAPACITY 16

#define DMENU_NORM_FG    0xbbbbbb
#define DMENU_NORM_BG    0x222222
#define DMENU_SEL_FG     0xeeeeee
#define DMENU_SEL_BG     0x005577

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

  tb_set_output_mode(TB_OUTPUT_TRUECOLOR);

  char buf[MAX_BUF + 1] = {0};
  uint len = 0;

  while (1) {
    tb_clear();

    int width = tb_width();
    for (int x = 0; x < width; x++) tb_set_cell(x, 0, ' ', DMENU_NORM_FG, DMENU_NORM_BG);

    tb_print(0, 0, DMENU_NORM_FG, DMENU_NORM_BG, buf);

    // int x = len > 0 ? len + 2 : 2;
    int x = width / 4;

    for (size_t i = 0; i < m->count; i++) {
      if (x >= width) break;

      char item_buf[256];
      snprintf(item_buf, sizeof(item_buf), " %s ", m->items[i]);

      uintattr_t fg = (i == m->selected) ? DMENU_SEL_FG : DMENU_NORM_FG;
      uintattr_t bg = (i == m->selected) ? DMENU_SEL_BG : DMENU_NORM_BG;

      for (int j = 0; item_buf[j] != '\0' && x + j < width; j++)
        tb_set_cell(x + j, 0, item_buf[j], fg, bg);
      x += strlen(item_buf);
    }

    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_ENTER) break;

      if (ev.key == TB_KEY_ARROW_LEFT) xmenu_select_prev(m);
      else if (ev.key == TB_KEY_ARROW_RIGHT) xmenu_select_next(m);

      if (ev.key == TB_KEY_BACKSPACE || ev.key == TB_KEY_BACKSPACE2 || ev.key == TB_KEY_CTRL_H) {
        if (len > 0) buf[--len] = '\0';
      } else if (ev.ch != 0 && len < MAX_BUF) {
        buf[len++] = (char)ev.ch;
        buf[len] = '\0';
      }
    }
  }

  tb_shutdown();
  fprintf(stderr, "input: %s\n", buf);
  fprintf(stderr, "selected: %s\n", m->items[m->selected]);
  xmenu_free(m);
  return EXIT_SUCCESS;
}
