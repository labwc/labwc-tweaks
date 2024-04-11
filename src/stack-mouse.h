/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef STACK_MOUSE_H
#define STACK_MOUSE_H
#include <gtk/gtk.h>

struct state;

void stack_mouse_init(struct state *state, GtkWidget *stack);

#endif /* STACK_MOUSE_H */
