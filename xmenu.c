#define TB_IMPL
#include "termbox2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 30

char **get_inputs() {
  static char *dummy_data[] = {"ghostty", "kitty", "gimp", NULL};
  return dummy_data;
}

int main() {
  if (tb_init() != 0) return EXIT_FAILURE;

  char buf[MAX_BUF + 1] = {0};
  int len = 0;

  while (1) {
    tb_clear();
    // NOTE: this loop set colors before user starts typing
    // for (int i = 0; i < MAX_BUF; i++)
    //   tb_set_cell(i, 0, ' ', TB_BLUE, TB_DEFAULT);
    for (int i = 0; i < len; i++) // input box
      tb_set_cell(i, 0, buf[i], TB_BLUE | TB_BOLD, TB_DEFAULT);

    int x = MAX_BUF + 1;
    char **inputs = get_inputs();
    for (int i = 0; inputs[i] != NULL; i++) { // prints std inputs
      if (i > 0) {
        tb_printf(x, 0, TB_BLACK, TB_WHITE, "|");
        x += 1;
      }
      tb_printf(x, 0, TB_BLACK, TB_WHITE, " %s ", inputs[i]);
      x += strlen(inputs[i]) + 2;
    }
    tb_set_cursor(len, 0);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);

    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_ENTER) break;
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
  printf("print: %s\n", buf);
  return EXIT_SUCCESS;
}
