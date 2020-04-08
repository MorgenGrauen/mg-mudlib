#pragma strict_types, rtt_checks, range_check, no_clone

#include <config.h>
#include <shells.h>
#include <defines.h>

// Ersetzt Platzhalter im Pfad und versorgt relative Pfade mit P_CURRENTDIR
// Funktionalitaet war mal im master() in der Rechtepruefung, gehoert da aber
// wirklich nicht hin.
string expand_path(string path, string user)
{
  string cwd;
  if (!sizeof(path))
    return path;
  switch(path[0])
  {
    // expand nur fuer nicht-absolute Pfade
    case '/':
      break;
    case '+':
      if(sizeof(path)==1)
        path="/"DOMAINDIR;
      path="/"DOMAINDIR"/" + path[1..];
      break;
    case '~':
      if (sizeof(path)==1)
      {
        if(user)
          path="/"WIZARDDIR"/"+user;
        else
          path="/"WIZARDDIR;
      }
      else
      {
        if(user && sizeof(path)>1 && path[1]=='/') // "~/"
          path="/"WIZARDDIR"/" + user + "/" + path[2..];
        else
          path="/"WIZARDDIR"/" + path[1..];
      }
      break;
    default:
      if(user && PL && getuid(PL) == user
          && (cwd=({string})PL->QueryProp(P_CURRENTDIR)))
        path=cwd + "/" + path;
  }
  return path;
}

// Besser benamste Version von _get_path() aus master().
// Normalisiert den Pfad, expandiert Platzhalter auf Wunsch, benutzt ggf. RPL
// oder PL als User, wenn nicht angegeben.
varargs string normalize_path(string path, string user, int expand) {
  if (!user && (RPL || PL))
    user = getuid(RPL || PL);
  if (expand)
    path=expand_path(path, user);
  return implode(({string*})master()->path_array(path), "/");
}

