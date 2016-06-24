/* This sefun is to provide a replacement for the efun query_snoop().
 * Feel free to add it to your mudlibs, if you have much code relying on that.
 */


#include <interactive_info.h>

private object _query_snoop(object ob)
{
    if(!efun::interactive(ob))
        return 0;

    object prev = efun::previous_object();
    efun::set_this_object(prev);
#if ! __EFUN_DEFINED__(query_snoop)
    return efun::interactive_info(ob, II_SNOOP_NEXT);
#else
    return efun::query_snoop(ob);
#endif
}

nomask object query_snoop(object who) {
  object snooper;

  snooper=_query_snoop(who);
  if (!snooper) return 0;
  if (query_wiz_grp(snooper)>query_wiz_grp(getuid(previous_object())) &&
      IS_ARCH(snooper)) return 0;
  return snooper;
}
