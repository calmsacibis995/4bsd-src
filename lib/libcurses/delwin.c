# include	"curses.ext"

/*
 *	This routine deletes a window and releases it back to the system.
 */
delwin(win)
reg WINDOW	*win; {

	reg int	i;

	if (!(win->_flags & _SUBWIN))
		for (i = 0; i < win->_maxy && win->_y[i]; i++)
			cfree(win->_y[i]);
	cfree(win->_y);
	cfree(win);
}
