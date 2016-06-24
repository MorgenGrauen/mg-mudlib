#pragma strong_types,save_types,rtt_checks
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma pedantic

#include <driver_info.h>

#ifndef DEBUG
#define DEBUG(x)        if (find_player("zesstra"))\
        tell_object(find_player("zesstra"),"LAG: "+x+"\n")
#endif

#define SAVEFILE __DIR__+"save/lag-o-daemon"
#define LOG(x) log_file("LAG",x,200000);
#define LOGMAX(x) log_file("LAGMAX",x,300000);

// Anzahl an zu merkenden Werten (1800==3600s)
#define MESSWERTE 1800

nosave float hbstat, obstat;
nosave mapping lastwarning=([]);
nosave int *lasttime = utime();
nosave int lasthbcount = absolute_hb_count();
float *lagdata = allocate(MESSWERTE,0.0);
// Lags fuer die letzten   2s (1 HB), 20s (10 HB), 60s (30 HB),
//                       300s (150 HB), 900s (450 HB), 3600s (1800 HB)
float *lag = allocate(6,0.0);
int *hbdata = allocate(MESSWERTE);
// Lags fuer die letzten   2s (1 HB), 20s (10 HB), 60s (30 HB),
//                       300s (150 HB), 900s (450 HB), 3600s (1800 HB)
int *hbs = allocate(6);
int *codata = allocate(MESSWERTE);
// Lags fuer die letzten   2s (1 HB), 20s (10 HB), 60s (30 HB),
//                       300s (150 HB), 900s (450 HB), 3600s (1800 HB)
int *callouts = allocate(6);

void create() {
  seteuid(getuid());
  // Bei nicht einlesbaren Savefile und nach Reboot Werte neu initialisieren.
  // Denn dann ist die Differenz der HBs bedeutungslos. Erkennung ploetzlich
  // kleinere HB-Zahl.
  if (!restore_object(SAVEFILE)
      || efun::driver_info(DI_NUM_HEARTBEAT_TOTAL_CYCLES) < lasthbcount)
  {
    lagdata = allocate(MESSWERTE,0.0);
    lag = allocate(6,0.0);
    hbdata = allocate(MESSWERTE);
    hbs = allocate(6);
    codata = allocate(MESSWERTE);
    callouts = allocate(6);
  }
  set_heart_beat(1);
}

int remove(int silent) {
  save_object(SAVEFILE);
  destruct(this_object());
  return 1;
}

// liefert Mittelwertarray zurueck
//TODO: <int|string>* results 
private mixed calc_averages(mixed data, mixed results, int index, float scale) { 
  float tmp;
  int t;

  if (!scale) scale=1.0;
  
  // 2s
  results[0] = to_float(data[index]) * scale;

  // alle 10s / 5 HBs den 20s-Wert neu berechnen
  if ( index%5 == 1) {
    if (index<9) {
      foreach(int i: index-4+MESSWERTE .. MESSWERTE-1) {
        tmp+=data[i];
        t++;
      }
      foreach(int i: 0 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    else {
      foreach(int i: index-9 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    results[1]=tmp/to_float(t) * scale;
  }

  // alle 10s / 5 HBs den 60s-Wert neu berechnen, aber in anderem HB als den
  // 10s-Wert
  if ( index%5 == 3) {
    tmp=0.0;
    t=0;
    if (index<29) {
      foreach(int i: index-14+MESSWERTE .. MESSWERTE-1) {
        tmp+=data[i];
        t++;
      }
      foreach(int i: 0 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    else {
      foreach(int i: index-29 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    results[2]=tmp/to_float(t) * scale;
  }

  // 300s
  // alle 40s / 20 HBs den 300s-Wert neu berechnen.
  if ( index%20 == 12) {
    tmp=0.0;
    t=0;
    if (index<149) {
      foreach(int i: index-74+MESSWERTE .. MESSWERTE-1) {
        tmp+=data[i];
        t++;
      }
      foreach(int i: 0 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    else {
      foreach(int i: index-149 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    results[3]=tmp/to_float(t) * scale;
  }


  // 900s
  // alle 80s / 40 HBs den 900s-Wert neu berechnen.
  if ( index%40 == 31) {
    tmp=0.0;
    t=0;
    if (index<449) {
      foreach(int i: index-224+MESSWERTE .. MESSWERTE-1) {
        tmp+=data[i];
        t++;
      }
      foreach(int i: 0 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    else {
      foreach(int i: index-449 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    results[4]=tmp/to_float(t) * scale;
  }

  // 3600s
  // alle 300s / 150 HBs den 3600s-Wert neu berechnen.
  if ( index%150 == 128) {
    tmp=0.0;
    t=0;
    if (index<1799) {
      foreach(int i: index-899+MESSWERTE .. MESSWERTE-1) {
        tmp+=data[i];
        t++;
      }
      foreach(int i: 0 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    else {
      foreach(int i: index-1799 .. index) {
        tmp+=data[i];
        t++;
      }
    }
    results[5]=tmp/to_float(t) * scale;
  }
  return results;
}

protected void heart_beat() {
  int hbcount=efun::driver_info(DI_NUM_HEARTBEAT_TOTAL_CYCLES);
  // per Modulo Index auf 0 - 449 begrenzen
  int index=hbcount%MESSWERTE;

  // mass fuer das lag ist das Verhaeltnis von vergangener Sollzeit (Differenz
  // HB-Counts * 2) und vergangener Istzeit (Differenz von time()). Und das
  // ganze wird dann als Abweichung von 1 gespeichert (0, wenn kein Lag)
  int *nowtime = utime();
  float timediff = to_float((nowtime[0]-lasttime[0])) 
                 + to_float(nowtime[1]-lasttime[1])/1000000.0;

//  lagdata[index] = 1.0 -
//                   (to_float((hbcount - lasthbcount) * __HEART_BEAT_INTERVAL__)
//                     / timediff);
//  lagdata[index] = (timediff -
//    to_float((hbcount - lasthbcount) * __HEART_BEAT_INTERVAL__)) /
//    to_float((hbcount - lasthbcount) * __HEART_BEAT_INTERVAL__);
  lagdata[index] = abs(to_float((hbcount - lasthbcount) * __HEART_BEAT_INTERVAL__)
                    - timediff)  /
                   to_float((hbcount - lasthbcount) * __HEART_BEAT_INTERVAL__);


  hbdata[index] = efun::driver_info(DI_NUM_HEARTBEATS_LAST_PROCESSED);
  codata[index] = efun::driver_info(DI_NUM_CALLOUTS);

  /*                     
  DEBUG(sprintf("Index: %d: %d/%.6f => %:6f /1\n",index,
        (hbcount - lasthbcount)*2,
        timediff, lagdata[index]));
  */      
  lasthbcount=hbcount;
  lasttime=nowtime;
 
  lag = calc_averages(lagdata, lag, index, 100.0);
  // Multiplizieren mit 100
  //DEBUG(sprintf("%O\n",lag));
  //lag = map(lag, function float (float f) {return f*100.0;} );
  //DEBUG(sprintf("%O\n",lag));
  hbs = map(calc_averages(hbdata, hbs, index,1.0),#'to_int);
  callouts = map(calc_averages(codata, callouts, index,1.0),#'to_int);

  // und ausserdem mal etwas loggen fuer den Moment
  if (lag[0] > 1000 || callouts[0] > 1000)
  {
  LOGMAX(sprintf("%s, %d, %d, %:6f, %:6f, %:6f, %:6f, %:6f, %:6f, "
              "%d, %d, %d, %d, %d, %d, "
              "%d, %d, %d, %d, %d, %d, \n",
          strftime("%y%m%d-%H%M%S"),time(),hbcount,
          lag[0],lag[1],lag[2],lag[3],lag[4],lag[5],
          hbs[0],hbs[1],hbs[2],hbs[3],hbs[4],hbs[5],
          callouts[0],callouts[1],callouts[2],callouts[3],callouts[4],
          callouts[5]));
  }
  else if (hbcount%80 == 0 || lag[0] > 10)
  {
  LOG(sprintf("%s, %d, %d, %:6f, %:6f, %:6f, %:6f, %:6f, %:6f, "
              "%d, %d, %d, %d, %d, %d, "
              "%d, %d, %d, %d, %d, %d, \n",
          strftime("%y%m%d-%H%M%S"),time(),hbcount,
          lag[0],lag[1],lag[2],lag[3],lag[4],lag[5],
          hbs[0],hbs[1],hbs[2],hbs[3],hbs[4],hbs[5],
          callouts[0],callouts[1],callouts[2],callouts[3],callouts[4],
          callouts[5]));
  }
}

float *read_lag_data() {
  return ({lag[2],lag[4],lag[5]});
}
float *read_ext_lag_data() {
  return copy(lag);
}
int *read_hb_data() {
  return copy(hbs);
}
int *read_co_data() {
  return copy(callouts);
}

