// TWEAK.H -- system dependend macros

#define MORGENGRAUEN

#if (defined(AVALON) || defined(FinalFrontier))
#  define UNItopia
#endif

// The home directory of the lisp compilers files
#if (defined(TAPPMUD) || defined(UNItopia))
#  define HOME(file)	"/w/hate/lisp/"+file
#elif (defined(ADAMANT))
#  define HOME(file)	"/wiz/hate/lisp/"+file
#else
# ifdef MORGENGRAUEN
#   define HOME(file)	"/obj/tools/lisp/"+file
# else
#   define HOME(file)	"/players/hate/lisp/"+file
# endif
#endif

// symbol_variable() is not available on drivers below 3.2.1@1
//#define symbol_variable(str)	0

// DEBUG mode
#ifdef MORGENGRAUEN
#  define USER "etah"
#else
#  define USER "hate"
#endif
#define DEBUG(x)	if(find_player(USER)) \
                          tell_object(find_player(USER), \
                                      sprintf("[%O] %O\n", this_object(), (x)))

