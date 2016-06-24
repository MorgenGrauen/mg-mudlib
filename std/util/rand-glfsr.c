/* Dieses File implementiert einen Pseudo-Zufallszahlengenerator mit einem
 * Linear Feedback Shift Register in der Galois-Variante.
 * Dieses PRNG ist schlechter und viel ineffizienter als der im Driver
 * eingebaute.
 * Die einzige sinnvolle Anwendung ist, wenn man aus irgendeinem Grund das
 * seed selber waehlen muss, damit man die Sequenz von Pseudozufall immer
 * wieder reproduzieren kann.
 *
 * Das Seed von der Blueprint wird vermutlich staendig veraendert (d.h.
 * verlasst euch nicht drauf, dass es konstant bleibt), wollt ihr eine
 * 'private' Instanz mit eurem Seed, clont das Objekt (aber verliert den Clone
 * nicht).
 *
 * Es wird standardmaessig ein Polynom fuer eine Periodenlaenge von 2^32 - 1
 * verwendet.
 * 
 * Im Netz finden sich Infos ueber LFSRs und Tabellen fuer maximum-length
 * feedback polynomials (M-Sequence Feedback Taps), falls jemand braucht.
 *
*/
#pragma strong_types,rtt_checks,pedantic

#include <tls.h>

// Default-Polynom ist das:
// x^32 + x^31 + x^28 + x^27 + x^24 + x^23 + x^20 + x^19 + x^16 + x^15 + x^12
//      + x^11 + x^8 + x^7 + x^5 + x^3 + 1
// Taps: 32, 31, 28, 27, 24, 23, 20, 19, 16, 15, 12, 11, 8, 7, 5, 3
// Binary: 110011001100110011001100110101000
#define DEFAULTP 0x1999999a8
// Andere gebraeuchliche:
// x^32 + x^30 + x^26 + x^25 + 1,
// Taps 32, 30, 26, 25
// Binary: 101000110000000000000000000000000
//#define DEFAULTP 0x146000000
// x^16 + x^14 + x^13 + x^11 + 1
// Taps 16, 14, 13, 11
// Binary: 1011010000000000
//#define DEFAULTP 0xB400

private int polynom = DEFAULTP;
private int state = 2553647223;
//private int period;

public varargs void init(int seed, int newp)
{
  if (!seed)
    raise_error("Illegal seed 0\n");
  // Es darf nur ein 32 bit breiter Seed verwendet werden. Die oberen 32 bit
  // werden mit den unteren 32 bit XOR-rt (also, einmal die oberen 32 shiften
  // und einmal die oberen 32 wegmaskieren.
  seed = ((seed>>32) & 0xffffffff) ^ (seed & 0xffffffff);
  //printf("Seed: %064b - 0x%x\n",seed,seed);
  
  if (!seed)
    raise_error("Illegal reduced seed: 0\n");

  state = seed;
  polynom = newp || DEFAULTP;
}

public void InitWithUUID(string uuid)
{
  string md5hash = hash(TLS_HASH_MD5, uuid);
  int seed;
  // 8 Bytes aus dem Hash ermitteln
  foreach(int b: 16)
  {
    // Jeweils zwei Zeichen herausschneiden und als Hexadezimalzahl
    // interpretieren, was jeweils ein byte (8 bit) gibt. 
    int tmp = to_int("0x"+md5hash[b*2..b*2+1]);
    // diese werden dann in eine 64 bit breite Zahl zusammengefasst. Das
    // gerade ermittelte Byte wird nach links geshiftet und mit dem, was da
    // ggf. schon steht, XORred. Ist der int voll, faengt man wegen Modulo 64
    // wieder rechts an.
    //printf("S: %064b - %08b\n",seed,tmp);
    seed = seed ^ (tmp << ((b*8)%64));
    //printf("S: %064b - 0x%x\n",seed, seed);
  }
  //printf("Seed: 0x%x - %b\n",seed,seed);
  init(seed);
}

public int nextbit()
{
  // Get LSB (i.e., the output bit).
  int lsb = state & 1;
  // Shift register by one bit
  state >>= 1;
  // Apply a toggle mask, which flips all the tap bits, _if_ the output bit is
  // 1. The mask has 1 at bits corresponding to taps, 0 elsewhere. In other
  // words, the polynom from above.
  if (lsb)
      state ^= polynom;
  // debug check ;-)
  if (!state)
    raise_error("State must not be zero, but it is.\n");
  //++period; 
  //printf("State: %032b (Period: %d)\n",state,period);
  return lsb;
}

public int nextbits(int count)
{
  int result;
  if (count>64)
    raise_error(sprintf("Count is %d, but must be <= 64.\n",count));
  foreach(int i: count)
    result = (result << 1) | nextbit();
  return result;
}

public int nextbyte()
{
  return nextbits(8);
}

public int random(int n)
{
  int rnd = nextbits(32);
  //generates a random number on [0,1)-real-interval 
  float tmp = rnd * (1.0/4294967296.0);
  // Skalieren auf [0,n)
  return to_int(tmp * n);
}

// Just skip some bits and discard them.
public void skipbits(int count)
{
  foreach(int i: count)
    nextbit();
}


public void dumpstream(string file, int bytes)
{
  int *stream = allocate(bytes);
  foreach(int i: bytes)
  {
    stream[i] = nextbits(8);
  }
  write_file(file, sprintf("%@c",stream));
}

public int Configure(int data)
{
  if (!data)
    return state;
  
  if (!intp(data))
    return 0;
  state = data;
  return 1;
}

