logon()
=======

SYNOPSIS
--------
::

	int logon(void)

DESCRIPTION
-----------
::

	When the parser accepts a new connection, it first calls
	connect() in the master object and then applies logon() to
	the object that is returned by the master. That will usually be
	a special login object, that is expected to clone and get going
	a user shell or player object.
	Should return 0 on failure, everything else means success.

SEE ALSO
--------
::

	connect(M)

