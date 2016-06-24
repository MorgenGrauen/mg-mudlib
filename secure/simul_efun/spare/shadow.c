/* These sefuns are to provide a replacement for the efun query_shadowing()
 * and the old semantics of the efun shadow().
 * Feel free to add it to your mudlibs, if you have much code relying on that.
 */

#if ! __EFUN_DEFINED__(query_shadowing)

#include <object_info.h>

object query_shadowing(object ob)
{
    return efun::object_info(ob, OI_SHADOW_PREV);
}

private object _shadow(object ob, int flag)
{
    if(flag)
    {
        object shadower = efun::previous_object(1);
        efun::set_this_object(shadower);

        if (efun::shadow(ob))
            return efun::object_info(shadower, OI_SHADOW_PREV);
        else
            return 0;
    }
    else
        return efun::object_info(ob, OI_SHADOW_NEXT);
}
#else
private object _shadow(object ob, int flag)
{
  set_this_object(previous_object(1));
  return efun::shadow(ob, flag);
}
#endif


public object shadow(object ob, int flag)
{
  object res = funcall(#'_shadow,ob, flag);
  if (flag)
    "/secure/shadowmaster"->RegisterShadow(previous_object());
  return res;
}

private void _unshadow() {
  set_this_object(previous_object(1));
  efun::unshadow();
}

public void unshadow() {
  funcall(#'_unshadow);
  "/secure/shadowmaster"->UnregisterShadow(previous_object());
}
