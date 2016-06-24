#pragma strict_types
#include <udp.h>
#include <daemon.h>

#define MAX_READ_FILE_LEN 50000

// TEMPORARY

#include <udp_language.h>
#include <logging.h>

#ifndef LOCAL_NAME
#define LOCAL_NAME "MorgenGrauen"
#endif

#ifndef INETD_INVALID_ACCESS
#define INETD_INVALID_ACCESS INTERMUDLOG("INVALID_ACCESS")
#endif

#ifndef INVALID_ACCESS
#define INVALID_ACCESS(x) \
           log_file(INETD_INVALID_ACCESS, \
                    sprintf(INETD_INV_ACCESS_MSG "TI: %O PO: %O\n", \
                            ctime()[4..15],this_interactive(), \
                            previous_object()))
#endif


// END TEMPORARY

void udp_man(mapping data)
{
  mapping pages;
  int index;
  string manpage,ret;
  string|string* tmp;

  if (previous_object()!=find_object(INETD))
  {
    INVALID_ACCESS(Man);
    return;
  }

  manpage=data[DATA];
  tmp=explode(manpage,"/");
  if (sizeof(tmp)>1)
  {
    if (file_size(MAND_DOCDIR+manpage)>=0)
      tmp=({tmp[<1],manpage});
    else
      tmp=({});
  }
  else
    tmp=(string *)call_other(MAND,"locate",data[DATA],0);
  pages=([]);
  index=sizeof(tmp);
  while(index--)
  {
    if (tmp[1][0..1]!="g.") pages[tmp[index]]=tmp[index-1];
    index--;
  }
  switch(sizeof(pages))
  {
    case 0:
      ret=sprintf(INETD_NO_MANPAGE,LOCAL_NAME,manpage);
      break;
    case 1:
      tmp=m_indices(pages)[0];
      ret=sprintf(INETD_MANPAGE_FOUND,LOCAL_NAME,pages[tmp]);
      index=0;
      while(manpage=read_file(MAND_DOCDIR+tmp,index))
      {
        ret+=manpage;
        index+=MAX_READ_FILE_LEN;
      }
      break;
    default:
      ret=sprintf(INETD_MANPAGES,LOCAL_NAME,"",
                  break_string(implode(m_values(pages)," "),78),"");
      break;
  }
  INETD->_send_udp(data[NAME],
                     ([
                       REQUEST: REPLY,
                       RECIPIENT: data[SENDER],
                       ID: data[ID],
                       DATA: ret
                     ]));
}

string send_request(string mudname, string pagename)
{
  return (INETD->_send_udp(mudname,
                             ([REQUEST: "man",
                               DATA: pagename,
                               SENDER: getuid(previous_object())]),1)||
          sprintf(INETD_MAN_REQUESTED,pagename,mudname));
}
