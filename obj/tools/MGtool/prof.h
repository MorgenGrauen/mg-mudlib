private mixed __tm__=({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
private mixed __xtm__=({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
private mixed  __ec__=({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
private mixed  __xec__=({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
private int __level__;
private mapping __data__ = ([]);
__query_xprof_data__() { return __data__; }
#define U2T(ru) (((ru)[0]+(ru)[1])*10)
#define EC1 40
#define EC2 22
#define EC3 150
#define TM1 0
#define TM2 0
#define TM3 0
#define F(func,name,args) func args { \
  mixed *__dt__, *__ru__, __re__; \
  int __t__; \
  if(!__data__) return :: func args; \
  __ru__=rusage(); \
  __tm__[__level__]=(__t__=U2T(__ru__))<0?0:__t__; \
  __xtm__[__level__]=TM1; __tm__[__level__+1]=0; \
  __ec__[__level__]=get_eval_cost(); \
  __xec__[__level__]=EC1; __ec__[__level__+1]=0; \
  __level__++; __re__ = :: func args; __level__--; \
  if(!(__dt__=__data__[name])) { \
    __dt__=__data__[name]=({0,0,0,0,0}); \
    __xtm__[__level__]+=TM2; __xec__[__level__]+=EC2; \
  } \
  __ec__[__level__]-=get_eval_cost()+__xec__[__level__]; \
  __ru__=rusage(); \
  __t__=(__t__=U2T(__ru__))<0?0:__t__; \
  __tm__[__level__]=__t__-__tm__[__level__]-__xtm__[__level__]; \
  __dt__[0]++; \
  __dt__[1]+=__ec__[__level__]-__ec__[__level__+1]; \
  __dt__[2]+=__tm__[__level__]-__tm__[__level__+1]; \
  __dt__[3]+=__ec__[__level__]; __dt__[4]+=__tm__[__level__]; \
  if(__level__>0) { \
    __xec__[__level__-1]+=__xec__[__level__]+EC3; \
    __xtm__[__level__-1]+=__xtm__[__level__]+TM3; \
  } \
  return __re__; \
}
