/*
 * MGtool-1.0
 * File: toollib.h
 * Maintainer: Kirk@MorgenGrauen
 */

/*------------------------------------------*/
/* the original Xtool is copyrighted by Hyp */
/*------------------------------------------*/

#ifndef __TOOLLIB_H__
#define __TOOLLIB_H__ 1

#ifndef MASTER
#define MASTER __MASTER_OBJECT__
#endif

#define is_obj(x)         ((x)&&objectp(x))

#define is_living(x)      (is_obj(x)&&living(x))
#define is_not_living(x)  (is_obj(x)&&!is_living(x))
#define is_netdead(x)     (is_obj(x)&&is_player(x)&&!interactive(x))
#define is_alive(x)       (is_obj(x)&&is_player(x)&&interactive(x))
#define is_snooped(x)     (is_obj(x)&&objectp(query_snoop(x)))
#define is_not_snooped(x) (is_obj(x)&&!is_snooped(x))
#define is_invis(x)       (is_obj(x)&&((x)->QueryProp(P_INVIS)||!(x)->QueryProp(P_SHORT))))
#define is_not_invis(x)   (is_obj(x)&&!is_invis(x))

#define pure_file_name(x)     (is_obj(x)?old_explode(object_name(x),"#")[0]:0)
#define source_file_name(x)   (is_obj(x)?pure_file_name((x))+".c":0)
#define string_replace(x,y,z) implode(explode((x),(y)),(z))

int string_compare(string a, string b);
string cap_string(string str);  
string short_path(string file);
string long_path(string file);
//string *old_explode(string str, string del);
//string *explode(string str, string del);
string *strip_explode(string str, string del);
string strip_string(string str);
string *long_get_dir(string pat, int all);
string lit_string(string str);
string mixed_to_string(mixed mix, int level);
int is_player(object obj);
int is_not_player(object obj);
int round(float val);

#endif /* __TOOLLIB_H__ */
