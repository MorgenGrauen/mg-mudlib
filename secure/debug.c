// MorgenGrauen MUDlib
/** \file /file.c
* Kurzbeschreibung.
* Langbeschreibung...
* \author <Autor>
* \date <date>
* \version $Id$
*/
/* Changelog:
*/
#pragma strict_types,save_types,rtt_checks
#pragma no_clone
#pragma no_inherit
#pragma no_shadow
#pragma pedantic
#pragma range_check

#include <functionlist.h>
#include <lpctypes.h>

#include <defines.h>
#include <wizlevels.h>

/** \def DEBUG
  Outputs debug message to Maintainer, if Mainteiner is logged in. 
*/
#ifndef DEBUG
#define DEBUG(x)  if (find_player("zesstra"))\
            tell_object(find_player("zesstra"),\
                                      "DDBG: "+x+"\n")
#endif

/** \fn set_object_next_reset(ob,zeit)
  \brief setzt den naechsten Reset auf 'zeit'
  \details setzt in Objekten den naechsten Reset - nur fuer EM+
  \param[in] ob
  (object) Objekt des Reset geaendert wird.
  \param[in] zeit
  (int) Zeit bis zum naechsten Reset.
  \return (string) Gibt die uebergebene Zeit bis zum bisherigen Reset.
  \author Zesstra
  \date 06.10.2007
  \sa set_object_heart_beat()
*/
// * Reset eines Objektes ein/ausschalten
int set_object_next_reset(mixed ob, int zeit) {

  if (stringp(ob))
    ob=find_object(ob);

  if (objectp(ob) && ELDER_SECURITY)
  //if (objectp(ob) && SPECIAL_SECURITY && !clonep(ob))
      return funcall(bind_lambda(#'efun::set_next_reset,ob),zeit);

  return -2;
}

mixed query_variable(object ob, string var)
{
  if (!previous_object() || !IS_ARCH(geteuid(previous_object())) 
      || !this_interactive() || !IS_ARCH(this_interactive())
      || getuid(ob)==ROOTID )
  {
    write("Du bist kein EM oder Gott!\n");
    return 0;
  }

  log_file("ARCH/QV", sprintf("%s: %O inquires var %s in %O\n",
                              ctime(time()),this_interactive(),var,ob));
  
  mixed res = variable_list(ob, RETURN_FUNCTION_NAME|RETURN_FUNCTION_FLAGS|
                                RETURN_FUNCTION_TYPE|RETURN_VARIABLE_VALUE);
  int index = member(res,var);
  if (index > -1)
  {
    return ({res[index],res[index+1],res[index+2],res[index+3]});
  }

  return 0;
}

protected void create() {
  // secure_level() in *_SECURITY() prueft auf die EUID
  seteuid(getuid(ME));
}

