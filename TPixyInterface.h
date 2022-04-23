//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//
// This file is for defining the SPI-related classes.  It's called Pixy.h instead
// of Pixy_SPI.h because it's the default/recommended communication method
// with Arduino.  This class assumes you are using the ICSP connector to talk to
// Pixy from your Arduino.  For more information go to:
//
//http://cmucam.org/projects/cmucam5/wiki/Hooking_up_Pixy_to_a_Microcontroller_(like_an_Arduino)
//
 
#ifndef _TPIXY_INTERFACE_H
#define _TPIXY_INTERFACE_H
 
#include "TPixy.h"
 
#define PIXY_SYNC_BYTE              0x5a
#define PIXY_SYNC_BYTE_DATA         0x5b
#define PIXY_BUF_SIZE               16


/** An interface for communicating between the Pixy and a specific hardware interface
  */
class TPixyInterface
{
public:
    /** Creates a TPixyInterface
      */
    TPixyInterface() {}
    /** Initializes a TPixyInterface
      */
    virtual void init() = 0;
    /** Sends the given data to the Pixy with a given data length
      * @param data the data to send
      * @param len the length of the data to send
      * @return the interface return signal
      */
    virtual int8_t send(uint8_t *data, uint8_t len) = 0;
    /** Sets an argument for the interface to use
      * @param arg the argument to use
      */
    virtual void setArg(uint16_t arg) = 0;
    /** Reads a word from the interface
      * @return the word read from the interface
      */
    virtual uint16_t getWord() = 0;
    /** Reads a byte from the interface
      * @return the byte read from the interface
      */
    virtual uint8_t getByte() = 0;
};

template <class BufType> class CircularQ
{
public:
    BufType buf[PIXY_BUF_SIZE];
    uint8_t len;
    uint8_t writeIndex;
    uint8_t readIndex;
    
    CircularQ();
    bool read(BufType *c);
    uint8_t freeLen();
    bool write(BufType c);
};
 
/** An interface for communicating between the Pixy via an SPI interface
  */
class PixyInterfaceSPI : TPixyInterface
{
public:
    SPI* spi;
    /** Constructs a PixyInterfaceSPI object
      * @param interface the SPI pointer
      */
    PixyInterfaceSPI(SPI* interface);
    /** Initializes the PixyInterfaceSPI
      */
    virtual void init();
    /** Reads a word from the interface
      * @return the word read from the interface
      */
    virtual uint16_t getWord();
    /** Reads a byte from the interface
      * @return the byte read from the interface
      */
    virtual uint8_t getByte();
    /** Sends the given data to the Pixy with a given data length
      * @param data the data to send
      * @param len the length of the data to send
      * @return the interface return signal
      */
    virtual int8_t send(uint8_t *data, uint8_t len);
    /** Sets an argument for the interface to use [unused]
      * @param arg the argument to use
      */
    virtual void setArg(uint16_t arg);
 
private:
    // we need a little circular queues for both directions
    CircularQ<uint8_t> outQ;
    CircularQ<uint16_t> inQ;
 
    uint16_t getWordHw();
    void flushSend();
};
 
#endif
