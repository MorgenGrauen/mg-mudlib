// LEX.C -- creating tolkens from input string
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#pragma strong_types

#include "tweak.h"

private inherit HOME("microcode");
private inherit HOME("escape");

private nosave mixed *tokens;

static void create() 
{ 
  microcode::create();
  tokens = ({});
}

nomask private mixed make_symbol(string str)
{
  mixed sym;

  if(!stringp(str)) return str;
  if(sym = get_function(str))				// FUNCTIONS
    return sym;
  if(str == "list")
    return #'({;
  if(str[0] == '"')
    return implode(map(regexplode(str, "\\\\."), #'unescape), "");
  if((str[0] >= '0' && str[0] <= '9') ||                // NUMBERS
    ((str[0] == '+' || str[0] == '-') &&
     (str[1] >= '0' && str[1] <= '9')))
    if(member(str, '.') != -1)                          // FLOATS
      return to_float(str);
    else
      return to_int(str);                               // INTEGERS

  if(str == "(" || str == ")" || str == "'") 
    return str;

  return quote(str);            			// SYMBOLS
}

nomask private void get_tokens()
{
  mixed input, tmp;

  do {
    if(!input = get_line()) return 0;
    // splitting the input line in string tokens and removing comments
    tmp = regexp(regexplode(input,
                               "(\"(\\\\.|[^\\\\\"])*\")|[;].*|[() \t']"),
                    "^[^;]")
           - ({""," ","\t"});
    tokens += map(tmp, #'make_symbol);
  }
  while(!sizeof(tokens));
}

nomask static int lex(mixed token)
{
  if(!sizeof(tokens)) get_tokens();
  if(sizeof(tokens))
  {
    token = tokens[0];
    tokens[0..0] = ({});
    return 1;
  }
  return 0;
}
