// STACK.H -- stack macros

#define INITSTACK(s)	(s = ({}))
#define PUSH(s,x)	(s += ({ (x) }))
#define POP(s,x)	(x = s[<1], s = s[0..<2])
#define POPX(s)		(s = s[0..<2])
#define TOP(s)		((s)[<1])
#define SP(s)		(sizeof(s))
