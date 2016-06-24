// MorgenGrauen MUDlib
/** \file /secure/master/cidr.c
* Funktionen zur Interpretation und Umrechnung von IP-Adressen in CIDR-Notation.
* \author Zesstra
* \date 05.02.2010
* \version $Id$
*/
/* Changelog:
*/

private int IPv4_addr2int(string addr) {
    int mask;

    if (!stringp(addr) || !sizeof(addr))
      return 0;
    // irgendwelche Zeichen ausser 0-9, . und / drin?
    if (sizeof(addr-"0123456789./"))
      return 0;

    string *parts = explode(addr, "/") - ({""});
    // Netzmaske gegeben? -> Netzwerkadresse bestimmen
    if (sizeof(parts) == 2) {
        // a.b.c.d/x oder a.b.c.d/w.x.y.z
        int res = IPv4_addr2int(parts[0]); // erstmal Wert von a.b.c.d...
        if (strstr(parts[1], ".") > -1
            && sizeof(explode(parts[1],".")) == 4)
            // a.b.c.d/w.x.y.z -> Wert von w.x.y.z bestimmen
          mask = IPv4_addr2int(parts[1]);
        else {
          mask = to_int(parts[1]);
          // Wert von /x
          mask = 0xffffffff - (to_int(pow(2, 32-mask))-1);
        }
        // verunden und Ende.
        return res & mask;
    }

    parts = explode(parts[0], ".");
    // Abgrekuerzte Adresse a la a.b.? Rest mit 0 ergaenzen.
    switch(sizeof(parts - ({""})) ) {
      case 1:
        parts += ({"0","0","0"});
      case 2:
        parts += ({"0","0"});
      case 3:
        parts += ({"0"});
      case 4:
        break;
      default:
        return 0;
    }

   return( to_int(parts[0]) << 24)
         + ( to_int(parts[1]) << 16)
         + ( to_int(parts[2]) << 8 )
         + to_int(parts[3]);
}

private int IPv4_net_size(string addr) {
    if (!stringp(addr) || !sizeof(addr))
      return 0;
    // irgendwelche Zeichen ausser 0-9, . und / drin?
    if (sizeof(addr-"0123456789./"))
      return 0;

    string *parts = explode(addr, "/") - ({""});
    // Netzmaske gegeben? -> Netzwerkadresse bestimmen
    if (sizeof(parts) == 2) {
        // a.b.c.d/x oder a.b.c.d/w.x.y.z
        if (strstr(parts[1], ".") > -1
            && sizeof(explode(parts[1],".")) == 4)
            // a.b.c.d/w.x.y.z 
          return 0xffffffff - (IPv4_addr2int(parts[1]) || 1) + 1;
        else {
          // Einfach 2^(32-mask) Adressen.
          return to_int(pow(2, 32 - to_int(parts[1]) ) );
        }
    }

    parts = explode(parts[0], ".");
    switch(sizeof(parts - ({""})) ) {
      case 1: // Class A
        return 256*256*256;
      case 2: // Class B
        return 256*256;
      case 3: // Class C
        return 256;
      case 4: // einzelne Adresse
        return 1;
      default: // ungueltige Adresse
        return __INT_MAX__;
    }

    // hier sollte man gar nicht ankommen.
    return __INT_MAX__;
}

string IPv4_int2addr(int ip) {
    int *parts=allocate(4);
    parts[0] = (ip & 0xff000000) >> 24;
    parts[1] = (ip & 0x00ff0000) >> 16;
    parts[2] = (ip & 0x0000ff00) >> 8;
    parts[3] = (ip & 0x000000ff);
    // int is signed. Anything > 127 would wrap around to -128 if we are on a
    // machine with 32-bit wide ints.
    if (parts[0] < 0)
        parts[0] = 128 + (128-abs(parts[0]));

    return implode(map(parts,#'to_string), ".");
}

