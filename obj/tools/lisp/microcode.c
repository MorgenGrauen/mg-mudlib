// MICROCODE.C -- lisp microcode
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#pragma strong_types
 
#include "tweak.h"
#include "types.h"

private nosave mapping memory;	// contains function information
private nosave string *G_input;	// global input stack
private nosave string *history;	// input history

nomask static public string output(mixed res);

#define CODE	0
#define QUOTE	1

static void create()
{
  memory = m_allocate(0, 2);
  G_input = ({});
}

nomask static closure get_function(string str)
{
  closure sym;
  if((sym = symbol_function(str, this_object())) ||     // FUNCTIONS
     (sym = symbol_function(str)) ||
     (sym = symbol_variable(str)))
    return sym;
#if defined(symbol_variable)
  if(str == "memory") return #'memory;
#endif
  return 0;
}

// pre_parse() -- parse code before examination by a code generator
nomask static public int pre_parse(mixed code)
{
  if(pointerp(code) && sizeof(code) &&
     (closurep(code[0]) || symbolp(code[0])))
  {
    mixed idx, tmp;
    if(sizeof(tmp = old_explode(sprintf("%O", code[0]), "->")) > 1)
      idx = tmp[1];
    else idx = code[0];
    // warning only the setting of bit 0 or 1 is allowed, not 0 and 1
    // thus 01 leads to normal quote, 10 for special, 11 not quote at all
    return (memory[idx, QUOTE] & (3<<2*(sizeof(code)-1)))>>2*(sizeof(code)-1);
  }
}

// prog_parse() -- examine a token and take actions
//
nomask varargs static public mixed prog_parse(mixed token, int func)
{
  if(symbolp(token))
      if(!member(memory, token))
        token = ({#'?, ({#'member, ({#'memory}), quote(token)}),
                     ({#'[, ({#'memory}), quote(token)}),
                     ({#'raise_error,
		       sprintf("Symbol %O' not bound\n", token) }) });
      else token = func ? token : memory[token];
  return token;
}

// evaluate compiled code
nomask static public mixed eval(mixed code)
{
  mapping sym;
  sym = filter_indices(memory, #'symbolp);
  return apply(lambda(m_indices(sym), code), m_values(sym));
}

// add new lines to input stack
nomask static public void add_input(string input) 
{ 
  if(!stringp(input)) return;
  G_input += old_explode(input, "\n"); 
}

// clear the input stack
nomask static public void clear_input()
{
  G_input = ({});
}

// get a line from input stack
// and save it into the history
nomask varargs string get_line()
{
  if(sizeof(G_input))
  {
    if(!pointerp(history)) history = ({});
    history += ({ G_input[0] });
    G_input[0..0] = ({});
    return history[<1];
  }
  return 0;  
}

// transform() -- transforms some of the output strings
nomask private string transform(string e)
{
  switch(e)
  {
  case "({": return "(";
  case "})": return ")";
  case "([": return "[";
  case "])": return "]";
  }
  if(strstr(e, " /* sizeof() == ") != -1) return "";
  if(strstr(e, "\n") != -1) if(e[0] == ',') return " "; else return "";
  return e;
}

// output() -- prints given data in a clean way
nomask static public string output(mixed res)
{
  res = regexplode(sprintf("%O", res),
                   "\n[ ]*|[,]\n[ ]*|[(][{\[]|[\]}][)]|[#][']|"
                  +" [/][*] sizeof[(][)] == [0-9][0-9]* [*][/]")
      - ({"\n", ",\n", "#'"});
  res = map(res, #'transform);
  return sprintf("%s\n", implode(res, ""));
}
