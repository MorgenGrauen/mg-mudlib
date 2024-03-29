// MorgenGrauen MUDlib
//
// telnetneg.c -- Verwaltung von Telnet-Negotiations
//
// $Id$

/* Das Original wurde von Marcus@Tapp zur Verfuegung gestellt. */
/* Angepasst fuer die MG-Mudlib von Ringor@MG */
/* Weitgehend ueberarbeitet von Zesstra@MG */

#pragma strict_types,save_types
#pragma range_check
#pragma no_clone
#pragma no_shadow
#pragma pedantic

inherit "/secure/telnetneg-structs.c";

#define NEED_PROTOTYPES
#include "/secure/telnetneg.h"
#undef NEED_PROTOTYPES
#include <configuration.h>

// unterstuetzte Optionen:
// TELOPT_EOR, TELOPT_NAWS, TELOPT_LINEMODE, TELOPT_TTYPE, TELOPT_BINARY,
// TELOPT_CHARSET

//#define __DEBUG__ 1

#ifdef __DEBUG__
#define DEBUG(x)        if (interactive(this_object()))\
          tell_object(this_object(),"TN: " + x + "\n")
#define DTN(x,y) _debug_print(x,y)
#else
# define DEBUG(x)
# define DTN(x,y)
#endif

// first element "" to yield the separator
#define OFFERED_CHARSETS ({"", "UTF-8", "ISO8859-15", "LATIN-9", "ISO8859-1",\
                             "LATIN1", "WINDOWS-1252", "US-ASCII"})

// Aus mini_props.c:
public varargs mixed Query( string str, int type );
public varargs mixed Set( string str, mixed value, int type );

private nosave mapping TN = ([]);
nosave string *Terminals;

// Prototypen
private void eval_naws(int *optargs);

#ifdef __DEBUG__
// Gibts einige Konstanten mit sym. Namen aus.
private string dtranslate(int i) {
  switch(i) {
    case IAC: return "IAC";
    case DONT: return "DONT";
    case DO: return "DO";
    case WONT: return "WONT";
    case WILL: return "WILL";
    case SB: return "SB";
    case SE: return "SE";
    case EOR: return "EOR";
    case TELOPT_LINEMODE: return "TELOPT_LINEMODE";
    case TELOPT_XDISPLOC: return "TELOPT_XDISPLOC";
    case TELOPT_ENVIRON: return "TELOPT_ENVIRON";
    case TELOPT_NEWENV: return "TELOPT_NEWENV";
    case TELOPT_EOR: return "TELOPT_EOR";
    case TELOPT_NAWS: return "TELOPT_NAWS";
    case TELOPT_TSPEED: return "TELOPT_TSPEED";
    case TELOPT_TTYPE: return "TELOPT_TTYPE";
    case TELOPT_ECHO: return "TELOPT_ECHO";
    case TELOPT_SGA: return "TELOPT_SGA";
    case TELOPT_NAMS: return "TELOPT_NAMS";
    case TELOPT_STATUS: return "TELOPT_STATUS";
    case TELOPT_TM: return "TELOPT_TM";
    case TELOPT_BINARY: return "TELOPT_BINARY";
    case TELOPT_CHARSET: return "TELOPT_CHARSET";
    case TELOPT_COMPRESS2: return "TELOPT_COMPRESS2";
    case TELOPT_MSP: return "TELOPT_MSP";
    case TELOPT_MXP: return "TELOPT_MXP";
    case TELOPT_ATCP: return "TELOPT_ATCP";
    case TELOPT_GMCP: return "TELOPT_GMCP";
    case TELOPT_MSSP: return "TELOPT_MSSP";
  }
  return to_string(i);
}

// Gibt <arr> halbwegs lesbar an this_object() aus.
private void _debug_print(string x, int *arr) {
  if (sizeof(arr) >1 && arr[1] == SB && arr[<1] != SE)
    arr += ({IAC, SE});
  closure map_int = function string (int i)
    { if (i >= 32 && i <= 126) return sprintf("%c",i);
      return "["+to_string(i)+"]";
    };
  if (sizeof(arr)<=5) {
    foreach(int c : arr)
      x += " " + dtranslate(c);
  }
  else {
      x += dtranslate(arr[0]) + " " + dtranslate(arr[1]) + " "
           + dtranslate(arr[2]) + " "
           + implode(map(arr[3..<3], map_int)," ")
           + " " + dtranslate(arr[<2]) + " " + dtranslate(arr[<1]);
  }
  DEBUG(x);
}
#endif

protected varargs int send_telnet_neg(int *arr, int bm_flags)
{
    if ( sizeof(arr) < 2 )
        return efun::binary_message(arr,bm_flags);

    struct telopt_s opt = TN[arr[1]];

    switch (arr[0]){
    case DO:
    case DONT:
        (opt->lo_wishes)->remoteside = arr[0];
        arr = ({IAC}) + arr;
        break;
    case WILL:
    case WONT:
        (opt->lo_wishes)->localside = arr[0];
        arr = ({IAC}) + arr;
        break;
    case SB:
        (opt->lo_wishes)->sbdata = arr[0..];
        arr = ({IAC}) + arr + ({IAC, SE});
        break;
    default:
        break;
    }
    DTN("send_tn: ",arr);
    return efun::binary_message(arr, bm_flags);
}

protected varargs int send_telnet_neg_str(bytes str, int bm_flags)
{
#ifdef __DEBUG__
    // Debugausgaben zur Zeit nur fuer arraybasierte Variante
    return send_telnet_neg(to_array(str), bm_flags);
#else
    if ( sizeof(str) < 2 )
        return efun::binary_message(str, bm_flags);

    struct telopt_s opt = TN[str[1]];

    switch (str[0]) {
    case DO:
    case DONT:
        (opt->lo_wishes)->remoteside = str[0];
        str = to_bytes(({IAC})) + str;
        break;
    case WILL:
    case WONT:
        (opt->lo_wishes)->localside = str[0];
        str = to_bytes(({IAC})) + str;
        break;
    case SB:
        (opt->lo_wishes)->sbdata = to_array(str[1..]);
        str = to_bytes(({IAC})) + str + to_bytes(({IAC,SE}));
        break;
    default:
        break;
    }

    return efun::binary_message(str, bm_flags);
#endif // __DEBUG__
}

// Wenn der Client via STARTTLS eine TLS negotiation angestossen hat und
// die noch laeuft, darf keine Ausgabe erfolgen. In diesem Fall wird das
// Loginverfahren ausgesetzt, bis die TLS-Verhandlung abgeschlossen ist.
// Danach wird es fortgesetzt bzw. neugestartet. Dies gilt auch fuer Fall,
// dass STARTTLS verhandelt wurde, aber die TLS-Verhandlung noch nicht
// laeuft. (Bemerkung: beides pruefen ist nicht ueberfluessig. Den Zustand
// der Telnet-Option muss man pruefen, weil der Client evtl. seine
// Verhandlung noch nicht signalisiert hat (FOLLOWS vom Client) und die
// efun muss man pruefen, weil nach Empfang von FOLLOWS vom Client der
// Status der Telnet-Optiosn resettet wurde - standardkonform.)
protected int check_tls_negotiation()
{
  struct telopt_s s_tls = TN[TELOPT_STARTTLS];
  if (tls_query_connection_state(this_object()) < 0
      || (structp(s_tls) && s_tls->state->remoteside) )
  {
    debug_message("In TLS negotiation.\n");
    return 1;
  }
  return 0;
}

// Startet eine Verhandlung, um den Status einer Option zu aendern.
// Wenn bereits eine Verhandlung laeuft, wird nichts gemacht und -1
// zurueckgeben.
// Wenn die Verhandlung keine Aenderung vom Status quo zum Ziel hat, wird
// nichts gemacht und -2 zurueckgegeben.
// Ansonsten ist die Rueckgabe die Anzahl der uebermittelten Zeichen.
// <action>: WILL: Option soll auf dieser Seite eingeschaltet werden.
//           WONT: Option soll auf dieser Seite ausgeschaltet werden.
//           DO  : Option soll auf der anderen Seite eingeschaltet werden.
//           DONT: Option soll auf der anderen Seite ausgeschaltet werden.
protected int do_telnet_neg(int option, int action) {

  // ggf. muss TLS (initiiert durch STARTTLS) noch ausverhandelt werden. In
  // dem Fall nix verhandeln/senden, was nicht STARTTLS ist.
  if (option != TELOPT_STARTTLS && check_tls_negotiation())
    return 0;

  struct telopt_s opt = TN[option];
  if (!structp(opt))
  {
    opt = (<telopt_s> option: option,
                      re_wishes: (<to_state_s>),
                      lo_wishes: (<to_state_s>),
                      state: (<to_state_s>)
          );
    TN[option] = opt;
  }
  // es wird nur geprueft, ob wir bereits eine Verhandlung begonnen haben
  // (lo_wishes), weil reinkommende remote Wuensche letztendlich sofort durch
  // unsere Antwort erledigt sind.
  switch(action)
  {
    case WILL:
      if (opt->lo_wishes->localside != 0)
        return -1;
      if (opt->state->localside)
        return -2;
      return send_telnet_neg( ({ WILL, option }) );
    case WONT:
      if (opt->lo_wishes->localside != 0)
        return -1;
      if (!opt->state->localside)
        return -2;
      return send_telnet_neg( ({ WONT, option }) );
    case DO:
      if (opt->lo_wishes->remoteside != 0)
        return -1;
      if (opt->state->remoteside)
        return -2;
      return send_telnet_neg( ({ DO, option }) );
    case DONT:
      if (opt->lo_wishes->remoteside != 0)
        return -1;
      if (!opt->state->remoteside)
        return -2;
      return send_telnet_neg( ({ DONT, option }) );
  }
  raise_error(sprintf("Unsupported telnet negotation action in "
      "do_telnet_neg(): %d\n",action));
}

// LOCAL Standard Handlers //
private void _std_lo_handler_eor(struct telopt_s opt, int action) {
  // tatsaechlich nix zu tun. Handler ist nur da, damit die Option auf dieser
  // Seite aktiviert wird. Die Arbeit erledigt print_prompt.
  return;
}

private void _std_lo_handler_mssp(struct telopt_s opt, int action) {
  // nur einschalten ist interessant.
  if (action != LOCALON)
    return;
  // Krams senden, wenn Objekt geladen. Sonst wieder abschalten (kommt
  // hoffentlich nicht vor)...
  object mssp = find_object("/secure/misc/mssp");
  if (!mssp)
    send_telnet_neg( ({WONT, TELOPT_MSSP }) );
  else
  {
    send_telnet_neg_str(
        to_bytes(({SB, TELOPT_MSSP}))
        + to_bytes(sprintf("%s", ({string})mssp->get_telnegs_str()),
                   "ASCII//TRANSLIT"));
    // die Daten brauchen wir nicht mehr
    opt->lo_wishes->sbdata = 0;
  }
}


// REMOTE Standard Handlers //
private void _std_re_handler_tm(struct telopt_s opt, int action,
                                int *data)
{
  // egal, was geantwortet wurde, es gibt nen Hinweis auf die round-trip-time.
  // Wenn ein Array in opt->data[1] steht, rechnen wir das aus und schreiben es
  // in opt->data[0] als Ergebnis rein.
  if (pointerp(opt->data) && pointerp(opt->data[1]))
  {
    int *ut = utime();
    int *start = opt->data[1];
    int res = (ut[0] - start[0]) * 1000000;
    res += ut[1] - start[1];
    opt->data[0] = res;
    opt->data[1] = 0;
    DEBUG("RTT: "+res);
  }
  return;
}

private void _std_re_handler_naws(struct telopt_s opt, int action,
                                  int *data)
{
  if (action == SB)
  {
    eval_naws(data);
  }
}

private void _std_re_handler_linemode(struct telopt_s opt, int action,
                                      int *data)
{
  if (action == REMOTEON)
  {
    // see /doc/concepts/negotiations. We use only the minimum
    // needed for linemode: switching on local commandline-editing
    // for the client.
    send_telnet_neg(({ SB, TELOPT_LINEMODE, LM_MODE, MODE_EDIT }));
    // flush on 0d and 0a...
    // TODO: what does this exactly do?
    send_telnet_neg(({ SB, TELOPT_LINEMODE, DO, LM_FORWARDMASK, 0,
                          0x40|0x08 }));
    //Gna...
    opt->lo_wishes->sbdata = ({MODE_EDIT});
  }
}

private void _std_re_handler_ttype(struct telopt_s opt, int action,
                                   int *data)
{
  if (action == SB)
  {
    //TODO: get rid of this hysterical stuff...
    //NOTE: We do not do multiple SB SENDs due to some weird
    //bugs in IBM3270 emulating telnets which crash if we
    //do that.
    if ( sizeof(data) < 1 )
        return;

    if ( data[0] != TELQUAL_IS )
        return;

    string tmpterminal = lower_case( to_string(data[1..]) );
    if ( !Terminals )
        Terminals = ({ tmpterminal });
    else
        Terminals += ({ tmpterminal });

    if ( Query(P_TTY_TYPE) )
          Set( P_TTY_TYPE, Terminals[0] );
  }
  else if (action == REMOTEON)
  {
    send_telnet_neg(({ SB, TELOPT_TTYPE, TELQUAL_SEND }));
  }
}

// Der Handler fuer die BINARY option, wenn sie auf Clientseite
// aktiviert/deaktivert wird, d.h. der Client sendet jetzt Binaerdaten statt
// NVT-ASCII. Im Normalfall muessen wir im Handler nix machen. (SB gibts hier
// nicht.)
private void _std_re_handler_binary(struct telopt_s opt, int action,
                                   int *data)
{
  DTN("binary handler client",({action}));
}

// Der Handler fuer die BINARY option, wenn sie auf unserer Seite
// aktiviert/deaktivert wird, d.h. wir senden jetzt Binaerdaten statt
// NVT-ASCII. Im Normalfall muessen wir im Handler nix machen. (SB gibts hier
// nicht.)
private void _std_lo_handler_binary(struct telopt_s opt, int action,
                                   int *data)
{
  DTN("binary handler mg",({action}));
}

private int activate_charset(struct telopt_s opt, string charset)
{
  // Wenn der Client die Option nicht BINARY nicht unterstuetzt/will, duerfen
  // wir auch keine nicht-ASCII-Zeichen uebertragen. In diesem Fall ist der
  // einzige akzeptable Zeichensatz (US-)ASCII.
  struct telopt_s binary = TN[TELOPT_BINARY];
  if ( (!binary->state->remoteside || !binary->state->localside)
       && (upper_case(charset) != "US-ASCII"
          && upper_case(charset) != "ASCII") )
  {
    return 0;
  }
  // Wenn der Zeichensatz keine //-Variante ist, machen wir den zu
  // einer. Das verhindert letztlich eine Menge Laufzeitfehler, wenn ein
  // Zeichen mal nicht darstellbar ist.
  if (strstr(charset, "//") == -1)
    charset += "//TRANSLIT";
  // Falls das zu sehr scrollt, weil Clients staendig ungueltige/nicht
  // verwendbare Zeichensaetz schicken, muss das publish weg und ggf. sogar
  // ein nolog hin...
  if (!catch(configure_interactive(this_object(), IC_ENCODING, charset);
             publish))
  {
    m_delete(opt->data, "failed_negotiations");
    opt->data["accepted_charset"] = interactive_info(this_player(),
                                                     IC_ENCODING);
    return 1;
  }
  return 0;
}
#define REQUEST  1
#define ACCEPTED 2
#define REJECTED 3
#define TTABLE_IS 4
#define TTABLE_REJECTED 5
// Der Handler fuer die CHARSET option, wenn sie auf/fuer Clientseite
// aktiviert/deaktivert wird oder fuer empfangene SB.
private void _std_re_handler_charset(struct telopt_s opt, int action,
                                   int *data)
{
  DTN("charset handler client",({action}));

  // Wenn action == REMOTEON: Ab diesem Moment darf uns der Client einen
  // CHARSET REQUEST schicken (weil wir haben ihm auch schon ein DO
  // geschickt).
  if (action  == REMOTEON)
  {
    if (!mappingp(opt->data))
      opt->data = ([]);
  }
  else if (action == REMOTEOFF)
  {
    // Wenn auch auf mg-seite aus, kann data geloescht werden.
    if (!opt->state->localside)
      opt->data = 0;
  }
  else if (action == SB)
  {
    mapping statedata = opt->data;
    // <data> is the part following IAC SB TELOPT_CHARSET
    switch(data[0])
    {
      case REQUEST:
        // is the client allowed to REQUEST?
        if (opt->state->remoteside)
          return;
        // And enough data?
        if (sizeof(data) > 1 )
        {
          DTN("re_charset request:",data);
          string *suggestions = explode(to_text(data[2..], "ASCII"),
                                        sprintf("%c",data[1]));
          // Wenn UTF-8 drin vorkommt, nehmen wir das. (Gross-/Kleinschreibung
          // ist egal, aber wir muessen einen identischen String
          // zurueckschicken). (Gemischte Schreibweise: *ignorier* *stoehn*)
          string *selected = suggestions & ({"UTF-8","utf-8"});
          if (sizeof(selected)
              && activate_charset(opt, selected[0]))
          {
            send_telnet_neg(({ SB, TELOPT_CHARSET, ACCEPTED,
                               to_array(selected[0]) }));
            return;
          }
          else
          {
            // die ersten 10 Vorschlaege durchprobieren
            foreach(string cs : suggestions[0..min(sizeof(suggestions)-1, 10)])
            {
              if (activate_charset(opt, cs))
              {
                send_telnet_neg(({ SB, TELOPT_CHARSET, ACCEPTED,
                                   to_array(cs) }));
                return; // yeay, found one!
              }
            }
            // none acceptable
            send_telnet_neg(({ SB, TELOPT_CHARSET, REJECTED }));
            ++opt->data["failed_negotiations"];
            // fall-through, no return;
          }
        }
        else // malformed message
        {
          send_telnet_neg(({ SB, TELOPT_CHARSET, REJECTED }));
          ++opt->data["failed_negotiations"];
          // fall-through, no return;
        }
        // when arriving here, the negotiation was not successful. Check if
        // too many unsuccesful tries in a row.
        if (opt->data["failed_negotiations"] > 10)
        {
          send_telnet_neg(({ TELOPT_CHARSET, DONT }));
          send_telnet_neg(({ TELOPT_CHARSET, WONT }));
        }
        break;
      case ACCEPTED:
        // great - the client accepted one of our suggested charsets.
        // Negotiation concluded. However, check if we REQUESTed a charset in
        // the first place... And if the accepted one is one of our
        // suggestions
        if (sizeof(data) > 1)
        {
          DTN("re_charset accepted:",data);
          string charset = upper_case(to_text(data[1..], "ASCII"));
          string *offered = statedata["offered"];
          // in any case, we don't need the key in the future.
          m_delete(statedata, "offered");
          if (pointerp(offered) && member(offered, charset) > -1)
          {
            activate_charset(opt, charset);
            return;
          }
          // else: client did not sent us back one of our suggestions or we
          // did not REQUEST. :-(
        }
        ++opt->data["failed_negotiations"];
        // else? Huh. malformed message.
        break;
      case REJECTED:
        // none of our suggested charsets were acceptable. Negotiation is
        // concluded, we keep the current charset (and maybe we will receive a
        // suggestion of the client)
        if (member(statedata, "offered"))
          m_delete(statedata, "offered");
        ++opt->data["failed_negotiations"];
        DTN("re_charset_rejected:",data);
        break;
      case TTABLE_IS:
        // we plainly don't support TTABLES
        send_telnet_neg(({ SB, TELOPT_CHARSET, TTABLE_REJECTED }));
        ++opt->data["failed_negotiations"];
        break;
    }
  }
}

// Der Handler fuer die CHARSET option, wenn sie auf/fuer unserere Seite
// aktiviert/deaktivert wird.
private void _std_lo_handler_charset(struct telopt_s opt, int action,
                                   int *data)
{
  DTN("charset handler mg",({action}));
  if (action == LOCALON)
  {
    // Ab diesem Moment duerfen wir dem Client einen CHARSET REQUEST schicken
    // (denn wir haben auch schon ein DO erhalten). Und das tun wir auch
    // direkt.
    if (!mappingp(opt->data))
      opt->data = ([ "offered": OFFERED_CHARSETS ]);
    else
      opt->data["offered"] = OFFERED_CHARSETS;
    send_telnet_neg(({ SB, TELOPT_CHARSET, REQUEST })
                    + to_array(implode(opt->data["offered"], ";"))) ;
  }
  else if (action == LOCALOFF)
  {
    // ok, keine REQUESTS mehr nach dem LOCALOFF, aber viel muss nicht getan
    // werden. Wenn auch auf client-seite aus, kann data geloescht werden.
    if (!opt->state->remoteside)
      opt->data = 0;
  }
  // und SB gibt es nicht in diesem Handler.
}
#undef REQUEST
#undef ACCEPTED
#undef REJECTED
#undef TTABLE-IS
#undef TTABLE-REJECTED

// Called from the telnetneg handler for TELOPT_STARTTLS to initiate the TLS
// connection negotiation.
protected void init_tls()
{
  // Dabei muss unser ganzer Telnet-Option-State muss zurueckgesetzt werden.
  // Ja, wirklich! (Keine Sorge, der client muss das auch tun.)
  TN = ([]);
}

#ifdef __TLS__
// Der Handler fuer STARTTLS, wenn es auf der Clientseite
// deaktiviert/aktiviert wird. Es wird nur auf der Clientseite aktiviert, der
// Server darf kein WILL senden. Nach Aktivierung muessen wir ein FOLLOWS
// senden.
#define FOLLOWS 1
private void _std_re_handler_starttls(struct telopt_s opt, int action,
                                      int *data)
{
  DTN("starttls handler client",({action}));

  // Wenn action == REMOTEON: Ab diesem Moment darf uns der Client einen
  // STARTTLS FOLLOWS senden (weil wir haben ihm auch schon ein DO
  // geschickt). Wir sollen ihm aber jetzt auch ein FOLLOWS senden. Sobald wir
  // das gesendet haben und ein FOLLOWS erhalten haben, geht die Negotiation
  // los.
  if (action  == REMOTEON)
  {
    send_telnet_neg(({ SB, TELOPT_STARTTLS, FOLLOWS }));
    opt->data = 1; // Nur ein Flag, dass wir es gesendet haben.
  }
  else if (action == REMOTEOFF)
  {
    // data zuruecksetzen, sonst muessen wir nix machen.
    opt->data = 0;
  }
  else if (action == SB)
  {
    if (data[0] == FOLLOWS)
    {
      // FOLLOWS empfangen. Wenn wir noch kein FOLLOWS gesendet haben, tun wir
      // das jetzt.
      if (!opt->data)
        send_telnet_neg(({ SB, TELOPT_STARTTLS, FOLLOWS }));
      // Jetzt wird die Verhandlung auf unserer Seite gestartet, der Client
      // macht das entweder schon oder spaetestens, wenn er unser FOLLOWS
      // empfangen kann.
      init_tls();
    }
  }
}
#undef FOLLOWS
#endif // __TLS__

// Bindet/registriert Handler fuer die jew. Telnet Option. (Oder loescht sie
// auch wieder.) Je nach <initneg> wird versucht, die Option neu zu
// verhandeln.
protected int bind_telneg_handler(int option, closure re, closure lo,
                                  int initneg)
{
  struct telopt_s opt = TN[option];
  if (!structp(opt))
  {
    opt = (<telopt_s> option: option,
                      re_wishes: (<to_state_s>),
                      lo_wishes: (<to_state_s>),
                      state: (<to_state_s>)
          );
    TN[option] = opt;
  }

  opt->remotehandler = re;
  if (initneg)
  {
    if (re)
      do_telnet_neg(option, DO);
    else
      do_telnet_neg(option, DONT );
  }

  opt->localhandler = lo;
  if (initneg)
  {
    if (lo)
      do_telnet_neg(option, WILL);
    else
      do_telnet_neg(option, WONT);
  }
  return 1;
}


// Mal unsere Wuensche an den Client schicken und die Standardhandler
// registrieren. Hierbei bei Bedarf neue Verhandlungen starten. Es wird hier
// aber nur ein Basissatz an Optionen verhandelt, der Rest kommt spaeter
// nachdem das Spielerobjekt die Verbindung hat (in startup_telnet_negs())
// Gerufen aus login.c nach Verbindungsaufbau.
// Bemerkung: das Spielerobjekt bietet evt. noch zusaetzliche Telnetoptionen
//            an, die dann ueber startup_telnet_negs() (im Spielerobjekt)
//            laufen.
protected void SendTelopts()
{
#if __TLS__
  // If this is a non-TLS-connection, we offer STARTTLS, but wait for the
  // client to ask for it.
  if (tls_available() && tls_query_connection_state() == 0)
  {
    bind_telneg_handler(TELOPT_STARTTLS, #'_std_re_handler_starttls,
                        0, 1);
  }
#endif
  bind_telneg_handler(TELOPT_BINARY, #'_std_re_handler_binary,
                      #'_std_lo_handler_binary, 1);
  bind_telneg_handler(TELOPT_EOR, 0, #'_std_lo_handler_eor, 1);
  bind_telneg_handler(TELOPT_NAWS, #'_std_re_handler_naws, 0, 1);
  bind_telneg_handler(TELOPT_LINEMODE, #'_std_re_handler_linemode, 0, 1);
  bind_telneg_handler(TELOPT_TTYPE, #'_std_re_handler_ttype, 0, 1);
  if (find_object("/secure/misc/mssp"))
    bind_telneg_handler(TELOPT_MSSP, 0, #'_std_lo_handler_mssp, 1);
  // und auch CHARSET wird verzoegert bis das Spielerobjekt da ist.
}

// Bindet die Standardhandler _aus diesem_ Programm (und ueberschreibt dabei
// ggf. andere). Hierbei werden nur die Handler neu gebunden, keine neuen
// Verhandlungen initiiert.
// gerufen aus base.c indirekt via startup_telnet_negs().
protected void _bind_telneg_std_handlers()
{
  // BTW: es ist absicht, im Spielerobjekt keinen Support fuer STARTTLS mehr
  // anzubieten.
  bind_telneg_handler(TELOPT_BINARY, #'_std_re_handler_binary,
                      #'_std_lo_handler_binary, 0);
  bind_telneg_handler(TELOPT_EOR, 0, #'_std_lo_handler_eor, 0);
  bind_telneg_handler(TELOPT_NAWS, #'_std_re_handler_naws, 0, 0);
  bind_telneg_handler(TELOPT_LINEMODE, #'_std_re_handler_linemode, 0, 0);
  bind_telneg_handler(TELOPT_TTYPE, #'_std_re_handler_ttype, 0, 0);
  // Besondere Situation: MSSP ist nach Spielerlogin eigentlich uninteressant.
  // Daher sparen wir uns das im Kontext des Spielerobjekts und schalten es
  // einfach wieder aus.
  bind_telneg_handler(TELOPT_MSSP, 0, 0, 0);
}


// Ruft die entsprechenden handler von der Telnet Option.
// Wenn es keinen handler (mehr) gibt, wird die Option auch auf der jeweiligen
// Seite ausgeschaltet. Deshalb MUSS lo_wishes und re_wishes vom Aufrufer VOR
// DEM AUFRUF zurueckgesetzt worden sein!
// <action>: 'LOCALON':   Option wurde auf unserer Seite eingeschaltet
//           'LOCALOFF':  Option wurde auf unserer Seite ausgeschaltet
//           'REMOTEON':  Option wurde auf Clientseite eingeschaltet
//           'REMOTEOFF': Option wurde auf Clientseite ausgeschaltet
//           'SB':        Suboption negotiation Daten wurden empfangen
// <data>: die per SB empfangenen Daten (unverarbeitet)
private void _call_handler(struct telopt_s opt, int action, int *data) {
  switch(action)
  {
    case REMOTEON:
    case REMOTEOFF:
    case SB:
      if (opt->remotehandler)
      {
        funcall(opt->remotehandler, opt, action, data);
      }
      else
      {
        // ok, geht nicht. Ggf. Abschalten (do_telnet_neg passt auf,
        // dass nur verhandelt wird, wenn die Option an ist.)
        do_telnet_neg( opt->option, DONT );
      }
      break;
    case LOCALON:
    case LOCALOFF:
      if (opt->localhandler)
      {
        funcall(opt->localhandler, opt, action);
      }
      else
      {
      // ok, geht nicht. Ggf. Abschalten (do_telnet_neg passt auf,
      // dass nur verhandelt wird, wenn die Option an ist.)
        do_telnet_neg( opt->option, WONT );
      }
      break;
  }
}

// Gerufen vom Driver, wenn neue telnet options reinkommen.
void
telnet_neg(int command, int option, int *optargs)
{
    DTN("recv_tn: ", ({IAC, command, option}) + (optargs||({})));

    // ggf. muss TLS (initiiert durch STARTTLS) noch ausverhandelt werden. In
    // dem Fall muessen wir alles ignorieren, was nicht STARTTLS ist.
    if (option != TELOPT_STARTTLS && check_tls_negotiation())
      return;

    struct telopt_s opt = TN[option];
    if (!structp(opt))
    {
      opt = (<telopt_s> option: option,
                        re_wishes: (<to_state_s>),
                        lo_wishes: (<to_state_s>),
                        state: (<to_state_s>)
            );
      TN[option] = opt;
    }

    // Was will der Client tun?
    if (command == WONT)
    {
        // Client will die Option auf seiner Seite abschalten. Wir MUESSEN das
        // akzeptieren.
        // Wir muessen das allerdings ignorieren, wenn die Option bereits aus
        // ist.
        if (opt->state->remoteside==0)
        {
          // Ausnahme fuer TELOPT_TM, da das kaum ein Client kann und fuer RTT
          // es eigentlich auch egal ist, was zurueck kommt: der handler wird
          // zumindest doch gerufen zum Ausrechnen der RTT
          if (option == TELOPT_TM)
            _call_handler(opt, REMOTEOFF, 0);
          // ansonsten aber wirklich ignorieren. ;)
          return;
        }
        opt->re_wishes->remoteside = command;
        // Bestaetigung auf ein WONT senden, wenn wir nicht selber schon ein
        // DONT geschickt hatten.
        if (opt->lo_wishes->remoteside != DONT) {
          send_telnet_neg( ({DONT, option}) );
        }
        // Wir haben jetzt auf jeden Fall ein DONT gesendet und ein WONT
        // erhalten. Damit ist die Option jetzt auf der clientseite aus.
        // Ausserdem setzen wir die Wishes zurueck.
        opt->re_wishes->remoteside = 0;
        opt->lo_wishes->remoteside = 0;
        if (opt->state->remoteside != 0)
        {
            opt->state->remoteside = 0;
            _call_handler(opt, REMOTEOFF, 0);
        }
    } // WONT vom Client verarbeitet
    else if ( command == WILL)
    {
        // Wenn die Option bereits an ist, muessen wir dies ignorieren.
        if (opt->state->remoteside == 1)
        {
          // Ausnahme fuer TELOPT_TM, der handler wird zumindest doch gerufen
          // zum Ausrechnen der RTT. Diese Option laesst sich ohnehin
          // aktivieren, auch wenn sie schon an ist.
          if (option == TELOPT_TM)
            _call_handler(opt, REMOTEON, 0);
          // sonst aber wirklich ignorieren. ;-)
          return;
        }
        opt->re_wishes->remoteside = command;
        if ( opt->lo_wishes->remoteside == 0 )
        {
            // Der Client will, wir haben noch nix dazu gesagt. (Mit unserer
            // Antwort ist die Verhandlung uebrigens beendet.)
            // Wenn es einen remotehandler fuer die Option gibt, schalten wir
            // sie ein...
            if (opt->remotehandler)
            {
                send_telnet_neg(({DO, option}));
                // Option jetzt an der Clientseite an.
                opt->re_wishes->remoteside = 0;
                opt->lo_wishes->remoteside = 0;
                if (opt->state->remoteside != 1)
                {
                    opt->state->remoteside = 1;
                    _call_handler(opt, REMOTEON, 0);
                }
            }
            else
            {
              // sonst verweigern wir das einschalten (die meisten Optionen
              // auf Clientseite sind fuer uns eh egal).
              send_telnet_neg(({DONT, option}));
              // Option jetzt an der Clientseite aus.
              opt->re_wishes->remoteside = 0;
              opt->lo_wishes->remoteside = 0;
              if (opt->state->remoteside != 0)
              {
                  opt->state->remoteside = 0;
                  _call_handler(opt, REMOTEOFF, 0);
              }
            }
        }
        else if ( opt->lo_wishes->remoteside == DO)
        {
            // Wir haben haben bereits per DO angefordert, d.h. das ist die
            // Clientbestaetigung - wir duerfen nicht bestaetigen und die
            // Option ist jetzt clientseitig aktiv. Verhandlung beendet.
            opt->re_wishes->remoteside = 0;
            opt->lo_wishes->remoteside = 0;
            if (opt->state->remoteside != 1)
            {
                opt->state->remoteside = 1;
                _call_handler(opt, REMOTEON, 0);
            }
        } // if (DO)
        else {
          // Mhmm. Wir hatten ein DONT gesendet, aber der Client hat mit WILL
          // geantwortet. Das darf er eigentlich gar nicht.
          //TODO: was sollte man jetzt eigentlich tun? Erstmal wiederholen wir
          //das DONT...
          send_telnet_neg( ({DONT, option}) );
        }

        return;
    } // WILL vom Client verarbeitet
    // Was sollen wir (nicht) fuer den Client tun?
    else if ( command == DONT)
    {
        // Client will, dass wir etwas nicht tun. Wir MUESSEN das akzeptieren.
        // wenn die Option auf unserer Seite aber schon aus ist, muessen wir
        // dies ignorieren.
        if (opt->state->localside == 0)
          return;

        opt->re_wishes->localside = command;
        // Wenn wir noch kein WONT gesendet haben, senden wir das jetzt als
        // Bestaetigung.
        if (opt->lo_wishes->localside = WONT)
            send_telnet_neg( ({WONT, option}) );
        // Verhandlung beendet, Option is auf unserer Seite jetzt aus.
        // Wuensche auch wieder zuruecksetzen.
        opt->re_wishes->localside = 0;
        opt->lo_wishes->localside = 0;
        if (opt->state->localside != 0)
        {
          opt->state->localside = 0;
          _call_handler(opt, LOCALOFF, 0);
        }
    }
    else if ( command == DO )
    {
        // Client will, dass wir option tun. Mal schauen, wie wir dazu stehen.
        // wenn die Option auf unserer Seite aber schon an ist, muessen wir
        // dies ignorieren.
        if (opt->state->localside == 1)
          return;

        opt->re_wishes->localside = command;

        if ( opt->lo_wishes->localside == 0 ) {
            // wir haben unsere Wuensche noch nicht geaeussert. Sobald wir
            // bestaetigen, ist die Option auf unserer Seite an/aus und die
            // Verhandlungen beendet.
            // in jedem Fall die Wuensche zuruecksetzen
            opt->re_wishes->localside = 0;
            opt->lo_wishes->localside = 0;
            if (opt->localhandler)
            {
                send_telnet_neg(({WILL, option}));
                opt->state->localside = 1;
                _call_handler(opt, LOCALON, 0);
            }
            else
            {
                send_telnet_neg(({WONT, option}));
                opt->state->localside = 0;
                _call_handler(opt, LOCALOFF, 0);
            }
        }
        else if (opt->lo_wishes->localside == WILL ) {
            // wir haben schon WILL gesendet, welches der Client jetzt
            // bestaetigt hat (d.h. die Option ist jetzt auf dieser Seite an),
            // wir bestaetigen das aber nicht (nochmal).
            opt->re_wishes->localside = 0;
            opt->lo_wishes->localside = 0;
            if (opt->state->localside != 1)
            {
              opt->state->localside = 1;
              _call_handler(opt, LOCALON, 0);
            }
        }
        else {
            // Mhmm. Wir haben ein WONT gesendet, der Client hat mit DO
            // geantwortet. Das darf er eigentlich nicht.
            // TODO: Was tun?
            send_telnet_neg ( ({WONT, option}) );
        }
        // fertig mit DO
        return;
    }
    // bleibt noch SB ueber
    else if ( command == SB )
    {
        opt->re_wishes->sbdata = optargs;
        _call_handler(opt, SB, optargs);
        return;
    } // if ( command == SB )
}

// wird nur in base.c gerufen, wenn die Verbindung an das Spielerobjekt
// uebergeben wurde.
// es uebertraegt unter anderem den Telnet Option Zustand aus login.c (das ist
// dann previous_object()) in das Spielerobjekt (welches dann this_object())
// ist!
protected void
startup_telnet_negs()
{
  int* optargs;

  Set( P_TTY_TYPE, 0 );  //avoid ANY mistakes... Wird unten neu gesetzt.
  // Daten aus dem Loginobjekt uebertragen. Das ist wichtig! (Dabei wird dann
  // auch der Status von der letzten Session ueberschrieben.)
  TN = ({mapping}) previous_object()->query_telnet_neg();
  // bevor irgendwas anderes gemacht wird, werden erstmal die Standardhandler
  // gesetzt. Die sind naemlich in diesem Objekt jetzt erstmal kaputt, weil
  // sie im Loginobjekt gerufen werden.
  _bind_telneg_std_handlers();
  // dann restliche Daten aus dem Loginobjekt holen.
  Terminals = ({string *}) previous_object()->query_terminals();
  Set( P_TTY_COLS, ({int})previous_object()->Query(P_TTY_COLS) );
  Set( P_TTY_ROWS, ({int})previous_object()->Query(P_TTY_ROWS) );

  struct telopt_s opt = TN[TELOPT_NAWS];
  if (optargs = (opt->re_wishes)->sbdata) {
      eval_naws(optargs);
  }

  if ( pointerp(Terminals) && sizeof(Terminals)) {
      if ( Terminals[0][0..3] == "dec-" )
          Terminals[0] = Terminals[0][4..];

      if ( Terminals[0] == "linux" )
          Terminals[0] = "vt100";

      Set( P_TTY_TYPE, Terminals[0] );
  }
  // fuer TELOPT_TM jetzt keine Verhandlung anstossen, aber Handler
  // registrieren.
  bind_telneg_handler(TELOPT_TM, #'_std_re_handler_tm, 0, 0);
  // und zum Schluss wird der Support fuer CHARSET aktiviert.
  bind_telneg_handler(TELOPT_CHARSET, #'_std_re_handler_charset,
                      #'_std_lo_handler_charset, 1);
}

// somehow completely out of the ordinary options processing/negotiation. But
// the only purpose is to transmit something over the wire which is not shown,
// but (likely) answered by the other device.
protected void send_telnet_timing_mark() {
  struct telopt_s opt = TN[TELOPT_TM];
  if (pointerp(opt->data))
    opt->data[1] = utime();
  else
    opt->data = ({ 0, utime() });
  // absichtlich nicht do_telnet_ne() verwendet, da dies nicht senden wuerde,
  // haette der Client schonmal mit WILL geantwortet. TELOPT_TM ist aber eine
  // Option, bei der man das darf...
  send_telnet_neg( ({DO, TELOPT_TM}) );
}

/* Is called from the H_PRINT_PROMPT driver hook and appends the IAC EOR if
 * the client supports it.
 */
void print_prompt(string prompt) {
//    if (extern_call() && previous_object()!=this_object())
//        return;

    // ggf. Uhrzeit in den prompt reinschreiben.
    prompt = regreplace(prompt,"\\t",strftime("%H:%M"),0);
    // Prompt senden
    tell_object(this_object(), prompt);
    // Und EOR senden, falls vom Client gewuenscht.
    struct telopt_s opt = TN[TELOPT_EOR];
    if (structp(opt) && opt->state->localside == 1)
    {
        binary_message(({IAC, EOR}), 1);
        DTN("tn_eor ",({IAC,EOR}));
    }
}

// Helper
private void eval_naws(int *optargs) {
  int l, c;

  if ( sizeof(optargs) != 4 )
  {
      tell_object(this_object(),
          break_string( sprintf("Dein Client hat einen Fehler beim"
                            +"Aushandeln der TELOPT_NAWS - er hat"
                            +"IAC SB %O IAC SE gesendet!\n",
                            optargs), 78,
                    "Der GameDriver teilt Dir mit: " ));
      // und dem Client sagen, dass er den Schrott nicht mehr uebertragen
      // soll (falls wir das nicht schon gemacht haben).
      struct telopt_s opt = TN[TELOPT_NAWS];
      if (opt->state->remoteside == WILL
          && opt->lo_wishes->remoteside != DONT)
        send_telnet_neg(( {DONT, TELOPT_NAWS}) );
      return;
  }

  if ( interactive(this_object()) ){
      if ( !optargs[1] )
          c = optargs[0];
      else
          c = optargs[1] + optargs[0] * 256;

      if ( c < 35 ){
          if (Query(P_TTY_SHOW))
              tell_object( this_object(),
                       break_string("Dein Fenster ist schmaler als"
                                    +" 35 Zeichen? Du scherzt. ;-)"
                                    +" Ich benutze den Standardwert"
                                    +" von 80 Zeichen.\n", 78,
                                    "Der GameDriver teilt Dir mit: ")
                       );
          c = 80;
      }

      if ( !optargs[3] )
          l = optargs[2];
      else
          l = 256 * optargs[2] + optargs[3];

      if ( l > 100 ){
          //TODO: remove
          l = 100;
          if (Query(P_TTY_SHOW))
              tell_object( this_object(),
                       break_string("Tut mir leid, aber ich kann"
                                    +" nur bis zu 100 Zeilen"
                                    +" verwalten.\n", (c ? c-2 : 78),
                                    "Der GameDriver teilt Dir mit: " )
                       );
      }

      if ( l < 3 ){
          if (Query(P_TTY_SHOW))
              tell_object( this_object(),
                       break_string("Du willst weniger als drei"
                                    +" Zeilen benutzen? Glaub ich"
                                    +" Dir nicht - ich benutze den"
                                    +" Standardwert von 24"
                                    +" Zeilen.\n", (c ? c-2 : 78),
                                    "Der GameDriver teilt Dir mit: " )
                       );
          l = 24;
      }

      if ( (({int}) Query(P_TTY_ROWS) != l) ||
           (({int}) Query(P_TTY_COLS) != c) ){
          Set( P_TTY_ROWS, l );
          Set( P_TTY_COLS, c );

          if (Query(P_TTY_SHOW))
              tell_object( this_object(),
                       break_string("Du hast Deine Fenstergroesse auf"
                                    +" "+l+" Zeilen und "+c+
                                    " Spalten geaendert.\n", c-2,
                                    "Der GameDriver teilt Dir mit: ")
                       );
      }
  }
}

// Query-/Set-Methoden
// Und wenn hier einer von aussen dran rumpfuscht, werde ich sauer.
mapping
query_telnet_neg()
{
    return TN;
}

// siehe oben
string *
query_terminals() {
    return Terminals;
}

public int _query_p_lib_telnet_rttime()
{
  struct telopt_s opt = TN[TELOPT_TM];
  if (opt && pointerp(opt->data))
    return (opt->data)[0];
  return 0;
}

