// MorgenGrauen MUDlib
//
// thing/util.c -- Utilities
//
// $Id: util.c 6366 2007-07-15 21:06:24Z Zesstra $

#define NEED_PROTOTYPES
#include "/sys/thing/util.h"
#include "/sys/thing/properties.h"

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

public void ShowPropList(string *props) 
{
  int i,j;

  j=sizeof(props);

  for ( i=0; i<j ; i++) 
  {
    write("*"+props[i]+": ");
    PrettyDump(QueryProp(props[i]));
    write("\n");
  }
}

static void PrettyDump(mixed x) 
{
  if (pointerp(x)) 
  {
    DumpArray(x);
  }
  else if (mappingp(x))
  {
    DumpMapping(x);
  }
  else if (objectp(x)) 
  {
    write ("OBJ("+object_name(x)+")");
  }
  else if (stringp(x))
  {
    write("\""+x+"\"");
  }
  else
  {
    write (x);
  }
}

static void DumpArray(mixed *x) 
{
  int i,j;

  write ("({ ");
  if ( (j=sizeof(x))>0 )
  {
    for ( i=0 ; i<(j-1) ; i++) 
    {
      PrettyDump(x[i]);
      write(", ");
    }
    PrettyDump(x[i]);
    write(" ");
  }
  write ("})");
}

static void DumpMapping(mapping x)
{
  int   i, c, s;
  mixed *ind;

  write("([ ");

  if ( (c=sizeof(ind=m_indices(x)))<1 )
  {
    write(" ])");
    return;
  }

  s=get_type_info(x,1);

  DumpKeyValPair(x, ind[0], s);
  for ( i=1 ; i<c ; i++ )
  {
    write(", ");
    DumpKeyValPair(x, ind[i], s);
  }
  write(" ])");
}

// Lacht nicht ueber den Namen!!! -Boing
// Nein, ueber den Namen lache ich nicht ... -Paracelsus
static void DumpKeyValPair(mapping x, mixed key, int size)
{ int j, vc;

  PrettyDump(key);
  write(" : ");
  PrettyDump(x[key,0]);

  for ( j=1; j<size; j++)
  {
    write("; ");
    PrettyDump(x[key, j]);
  }
}
