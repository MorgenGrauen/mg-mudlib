// MorgenGrauen MUDlib
//
// v_compiler.h -- a general virtual compiler object
//
// $Id: v_compiler.h 6081 2006-10-23 14:12:34Z Zesstra $

#ifndef __V_COMPILER_H__
#define __V_COMPILER_H__

#define P_STD_OBJECT         "std_object"
#define P_COMPILER_PATH      "compiler_path"

#endif // __V_COMPILER_H__

#ifdef NEED_PROTOTYPES

#ifndef __V_COMPILER_H_PROTO__
#define __V_COMPILER_H_PROTO__

// to validate if the file name is in our range
string Validate(string file);

// sets costumization data in the cloned object
mixed CustomizeObject();

// this functions is called when a file is not found and may be virtual
mixed compile_object(string file);

// to get all yet cloned (and not cleaned up) objects
mixed QueryObjects();

#endif // __V_COMPILER_H_PROTO__

#endif //NEED_PROTOTYPES
