/* This sefun is to provide a replacement for the efuns query_ip_name() and
 * query_ip_number().
 * Feel free to add it to your mudlibs, if you have much code relying on that.
 *
 * The code of the helper functions originates from the driver sources, they
 * are kept as-is intentionally.
 */

#if ! __EFUN_DEFINED__(query_ip_name)

#include <interactive_info.h>

// (actually not used in MG)
private varargs string _query_ip_name(object player)
{
    object ob = player;
    ob ||= efun::this_player();

    return efun::interactive_info(ob, II_IP_NAME);
}

private varargs string int_query_ip_number(object player)
{
    object ob = player;
    ob ||= efun::this_player();

    return efun::interactive_info(ob, II_IP_NUMBER);
}

// First tries to get the "real" IP (instead of the proxy's one), which was
// saved in the object by the login object. If that is unsuccessful, it falls
// back to the driver information of the IP the interactive object is
// connected to.
public string query_ip_number(object ob)
{
  ob = ob || this_player();
  if(objectp(ob) && interactive(ob))
  {
    string realip = ob->query_realip();
    if (sizeof(ob->query_realip()))
    {
      return realip;
    }
  }
  return int_query_ip_number(ob);
}

/* Liefert zu einer gegebenen ipnum den Hostnamen.
 * @param ipnum eine numerische ip-adresse oder ein interactive
 * @return den Hostnamen der zu der angegebenen ip-adresse gehoert.
 * wenn der hostname nicht bekannt ist, wird die ipadresse zurueckgegeben.
 */
public string query_ip_name(string|object ob)
{
  // First get the IP number (as string) for the object or  a 0.
  if ( !ob || objectp(ob) )
      ob=query_ip_number(ob);
  // then get its host name from the lookup & cache daemon.
  return "/p/daemon/iplookup"->host(ob);
}

#endif

