#define TB_IMPL
#include "termbox2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 30

int main() {
  if (tb_init() != 0) return EXIT_FAILURE;

  char buf[MAX_BUF + 1] = {0};
  int len = 0;

  while (1) {
    tb_clear();
    // NOTE: this loop set colors before user starts typing
    // for (int i = 0; i < MAX_BUF; i++)
    //   tb_set_cell(i, 0, ' ', TB_BLUE, TB_DEFAULT);
    for (int i = 0; i < len; i++)
      tb_set_cell(i, 0, buf[i], TB_BLUE | TB_BOLD, TB_DEFAULT);
    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_ENTER) {
        printf("print: %s", buf);
        break;
      }
      if (ev.key == TB_KEY_BACKSPACE || ev.key == TB_KEY_BACKSPACE2 ||
          ev.key == TB_KEY_CTRL_H)
      {
        if (len > 0) buf[--len] = '\0';
      } else if (ev.ch != 0 && len < MAX_BUF) {
        buf[len++] = (char)ev.ch;
        buf[len] = '\0';
      }
    }
  }

  tb_shutdown();
  return EXIT_SUCCESS;
}
