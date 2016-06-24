// * living_name-Behandlung

#define clean_log(s)
//#define clean_log(s) log_file("CLEAN_SIM",ctime(time())[4..18]+": "+(s));

private nosave mapping living_name_m, name_living_m, netdead;

private void InitLivingData(mixed wizinfo) {
  // living_name ist ein Pointer der auch in der extra_wizinfo gehalten 
  // wird, so kann die simul_efun neu geladen werden, ohne dass dieses
  // Mapping verloren geht
  if (!mappingp(living_name_m = wizinfo[LIVING_NAME]))
    living_name_m = wizinfo[LIVING_NAME] = m_allocate(0, 1);

  // Gleiches gilt fuer das Mapping Name-Living
  if (!mappingp(name_living_m = wizinfo[NAME_LIVING]))
    name_living_m = wizinfo[NAME_LIVING] = m_allocate(0, 1);

  // Netztote sind ebenfalls in der extra_wizinfo
  if (!mappingp(netdead = wizinfo[NETDEAD_MAP]))
    netdead = wizinfo[NETDEAD_MAP] = ([]);
}

public varargs string getuuid( object ob )
{
    mixed *ret;

    if ( !objectp(ob) )
       ob = previous_object();

    if ( !query_once_interactive(ob) )
       return getuid(ob);

    ret = (mixed)master()->get_userinfo( getuid(ob) );

    if ( !pointerp(ret) || sizeof(ret) < 5 )
       return getuid(ob);

    // Username + "_" + CreationDate
    return ret[0] + "_" + ret[5];
}

void set_object_living_name(string livname, object obj)
{
  string old;
  mixed a;
  int i;

  if (previous_object()==obj || previous_object()==this_object() ||
      previous_object()==master())
  {
    if(!livname || !stringp(livname)) {
      set_this_object(previous_object());
      raise_error(sprintf("%O: illegal living name: %O\n", obj, livname));
    }
    if (old = living_name_m[obj]) {
      if (pointerp(a = name_living_m[old])) {
       a[member(a, obj)] = 0;
      } else {
       m_delete(name_living_m, old);
      }
    }
    living_name_m[obj] = livname;
    if (a = name_living_m[livname]) {
      if (!pointerp(a)) {
       name_living_m[livname] = ({a, obj});
       return;
      }
      /* Try to reallocate entry from destructed object */
      if ((i = member(a, 0)) >= 0) {
       a[i] = obj;
       return;
      }
      name_living_m[livname] = a + ({obj});
      return;
    }
    name_living_m[livname] = obj;
  }
}

void set_living_name(string livname)
{
  set_object_living_name(livname,previous_object());
}

void remove_living_name()
{
  string livname;

  if (!previous_object())
    return;
  if (livname=living_name_m[previous_object()])
  {
    m_delete(living_name_m,previous_object());
    if (objectp(name_living_m[livname]))
    {
      if (name_living_m[livname]==previous_object())
       m_delete(name_living_m,livname);
      return;
    }
    if (pointerp(name_living_m[livname]))
    {
      name_living_m[livname]-=({previous_object()});
      if (!sizeof(name_living_m[livname]))
       m_delete(name_living_m,livname);
    }
  }
}

void _set_netdead()
{
  if (query_once_interactive(previous_object()))
    netdead[getuid(previous_object())]=previous_object();
}

void _remove_netdead()
{
  m_delete(netdead,getuid(previous_object()));
}

object find_netdead(string uuid)
{
  int i;
  string uid;
  // Wenn sscanf() 2 liefert, ist uuid auch ne uuid.
  int is_uuid = sscanf(uuid, "%s_%d", uid, i) == 2;
  if (!is_uuid)
    uid = uuid;
 
  if (is_uuid && getuuid(netdead[uid]) != uuid)
      return 0;

  return netdead[uid];
}

string *dump_netdead()
{
  return m_indices(netdead);
}

object find_living(string livname) {
  mixed *a, r;
  int i;

  if (pointerp(r = name_living_m[livname])) {
    if (member(r,0)>=0)
      r-=({0});
    if (!sizeof(r)){
      m_delete(name_living_m,livname);
      clean_log(sprintf("find_living loescht %s\n",livname));
      return 0;
    }
    if ( !living(r = (a = r)[0])) {
      for (i = sizeof(a); --i;) {
       if (living(a[<i])) {
         r = a[<i];
         a[<i] = a[0];
         return a[0] = r;
       }
      }
    }
    return r;
  }
  return living(r) && r;
}

object *find_livings(string livname)
{
  mixed r;

  if (pointerp(r=name_living_m[livname]))
    r-=({0});
  else
    if (objectp(r))
      r=({r});
  if (!pointerp(r)||!sizeof(r))
    return 0;
  return r;
}

object find_player(string uuid) {
  object *objs;
  mixed res;
  int i;
  string uid;

  if (!stringp(uuid))
    return 0;
  // Wenn sscanf() 2 liefert, ist uuid auch ne uuid.
  int is_uuid = sscanf(uuid, "%s_%d", uid, i) == 2;
  if (!is_uuid)
    uid = uuid;

  if (pointerp(res = name_living_m[uid])) {
    // zerstoerte Objekte ggf. entfernen
    if (member(res,0)>=0) {
      res-=({0});
      name_living_m[uid] = res;
    }
    // ggf. Namen ohne Objekte entfernen.
    if (!sizeof(res)){
      m_delete(name_living_m,uid);
      clean_log(sprintf("find_player loescht %s\n",uid));
      return 0;
    }
    objs = res;
    res = objs[0];
    // Wenn das 0. Element der Spieler ist, sind wir fertig. Ansonsten suchen
    // wir den Spieler und schieben ihn an die 0. Stelle im Array.
    if ( !query_once_interactive(res)) {
      for (i = sizeof(objs); --i;) {
       if (objs[<i] && query_once_interactive(objs[<i])) {
         res = objs[<i];
         objs[<i] = objs[0];
         objs[0] = res;
         break;
       }
      }
      res = (objectp(res) && query_once_interactive(res)) ? res : 0;
    }
    // else: in res steht der Spieler schon.
  }
  else if (objectp(res)) // r ist nen Einzelobjekt
    res = query_once_interactive(res) ? res : 0;

  // res ist jetzt ggf. der Spieler. Aber wenn der ne andere als die gesuchte
  // UUID hat, ist das Ergebnis trotzdem 0.
  if (is_uuid && getuuid(res)!=uuid)
      return 0;

  // endlich gefunden
  return res;
}

private int check_match( string str, int players_only )
{
    mixed match;

    if ( !(match = name_living_m[str]) )
       return 0;

    if ( !pointerp(match) )
       match = ({ match });

    match -= ({0});

    if ( sizeof(match) ){
       if ( players_only )
           return sizeof(filter( match, #'query_once_interactive/*'*/ ))
              > 0;
       else
           return 1;
    }

    m_delete( name_living_m, str );
    clean_log( sprintf("check_match loescht %s\n", str) );
    return 0;
}

//TODO:: string|string* exclude
varargs mixed match_living( string str, int players_only, mixed exclude )
{
    int i, s;
    mixed match, *user;

    if ( !str || str == "" )
       return 0;

    if ( !pointerp(exclude) )
       exclude = ({ exclude });

    if ( member(exclude, str) < 0 && check_match(str, players_only) )
       return str;

    user = m_indices(name_living_m);
    s = sizeof(str);
    match = 0;

    for ( i = sizeof(user); i--; )
       if ( str == user[i][0..s-1] && member( exclude, user[i] ) < 0 )
           if ( match )
              return -1;
           else
              if ( check_match(user[i], players_only) )
                  match = user[i];

    if ( !match )
       return -2;

    return match;
}

mixed *dump_livings()
{
  return sort_array(m_indices(name_living_m),#'>);
}

// * regelmaessig Listen von Ballast befreien
private void clean_name_living_m(string *keys, int left, int num)
{
  int i, j;
  mixed a;

  while (left && num--)
  {
    if (pointerp(a = name_living_m[keys[--left]]) && member(a, 0)>=0)
    {
      a-=({0});
      name_living_m[keys[left]] = a;
    }
    if (!a || (pointerp(a) && !sizeof(a)))
    {
      clean_log("Toasting "+keys[left]+"\n");
      m_delete(name_living_m, keys[left]);
    } else clean_log(sprintf("KEEPING %s (%O)\n",keys[left],pointerp(a)?sizeof(a):a));
  }
  if (left)
    efun::call_out(#'clean_name_living_m, 1, keys, left, 80);
  else
    clean_log("Clean process finished\n");
}

private void clean_netdead() {
  int i;
  string *s;
  object ob;

  s=m_indices(netdead);
  for (i=sizeof(s)-1;i>=0;i--)
    if (!objectp(ob=netdead[s[i]]) || interactive(ob))
      m_delete(netdead,s[i]);
}

private void CleanLivingData() {
  if (find_call_out(#'clean_name_living_m) < 0) {
    clean_log("Starting clean process\n");
    efun::call_out(#'clean_name_living_m,
                 1,
                 m_indices(name_living_m),
                 sizeof(name_living_m),
                 80
                 );
  }
  efun::call_out(#'clean_netdead,2);
}

#undef clean_log

