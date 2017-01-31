catch_tell()
============

SYNOPSIS
--------
::

	void catch_tell(string)

DESCRIPTION
-----------
::

	When a message is sent to a noninteractive player, via say(),
	tell_object, tell_room(), printf() or write(), it will get to the
	function catch_tell(string). This will enable communications between
	NPCs and from a player to an NPC.

	Also, if an interactive object is being shadowed and the
	shadow has catch_tell() defined, it will receive all output
	that would otherwise be written to the user.

	If a message is sent by an interactive object, catch_tell() is
	not called in that object, to prevent recursive calls. Thus
	catch_tell() in interactive objects can be used to filter the
	output that goes to the users.

	The efun shout() sends to interactive objects only.

SEE ALSO
--------
::

	enable_commands(E), say(E), tell_object(E), tell_room(E),
	write(E), catch_msg(L) 

