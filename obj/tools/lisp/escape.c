// ESCAPE.C -- expanding escape characters
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#pragma strong_types

string escape(string str)
{
  switch(str) 
  {
    case "\b" : return "\\b";
    case "\f" : return "\\f";
    case "\n" : return "\\n";
    case "\r" : return "\\r";
    case "\t" : return "\\t";
    case "\\" : return "\\\\";
  }
  return str;
}

string unescape(string str)
{
  switch(str) 
  {
    case "\\b" : return "\b";
    case "\\f" : return "\f";
    case "\\n" : return "\n";
    case "\\r" : return "\r";
    case "\\t" : return "\t";
    case "\\\\": return "\\";
    default   : if(str && sizeof(str)>1 && str[0]=='\\') 
                return str[1..]; 
  }
  return str;
}
