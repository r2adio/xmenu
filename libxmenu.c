#include "xmenu.h"

#include <stdlib.h>
#include <string.h>

xmenu_Model *xmenu_new(char **items, size_t count) {
  xmenu_Model *m = malloc(sizeof(xmenu_Model));
  if (!m) return NULL;

  m->items = malloc(count * sizeof(char *));
  if (!m->items) {
    free(m);
    return NULL;
  }

  for (size_t i = 0; i < count; i++)
    m->items[i] = strdup(items[i]);

  m->count = count;
  m->selected = 0;
  return m;
}

void xmenu_free(xmenu_Model *m) {
  if (!m) return;
  for (size_t i = 0; i < m->count; i++)
    free(m->items[i]);
  free(m->items);
  free(m);
}

void xmenu_select_next(xmenu_Model *m) {
  if (m->selected + 1 < m->count)
    m->selected++;
}

void xmenu_select_prev(xmenu_Model *m) {
  if (m->selected > 0)
    m->selected--;
}
