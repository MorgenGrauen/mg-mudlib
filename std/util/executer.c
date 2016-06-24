// MorgenGrauen MUDlib
//
// utils/executer.c - Helfer zum Ausfuehren vom Kram
//
// $Id: skills.c 6673 2008-01-05 20:57:43Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

protected mixed execute_anything(mixed fun, varargs mixed args)
{
  if ( closurep(fun) && objectp(query_closure_object(fun)) )
    return apply(fun, args);

  if (stringp(fun))
    return call_other(this_object(), fun, args...);

  if ( pointerp(fun) && sizeof(fun)==2 )
  {
    object ob;
    if (sizeof(fun)>2)
      raise_error(sprintf("execute_anything(): first argument may only "
                         "have 2 elements if array.\n"));

    if ( stringp(fun[0]) )
      ob=find_object(fun[0]);
    else
      ob=fun[0];

    if ( !objectp(ob) || !stringp(fun[1]) )
      return 0;

    return call_other(ob, fun[1], args...);
  }
  return 0;
}

