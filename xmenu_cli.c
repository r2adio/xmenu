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
  size_t cap = INITIAL_CAPACITY;
  size_t count = 0;
  char **opts = malloc(cap * sizeof(char *));
  if (!opts) err(EXIT_FAILURE, "malloc failed");

  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  while ((nread = getline(&line, &len, stdin)) != -1) {
    if (nread > 0 && line[nread - 1] == '\n') line[nread - 1] = '\0';

    if (count + 1 >= cap) {
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
  int status = EXIT_FAILURE;

  xmenu_filter(m, "");

  while (1) {
    tb_clear();

    int width = tb_width();
    for (int x = 0; x < width; x++) tb_set_cell(x, 0, ' ', DMENU_NORM_FG, DMENU_NORM_BG);

    tb_print(0, 0, DMENU_NORM_FG, DMENU_NORM_BG, buf);

    int x = width / 4;
    size_t nvis = xmenu_visible_count(m);
    size_t sel = xmenu_visible_selected(m);
    for (size_t i = 0; i < nvis; i++) {
      if (x >= width) break;

      char item_buf[256];
      snprintf(item_buf, sizeof(item_buf), " %s ", xmenu_visible_item(m, i));

      uintattr_t fg = (i == sel) ? DMENU_SEL_FG : DMENU_NORM_FG;
      uintattr_t bg = (i == sel) ? DMENU_SEL_BG : DMENU_NORM_BG;

      for (int j = 0; item_buf[j] != '\0' && x + j < width; j++)
        tb_set_cell(x + j, 0, item_buf[j], fg, bg);
      x += strlen(item_buf);
    }

    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC) break;
      if (ev.key == TB_KEY_ENTER) {
        status = EXIT_SUCCESS;
        break;
      }

      if (ev.key == TB_KEY_ARROW_LEFT) xmenu_select_prev(m);
      else if (ev.key == TB_KEY_ARROW_RIGHT) xmenu_select_next(m);

      if (ev.key == TB_KEY_CTRL_W) {
        while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';
        while (len > 0 && buf[len - 1] != ' ') buf[--len] = '\0';
        xmenu_filter(m, buf);
      } else if (ev.key == TB_KEY_CTRL_U) {
        len = 0;
        buf[0] = '\0';
        xmenu_filter(m, buf);
      } else if (ev.key == TB_KEY_BACKSPACE || ev.key == TB_KEY_BACKSPACE2 || ev.key == TB_KEY_CTRL_H) {
        if (len > 0) {
          buf[--len] = '\0';
          xmenu_filter(m, buf);
        }
      } else if (ev.ch != 0 && len < MAX_BUF) {
        buf[len++] = (char)ev.ch;
        buf[len] = '\0';
        xmenu_filter(m, buf);
      }
    }
  }

  tb_shutdown();
  if (status == EXIT_SUCCESS) {
    const char *out = NULL;
    if (xmenu_visible_count(m) > 0)
      out = xmenu_visible_item(m, xmenu_visible_selected(m));
    else if (len > 0)
      out = buf;
    if (out) printf("%s\n", out);
  }
  xmenu_free(m);
  return status;
}
