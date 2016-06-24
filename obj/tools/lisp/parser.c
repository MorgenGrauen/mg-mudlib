// PARSER.C -- parser for parenthesis expressions
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#pragma strong_types

#include "tweak.h" 

private inherit HOME("lex");

#include "stack.h"
#include "types.h"

private nosave mixed code;
private nosave mixed stack;
private nosave int quot, assume;

nomask static void initialize()
{
  INITSTACK(stack);
  code = ({});
  quot = 0;
}

static void create()
{
  lex::create();
  initialize();
}

nomask private mixed CodeGen(mixed token)
{
  switch(pre_parse(&code)) 
  {
  case 1: PUSH(stack, "'");          // standard autoquote
	  quot++;
	  break;
  case 2: PUSH(stack, "'CODE");          // special QUOTE for code generation
          break;	
  }
  if(stringp(token))
    switch(token)
    {
      case "'": PUSH(stack, token); quot++; return;
      case "(": PUSH(stack, code); code = ({}); break;
      case ")":
      {
        mixed tmp;
	if(SP(stack) && pointerp(TOP(stack))) POP(stack, tmp);
        else tmp = ({});
        if(SP(stack) && TOP(stack) == "'")
	  /* do nothing */ ;
        else
          if(sizeof(code))
	  {
	    code[0] = prog_parse(code[0]);
	    if(CL_LAMBDA(code[0]) || symbolp(code[0]) || pointerp(code[0]))
	      code = ({#'funcall}) + code;
	  }
        code = tmp + ({ code });
        break;
      }
      default : code += ({ token[1..<2] }); break;
    }
  else
    code += ({ quot ? token : prog_parse(token, !sizeof(code)) });

  while(SP(stack) && (TOP(stack) == "'"  || TOP(stack) == "'CODE") && 
        sizeof(code)) 
  { 
    int q;
    if(stringp(code[<1]) || symbolp(code[<1]) || pointerp(code[<1]) || 
       get_type_info(code[<1])[0] == 10) // quoted array
        code[<1] = quote(code[<1]); 
    POPX(stack); 
  }
}

// parse() -- non-threaded parsing (continue on input)
nomask static int parse(mixed msg, int interactive)
{
  mixed token, res;
  int lexing, missing_par;
  do {
    if(lexing = lex(&token))	
    {					// try to assume outer "("
      if(!SP(stack) && token != "(") { CodeGen("("); assume = 1; }
      CodeGen(token);
    }
    else 				// add ")" if "(" is assumed
      if(sizeof(stack) == 1 && assume) { CodeGen(")"); assume = 0; lexing = 1; }
    if(lexing && !SP(stack))		// complete expression found
    {
      mixed exec_code; exec_code = code; initialize();
      if(sizeof(exec_code) && pointerp(exec_code[0])) 
        exec_code = ({#',}) + exec_code;// more than one expr on one line
      if(msg = catch(res = eval(exec_code))) 
        return -2;			// execution error
      if(interactive) printf(output(res));
      else msg = res;
    }
  }
  while(lexing);
  if(missing_par = SP(stack)) 
    return (msg = sprintf("*Missing %d )\n", missing_par), -3); 
  return 0;
}
