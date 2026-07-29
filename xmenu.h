#pragma once

#include <stddef.h>

#define XMENU_FILTER_MAX 30

typedef struct {
  char **items;
  size_t count;
  size_t selected;
  char filter[XMENU_FILTER_MAX];
} xmenu_Model;

xmenu_Model *xmenu_new(char **items,
    size_t count);               // allocate and init xmenu model with the given items and count
void xmenu_free(xmenu_Model *m); // free the xmenu model and its allocated resources

void xmenu_select_next(xmenu_Model *m); // select next item
void xmenu_select_prev(xmenu_Model *m); // select previous item

void xmenu_filter(xmenu_Model *m, const char *query); // filter menu items wrt query

size_t xmenu_visible_count(const xmenu_Model *m); // return num of visible items at a momemt
const char *xmenu_visible_item(const xmenu_Model *m,
    size_t idx); // return the visible item at the specified index
size_t xmenu_visible_selected(const xmenu_Model *m); // returns index of selected visible item
