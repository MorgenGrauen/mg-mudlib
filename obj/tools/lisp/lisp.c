// LISP.C -- Lisp2Closure compiler
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#pragma strong_types
 
#include "tweak.h"

private inherit HOME("parser");

static varargs mixed lisp(string input, int interactive, int in)
{
  string msg, error;
  add_input(input);					// put lines on stack
  switch(parse(&msg, interactive))
  { 
  case  0: return msg;					// ok
  case -1: clear_input(); return 0;			// empty list
  case -2: initialize();				// exec error
           clear_input();
           return (in ? (printf(msg), 0): (interactive ? msg : -1));
  case -3: if(interactive) 				// missing )
           {
             printf("%s? ", msg);
             input_to("lisp", 0, 1, 1);
             return -1;
           }
           else return (in ? (printf(msg), 0): (interactive ? msg : -1));
  default: if(!msg) msg ="*Unknown error occured\n";	// unknown error
           clear_input();
           return (in ? (printf(msg), 0): 
	                (interactive ? msg : -1));
  }
  return 0;
}

static void create()
{
  parser::create();
  lisp(read_file(__FILE__[0..<3]+".l"));
}

static void init(int arg) { }

