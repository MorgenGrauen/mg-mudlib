#define CL_LAMBDA(x)  (get_type_info(x)[0]==8 && get_type_info(x)[1]==5)
#define CL_LFUN(x)    (get_type_info(x)[0]==8 && get_type_info(x)[1]==0)
#define CL_EFUN(x)    (get_type_info(x)[0]==8 && get_type_info(x)[1]< 0)
#define CL_VAR(x)     (get_type_info(x)[0]==8 && get_type_info(x)[1]==1)
