// MorgenGrauen MUDlib
//
// util/input.c -- generic input handling
//
// $Id: input.c 8349 2013-02-04 21:15:28Z Zesstra $

#include <input_to.h>

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

varargs void input(mixed prompt, mixed pargs, mixed ctrl, mixed ctrlargs)
{
  mixed prompttext;
  if(closurep(prompt))
    prompttext = apply(prompt, pointerp(pargs) ? pargs : ({}));
  else prompttext = prompt;
  if (!stringp(prompttext)) prompttext="";
  input_to("done", INPUT_PROMPT, prompttext, prompt, pargs, ctrl, ctrlargs);
}

void done(string in, mixed prompt, mixed pargs, mixed ctrl, mixed ctrlargs)
{
  if(closurep(ctrl) &&
     apply(ctrl, ({ in }) + (pointerp(ctrlargs) ? ctrlargs : ({})))) 
    return;
  input(prompt, pargs, ctrl, ctrlargs);
}
