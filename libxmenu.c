#include "xmenu.h"

#include <stdlib.h>
#include <string.h>

struct xmenu_Model {
  char **items;
  size_t count;
  size_t *visible;      // indices into items, ordered by filter match
  size_t visible_count; // number of entries in visible
  size_t selected;      // index into visible
  char *filter;
  size_t filter_cap;
};

xmenu_Model *xmenu_new(char **items, size_t count) {
  xmenu_Model *m = calloc(1, sizeof(*m));
  if (!m) return NULL;

  if (count) {
    m->items = malloc(count * sizeof(char *));
    if (!m->items) {
      free(m);
      return NULL;
    }
  }

  for (size_t i = 0; i < count; i++) {
    m->items[i] = strdup(items[i]);
    if (!m->items[i]) {
      for (size_t j = 0; j < i; j++) free(m->items[j]);
      free(m->items);
      free(m);
      return NULL;
    }
  }

  m->visible = malloc(count * sizeof(size_t));
  if (count && !m->visible) {
    for (size_t j = 0; j < count; j++) free(m->items[j]);
    free(m->items);
    free(m);
    return NULL;
  }

  m->count = count;
  for (size_t i = 0; i < count; i++) m->visible[i] = i;
  m->visible_count = count;
  return m;
}

void xmenu_free(xmenu_Model *m) {
  if (!m) return;
  for (size_t i = 0; i < m->count; i++) free(m->items[i]);
  free(m->items);
  free(m->visible);
  free(m->filter);
  free(m);
}

void xmenu_select_next(xmenu_Model *m) {
  if (m->selected + 1 < m->visible_count) m->selected++;
}

void xmenu_select_prev(xmenu_Model *m) {
  if (m->selected > 0) m->selected--;
}

void xmenu_filter(xmenu_Model *m, const char *query) {
  size_t need = strlen(query) + 1;
  if (need > m->filter_cap) {
    size_t cap = m->filter_cap ? m->filter_cap * 2 : 16;
    while (cap < need) cap *= 2;
    char *tmp = realloc(m->filter, cap);
    if (!tmp) return;
    m->filter = tmp;
    m->filter_cap = cap;
  }
  memcpy(m->filter, query, need);

  size_t prev = m->visible_count ? m->visible[m->selected] : 0;

  m->visible_count = 0;
  for (size_t i = 0; i < m->count; i++) {
    if (m->filter[0] == '\0' || strcasestr(m->items[i], m->filter))
      m->visible[m->visible_count++] = i;
  }

  m->selected = 0;
  for (size_t i = 0; i < m->visible_count; i++) {
    if (m->visible[i] == prev) {
      m->selected = i;
      break;
    }
  }
}

size_t xmenu_visible_count(const xmenu_Model *m) { return m->visible_count; }

const char *xmenu_visible_item(const xmenu_Model *m, size_t idx) {
  if (idx >= m->visible_count) return NULL;
  return m->items[m->visible[idx]];
}

size_t xmenu_visible_selected(const xmenu_Model *m) { return m->selected; }
