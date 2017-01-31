catch_msg()
===========

SYNOPSIS
--------
::

    void catch_msg(mixed *arr, object obj)

DESCRIPTION
-----------
::

    When say(), tell_object()  or tell_room() are used with an array 
    as message, the array will be passed to catch_message() in all living
    objects that can hear it, instead of writing to the user resp.
    sending to catch_tell(). This can be used to implement
    communication protocols between livings. The second denotes
    the object that has sent the message.

SEE ALSO
--------
::

 say(E), tell_room(E), tell_object(E), catch_tell(L)

