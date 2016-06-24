// MorgenGrauen MUDlib
//
// pager.h -- generic pager header
//
// $Id: pager.h 6142 2007-01-31 20:34:39Z Zesstra $

#ifndef __PAGER_H__
#define __PAGER_H__

// select language
#define GERMAN
//#define ENGLISH

// select optimization
#define O_SPEED			// speed optimization
#define O_SPEED_UP		// additional speed up 
				// (works only with O_SPEED)
#define MAX_LINE_READ	625	// max. lines to be read by
				// read_file() 50k/80c

// the pager info structure
#define TEXT    0		// the filename or string
#define FILE    1		// 1 == file, 0 = string
#define CURL    2		// current line
#define MAXL    3		// last line
#define PAGE    4		// page length
#define REGX	5		// last regular expression
#define CTRL	6		// function to give control
#define CARG	7		// array of args to CTRL
#define FLAG	8		// special flags
# define E_PROMPT 1             //   prompt at end of file
# define E_CAT    2             //   do not prompt for more
# define E_ABS    4             //   show line numbers at prompt
#ifdef O_SPEED
# define JUNK		9	// junk index
# define JUNKSIZE	10	// size of string junks
#endif

#ifdef GERMAN
# define MSG_PROMPT	"--mehr--"
# define MSG_TOP	"ANFANG "
# define MSG_BOTTOM	"ENDE "
# define MSG_OOPS	"Ohh, da lief etwas schief!"
#else
# define MSG_PROMPT	"--more--"
# define MSG_TOP	"TOP "
# define MSG_BOTTOM	"BOTTOM "
# define MSG_OOPS	"Oops, something went wrong!"
#endif

#include <player/base.h>

#define PAGELENGTH	(this_interactive() ? \
			   this_interactive()->QueryProp(P_SCREENSIZE) : \
			   (interactive(this_player()) ? \
			     this_player()->QueryProp(P_SCREENSIZE) : 20))

// old_explode() must create an array that implode()'s to the same string
#define Explode(s,x)	(explode(s,x))

#endif // __PAGER_H__


