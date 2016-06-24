// MorgenGrauen MUDlib
//
// ringbuffer.h -- Header fuer /std/util/ringbuffer.c
//
// $Id: util.h 6404 2007-08-06 20:31:22Z Zesstra $

#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#define MODE_FIFO 1
#define MODE_LIFO 2

#endif // _RINGBUFFER_H

#ifndef _RINGBUFFER_H_IMPLEMENTATION
#ifdef NEED_PROTOTYPES

#define _RINGBUFFER_H_IMPLEMENTATION

protected struct std_ringbuffer CreateRingBuffer(int size, int newmode);
protected void RingBufferPut(struct std_ringbuffer buffer, mixed val);
protected mixed RingBufferGet(struct std_ringbuffer buffer);
protected struct std_ringbuffer ResizeRingBuffer(struct std_ringbuffer buf, 
                                                 int size);

#endif // NEED_PROTOTYPES
#endif // _RINGBUFFER_H_IMPLEMENTATION

