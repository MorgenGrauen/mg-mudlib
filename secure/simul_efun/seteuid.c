/* This sefun is to provide a replacement for the efun seteuid().
 * Feel free to add it to your mudlibs, if you have much code relying on that.
 */

#if ! __EFUN_DEFINED__(seteuid)

#include <configuration.h>

int seteuid(string str)
{
    object|lwobject ob = efun::previous_object();
    if(objectp(ob))
    {
        if (!str)
        {
            efun::configure_object(ob, OC_EUID, 0);
            return 1;
        }

        if (efun::call_direct(__MASTER_OBJECT__, "valid_seteuid", ob, str) != 1)
            return 0;

        efun::configure_object(ob, OC_EUID, str);
        return 1;
    }
    else if (lwobjectp(ob))
    {
        if (!str)
        {
            efun::configure_lwobject(ob, LC_EUID, 0);
            return 1;
        }

        if (efun::call_direct(__MASTER_OBJECT__, "privilege_violation", 
                "configure_lwobject", ob, ob, LC_EUID, str) != 1)
            return 0;

        efun::configure_lwobject(ob, LC_EUID, str);
        return 1;

    }
    return 0;
}

#endif
