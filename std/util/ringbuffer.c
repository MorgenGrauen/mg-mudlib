// MorgenGrauen MUDlib
/** \file /std/util/ringbuffer.c
* Implmentiert einen Ringbuffer in LPC.
* Ab und an hat man den Fall, dass man Ereignisse/Meldungen o.ae. hat, von
* denen man immer genau die letzten x gespeichert und abrufbar vorhalten
* moechte. Dieses File stellt eine Datenstruktur und dazugehoerige
* Verwaltungsfunktionen zu Verfuegung, mit denen dies effizient moeglich ist.
* Insbesondere umgehen diese das staendige Slicing von Arrays, mit denen
* haeufig Ringpuffer gebaut werden. Hierbei wird das Schreiben in den Puffer
* billiger, das Abrufen der Daten allerdings teurer. Wird der Puffer haeufiger
* abgefragt als beschrieben, ist dies keine effiziente Loesung.
* 
* \author Zesstra
* \date 22.05.2008
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma no_inherit
#pragma no_shadow
#pragma pedantic
#pragma range_check

#include <util/ringbuffer.h>

/** Standardgroesse fuer Ringbuffer.
  */
#define MAXCOUNT 30


/** Struktur, die einen Ringpuffer speichert.
  Der Puffer selber ist im wesentlichen ein Array. Hinzu kommt noch ein
  Zaehler, der den naechsten zu ueberschreibenden Wert indiziert sowie einen
  Modus, der die Reihenfolge beim Abrufen beshreibt:
  MODE_FIFO: First-in-First-out (default)
  MODE_LIFO: Last-in-First-out
  */
struct std_ringbuffer {
    mixed rbuffer;
    int mode;
    int next;
};

/** Erzeugt einen neuen Ringbuffer und liefert ihn zurueck.
  Die Funktion erzeugt einen neuen, leeren Ringbuffer der Groesse \a size
  und mit Ausgabemodus \a newmode und liefert die entsprechende
  Ringbuffer-Struktur zurueck.
  \attention Die zurueckgelieferte Datenstruktur bitte nicht per Hand
  manipulieren.
  \param[in] size Groesse des zu erzeugenden Ringbuffers. Default: 30
  \param[in] newmode Ausgabemodus des Ringbuffers: \a MODE_FIFO oder
  \a MODE_LIFO. Default: MODE_FIFO
  \return Der erzeugte Ringbuffer (struct vom Typ std_ringbuffer).
  \sa ResizeRingBuffer, RingBufferPut, RingBufferGet
  */
protected struct std_ringbuffer CreateRingBuffer(int size, int newmode) {
  
  struct std_ringbuffer buffer = (<std_ringbuffer>
       rbuffer: allocate(size||MAXCOUNT, 0), 
       mode: newmode || MODE_FIFO, 
       next: 0 );

  // Bei LIFO von oben anfangen.
  if (newmode==MODE_LIFO)
    buffer->next = sizeof(buffer->rbuffer);
  
  return buffer;
}

/** Schreibt einen neuen Wert in den Ringbuffer.
  Diese Funktion schreibt einen neuen Wert in den Ringbuffer \a buffer und
  loescht dabei ggf. den aeltesten Wert im Puffer.
  \param[in,out] buffer zu aendernder Ringbuffer
  \param[in] val hinzuzufuegender Wert
  \sa CreateRingBuffer, ResizeRingBuffer, RingBufferGet
  */
protected void RingBufferPut(struct std_ringbuffer buffer, mixed val) {
  int next = buffer->next;
  // je nach Ausgabemodus von oben nach unten oder von unten nach oben die
  // Werte reinschreiben.
  switch(buffer->mode) {
    case MODE_LIFO:
      // next ist hier eigentlich ein 'last'. Dekrementieren und dann zum
      // Indizieren benutzen.
      buffer->rbuffer[--next] = val;
      // wenn man gerade in Element 0 geschrieben hat, ist naechstes Element
      // sizeof()-1, d.h. sizeof() als next vermerken.
      next ||= sizeof(buffer->rbuffer);
      break;
    default:
      // Wert schreiben und next inkrementieren.
      buffer->rbuffer[next++] = val;
      // wird sizeof() erreicht, ist das naechste Element 0. Etwas schneller
      // waere ein Modulo mit der Buffergroesse direkt bei der Indizierung
      // oben, aber dann kann es u.U. durch einen numerischen Overflow buggen.
      if (next==sizeof(buffer->rbuffer))
	next = 0;
  }
  buffer->next = next;
}

/** Liefert ein Array mit allen Werten des Ringbuffers \a buffer.
  Das zurueckgelieferte Array enthaelt alle Daten des Ringbuffers in der
  beim Erstellen des Puffers gewuenschten Reihenfolge (s. MODE_FIFO,
  MODE_LIFO).
  Ist der Puffer noch nicht vollstaendig gefuellt, enthalten die bisher noch
  nie beschriebenen Elemente 0.
  \param[in] buffer Ringpuffer, dessen Daten ausgeben werden sollen.
  \return Array mit Daten des Puffers.
  \sa CreateRingBuffer, RingBufferPut, ResizeRingBuffer
  */
protected mixed RingBufferGet(struct std_ringbuffer buffer) {
  int size = sizeof(buffer->rbuffer);
  int next = buffer->next;
  mixed rbuffer = buffer->rbuffer;

  switch(buffer->mode) {
    case MODE_LIFO:
      // der hintere Teil enthaelt die neueren Daten. Die kommen zuerst.
      // Wenn next == sizeof(), dann kann das interne Array einfach kopiert
      // werden.
      if (next == size)
	return copy(rbuffer);
      else
	return rbuffer[next .. size-1] + rbuffer[0 .. next-1];
    default:
      // der vordere Teil enthaelt die neueren Daten. Also zuerst den hinteren
      // ausgeben, dann den vorderen.
      // Wenn next 0, kann das interne Array einfach kopiert werden. Sonst
      // muss es zusammengesetzt werden.
      if (next)
	return rbuffer[next .. size-1] + rbuffer[0 .. next-1];
      else
	return copy(rbuffer);
  }
  return 0;
}

/** Erzeugt einen neuen Ringbuffer der Groesse \a size und dem gleichen Modus
 * wie \a buf, der dieselben Daten enthaelt wie \a buf.
 * Diese Funktion erzeugt einen neuen Ringbuffer und kopiert alle Daten aus
 * dem alten Puffer in den neuen um.
 \param[in] buf alter Ringbuffer
 \param[in] size gewuenschte neue Groesse
 \return Liefert neuen Ringbuffer mit gewuenschten Groesse.
 \sa CreateRingBuffer, RingBufferPut, RingBufferGet
 \attention Wird der Ringbuffer verkleinert, kommt es zum Verlust der
 aeltesten Daten, die nicht mehr in den neuen hineinpassen.
 \todo Effizientere Implementation.
 */
protected struct std_ringbuffer ResizeRingBuffer(struct std_ringbuffer buf, 
                                                 int size) {
  // neuen Ringbuffer anlegen.
  struct std_ringbuffer nbuf = CreateRingBuffer(size, buf->mode);
  // und alle daten aus dem alten wieder reinstopfen.
  // ja, das geht effizienter, hab ich gerade aber keinen Bock drauf. Die
  // Funktion wird vermutlich eh sehr selten gerufen.
  foreach(mixed val: RingBufferGet(buf))
    RingBufferPut(nbuf, val);
  // neuen Buffer zurueckgeben. Der alte bleibt unveraendert!
  return nbuf;
}

/* halbfertige Implementation mit Arrays.
#define RBUFFER   0
#define MODE      1
#define COUNTER   2

protected mixed CreateRingBuffer(int size, int mode) {
  mixed buffer = ({ allocate(size||MAXCOUNT, 0), 
                    mode || MODE_FIFO, 
	            0 });
  if (mode==MODE_LIFO)
    buffer[COUNTER] = sizeof(buffer[RBUFFER]);
  return buffer;
}

protected void RingBufferPut(mixed buffer, mixed val) {
  int counter = buffer[COUNTER];
  switch(mode) {
    case MODE_LIFO:
      rbuffer[--counter] = val;
      counter ||= sizeof(rbuffer);
      break;
    default:
      rbuffer[(counter++)%sizeof(rbuffer)] = val;
  }
  buffer[COUNTER]=counter;
}

protected mixed RingBufferGet(mixed buffer) {
  int size=sizeof(rbuffer);

  switch(mode) {
    case MODE_LIFO:
      return rbuffer[counter..size] + rbuffer[0..counter-1];
    default:
      if (counter%size)
	return rbuffer[(counter%size) .. size-1] 
	       + rbuffer[0..(counter%size)-1];
      else
	return copy(rbuffer);
  }
}

protected mixed test() {return rbuffer;}
*/
