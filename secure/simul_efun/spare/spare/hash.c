#include "/sys/tls.h"

deprecated string md5(mixed arg, varargs mixed* iterations)
{
    if (extern_call())
         set_this_object(previous_object());

    return hash(TLS_HASH_MD5, arg, iterations...);
}

deprecated string sha1(mixed arg, varargs mixed* iterations)
{
    if (extern_call())
         set_this_object(previous_object());

    return hash(TLS_HASH_SHA1, arg, iterations...);
}
