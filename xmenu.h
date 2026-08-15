#pragma once

#include <stddef.h>

typedef struct xmenu_Model xmenu_Model;

xmenu_Model *xmenu_new(char **items, size_t count); // allocate and init model, copying the items
void xmenu_free(xmenu_Model *m);                    // free the model and its resources

void xmenu_select_next(xmenu_Model *m); // move selection down within the visible items
void xmenu_select_prev(xmenu_Model *m); // move selection up within the visible items

void xmenu_filter(xmenu_Model *m, const char *query); // keep items that match query (case-insensitive substring)

size_t xmenu_visible_count(const xmenu_Model *m); // number of items visible after filtering
const char *xmenu_visible_item(const xmenu_Model *m,
    size_t idx);                      // item at visible index idx, NULL if out of range
size_t xmenu_visible_selected(const xmenu_Model *m); // visible index of the selected item
