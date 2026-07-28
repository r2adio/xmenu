#ifndef XMENU_H
#define XMENU_H

#include <stddef.h>

typedef struct {
  char **items;
  size_t count;
  size_t selected;
} xmenu_Model;

xmenu_Model *xmenu_new(char **items, size_t count);
void xmenu_free(xmenu_Model *m);

void xmenu_select_next(xmenu_Model *m);
void xmenu_select_prev(xmenu_Model *m);

#endif
