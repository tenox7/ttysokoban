/* oldcurses.h - build ttysokoban with an old 4.2BSD style curses: no
 * colors, no attributes but standout, no keypad, no curs_set.
 * Made for the UNISCREEN curses of Whitesmiths Idris (1985).
 *
 * Arrow keys are read as VT52 (ESC A) or VT100 (ESC [ A, ESC O A)
 * sequences. Walls drawn in reverse video become solid blocks.
 *
 * oldcurses.c includes this file, then ttysokoban.c.
 */
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1
#endif

#define A_NORMAL	0
#define A_STANDOUT	1
#define A_REVERSE	2
#define A_BOLD		4
#define COLOR_PAIR(n)	0
#define COLOR_BLACK	0
#define COLOR_RED	1
#define COLOR_GREEN	2
#define COLOR_YELLOW	3
#define COLOR_BLUE	4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	6
#define COLOR_WHITE	7

#define has_colors()		FALSE
#define start_color()		OK
#define init_pair(p, f, b)	OK
#define keypad(w, f)		OK
#define curs_set(n)		OK
#define cbreak()		crmode()
#define getmaxyx(w, y, x)	((y) = (w)->lines, (x) = (w)->cols)

static int oc_attr = A_NORMAL;

static int oc_attrset(a)
	int a;
	{
	oc_attr = a;
	if (a & (A_STANDOUT | A_REVERSE))
		standout();
	else
		standend();
	return (OK);
	}

#define attron(a)	oc_attrset(oc_attr | (a))
#define attroff(a)	oc_attrset(oc_attr & ~(a))
#define attrset(a)	oc_attrset(a)

/* line drawing: a space in reverse video, else plain ASCII */
#define OC_LINE(c)	((oc_attr & A_REVERSE) ? ' ' : (c))
#define ACS_HLINE	OC_LINE('-')
#define ACS_VLINE	OC_LINE('|')
#define ACS_PLUS	OC_LINE('+')
#define ACS_LTEE	OC_LINE('+')
#define ACS_RTEE	OC_LINE('+')
#define ACS_TTEE	OC_LINE('+')
#define ACS_BTEE	OC_LINE('+')
#define ACS_ULCORNER	OC_LINE('+')
#define ACS_URCORNER	OC_LINE('+')
#define ACS_LLCORNER	OC_LINE('+')
#define ACS_LRCORNER	OC_LINE('+')

#define KEY_UP		0401
#define KEY_DOWN	0402
#define KEY_LEFT	0403
#define KEY_RIGHT	0404

static int oc_getch()
	{
	int c = wgetch(stdscr);

	if (c != 033)
		return (c);
	c = wgetch(stdscr);
	if (c == '[' || c == 'O')
		c = wgetch(stdscr);
	switch (c)
		{
	case 'A':
		return (KEY_UP);
	case 'B':
		return (KEY_DOWN);
	case 'C':
		return (KEY_RIGHT);
	case 'D':
		return (KEY_LEFT);
		}
	return (c);
	}

#undef getch
#define getch()		oc_getch()

/* clear() clears the terminal at once, then marks every cell of curscr
 * as unknown, so that the next refresh writes each cell itself. Without
 * this, UNISCREEN moves the cursor over blank cells by rewriting them
 * while standout is still on, and the floor between walls turns white.
 */
static int oc_clear()
	{
	int y, x;

	wclear(stdscr);
	wrefresh(stdscr);
	for (y = 0; y < curscr->lines; ++y)
		for (x = 0; x < curscr->cols; ++x)
			if (y < curscr->lines - 1 || x < curscr->cols - 1)
				curscr->room[y][x] = 0177;
	touchwin(stdscr);
	return (OK);
	}

#undef clear
#define clear()		oc_clear()

static char *oc_strdup(s)
	char *s;
	{
	char *p = malloc(strlen(s) + 1);

	if (p)
		strcpy(p, s);
	return (p);
	}

#define strdup(s)	oc_strdup((char *)(s))
