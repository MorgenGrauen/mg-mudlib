// MorgenGrauen MUDlib
//
// break_string.h - sym. Konstanten fuer break_string()
//
// $Id: defines.h 6415 2007-08-10 19:24:04Z Zesstra $

#ifndef _BREAK_STRING_H_
#define _BREAK_STRING_H_

#define BS_LEAVE_MY_LFS 	1
#define BS_SINGLE_SPACE         2
#define BS_BLOCK		4
#define BS_NO_PARINDENT		8
#define BS_INDENT_ONCE         16
#define BS_PREPEND_INDENT      32
// Falls hier Flags ergaenzt werden, sollten sie auch in living/comm.h fuer
// ReceiveMsg() eingetragen werden. ReceiveMsg() kann per se auch nur 10
// BS-Flags verarbeiten, das sollte beim Ergaenzen beachtet werden.

#endif /* _BREAK_STRING_H_ */

