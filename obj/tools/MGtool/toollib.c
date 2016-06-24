/*
 * MGtool-1.0
 * File: toollib.c
 * Maintainer: Kirk@MorgenGrauen
 */

/*------------------------------------------*/
/* the original Xtool is copyrighted by Hyp */
/*------------------------------------------*/

#include "toollib.h"
#include <lpctypes.h>
#include <properties.h>
/*
string *old_explode(string str, string del)
{
  int s, t;
  string *strs;
  
  if(del == "")
    return ({str});
  strs=explode(str, del);
  s=0;
  t=sizeof(strs)-1;
  while(s<=t && strs[s++] == "")
    ;
  s--;
  while(t>=0 && strs[t--] == "")
    ;
  t++;
  if(s<=t)
    return strs[s..t];
  return 0;
}

string *explode(string str, string del)
{
  return explode(str, del);
}
*/
string *strip_explode(string str, string del)
{
  return explode(str, del)-({""});
}

string strip_string(string str)
{
  return implode(strip_explode(str," ")," ");
}

int string_compare(string a, string b)
{
  return a==b?0:(a>b?1:-1);
}

string cap_string(string str)
{
  return capitalize(str);
}

string short_path(string file)
{
  int s;
  string tmp;
  
  if(!file)
    return 0;
  if(PL)
  {
    if(file[0.. 8]=="/players/")
    {
      s=sizeof(getuid(PL))+8;
      return "~"+(file[9.. s]==getuid(PL) ?
		  file[s+1.. <1] : file[9.. <1]);
    }
  }
  if(file[0.. 2]=="/d/")
    return "+"+file[3.. <1];
  else
    return file;  
}

string long_path(string file)
{
  return (string)MASTER->make_path_absolute(file);
}

string *long_get_dir(string pat, int all)
{
  int i, s;
  string str, dir, *file, *tmp;
  
  if(!pat)
    return ({});
  pat=long_path(pat);
  if(tmp=old_explode(pat, "/"))
    dir="/"+implode(tmp[0..sizeof(tmp)-2], "/")+"/";
  else
    dir="/";
  s=sizeof(tmp=get_dir(pat));
  file=({});
  for(i=0;i<s;i++)
  {
    str=dir+tmp[i];
    if(all||file_size(str)>=0)
      file+=({str});
  }
  return file;
}

string lit_string(string str)
{
  str=string_replace(str, "\\", "\\\\");
  str=string_replace(str, "\b", "\\b");
  str=string_replace(str, "\n", "\\n");
  str=string_replace(str, "\r", "\\r");
  str=string_replace(str, "\t", "\\t");
  return string_replace(str, "\"", "\\\"");
}

string mixed_to_string(mixed mix, int lvl)
{
  int i, j, s, t;
  string str;
  mixed *keys;

  if(lvl)
  {
    switch(typeof(mix))
    {
      default:
      case T_INVALID:
      return "<invalid>";
      case T_STRUCT:
      return to_string(mix);
      case T_LVALUE:
      return "&"+mixed_to_string(mix, lvl-1);
      case T_NUMBER:
      case T_FLOAT:
      return to_string(mix);
      case T_STRING:
      return "\""+lit_string(mix)+"\"";
      case T_POINTER:
      return "({"+implode(map(mix,"mixed_to_string",ME,lvl-1),",")+"})";
      case T_OBJECT:
      return "["+short_path(object_name((object)mix))+"]";
      case T_MAPPING:
      s=sizeof(keys=m_indices(mix));
      t=get_type_info(mix, 1);
      str="([";
      for(i=0;i<s;i++)
      {
	str+=mixed_to_string(keys[i], lvl-1);
	if(t)
	{
	  str+=":"+mixed_to_string(mix[keys[i],0], lvl-1);
	  for(j=1;j<t;j++)
	    str+=";"+mixed_to_string(mix[keys[i],j], lvl-1);
	}
	if(i<s-1) str+=",";
      }
      return str+"])";
      case T_CLOSURE:
      case T_SYMBOL:
      return sprintf("%O", mix);
      case T_QUOTED_ARRAY:
      return "'"+mixed_to_string(funcall(lambda(0, mix)), lvl-1);
    }
  }
  return "...";
}

int is_player(object obj)
{
  return is_obj(obj)&&query_once_interactive(obj);
}

int is_not_player(object obj)
{
  return is_obj(obj)&&!is_player(obj);
}

int round(float val)
{
  int tmp;

  tmp=(int)val;
  // only positive val
  if( val - tmp >= 0.5 )
    return tmp+1;
  return tmp;
}
