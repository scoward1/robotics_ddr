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
// This file is for defining the Block struct and the Pixy template class.
// (TPixy).  TPixy takes a communication link as a template parameter so that
// all communication modes (SPI, I2C and UART) can share the same code.
//
 
#ifndef _TPIXY_H
#define _TPIXY_H
 
#include "mbed.h"
//#include "TPixyInterface.h"
 
// Communication/misc parameters
#define PIXY_INITIAL_ARRAYSIZE      30
#define PIXY_MAXIMUM_ARRAYSIZE      130
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa
#define PIXY_MAX_SIGNATURE          7
#define PIXY_DEFAULT_ARGVAL         0xffff
 
// Pixy x-y position values
#define PIXY_MIN_X                  0L
#define PIXY_MAX_X                  319L
#define PIXY_MIN_Y                  0L
#define PIXY_MAX_Y                  199L
 
// RC-servo values
#define PIXY_RCS_MIN_POS            0L
#define PIXY_RCS_MAX_POS            1000L
#define PIXY_RCS_CENTER_POS         ((PIXY_RCS_MAX_POS-PIXY_RCS_MIN_POS)/2)
 
enum BlockType {
    NORMAL_BLOCK,
    CC_BLOCK
};
 
/** A structure for containing Block data
  * signature the signature of the block
  * x the x position of the block on the screen
  * y the y position of the block on the screen
  * width the width of the block on the screen
  * height the height of the block on the screen
  * angle the angle of the block
  */
struct Block {
    uint16_t signature;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t angle;
};
 
/** The superclass handler for the Pixy camera from a given connection handler (SPI, I2C, etc)
  * Used for instantiating a Pixy object that interfaces via an abtritrary connection handler
  */
template <class TPixyInterface> class TPixy
{
public:
    Serial* serial;
    Block *blocks;
    
    /** Creates a TPixy object with a given connection handler
      * @param type the pointer to the interface connection handler
      * @param serialOut the optional serial output pointer to print out Pixy camera data
      * @param arg an optional integer argument used for custom implementations of the Pixy library
      */
    TPixy(TPixyInterface* type, Serial* serialOut = NULL, uint16_t arg = PIXY_DEFAULT_ARGVAL);
    ~TPixy();
 
    /** Prints the given Block out to the given serial port
      * @param &block the Block to print
      */
    void printBlock(Block &block);
    /** Returns the number of Blocks found in the current view and updates all Blocks' info
      * @param maxBlocks the maximum number of Blocks to search for
      * @return the number of Blocks found
      */
    uint16_t getBlocks(uint16_t maxBlocks = 1000);
    /** Sets the PWM value of the Pixy servos
      * @param s0 the value of the left servo
      * @param s1 the value of the right servo
      * @return the interface return value for sending the servo command
      */
    int8_t setServos(uint16_t s0, uint16_t s1);
    /** Sets the brightness of the Pixy RGB LED
      * @param brightness the brightness of the LED
      * @return the interface return value for sending the brightness command
      */
    int8_t setBrightness(uint8_t brightness);
    /** Sets the color of the Pixy RGB LED
      * @param r the red color value
      * @param g the green color value
      * @param b the blue color value
      */
    int8_t setLED(uint8_t r, uint8_t g, uint8_t b);
    /** Initializes the Pixy
      */
    void init();
 
private:
    TPixyInterface* link;
    BlockType blockType;
    /** Returns if the Pixy is ready to receive/send a message
      */
    bool getStart();
    /** Resizes the block array to add an additional block
      */
    void resize();
    bool skipStart;
    uint16_t blockCount;
    uint16_t blockArraySize;
};
 
template <class TPixyInterface> void TPixy<TPixyInterface>::init()
{
    link->init();
}
 
 
template <class TPixyInterface> TPixy<TPixyInterface>::TPixy(TPixyInterface* type, Serial* serialOut, uint16_t arg) : serial(serialOut), link(type)
{
    skipStart = false;
    blockCount = 0;
    blockArraySize = PIXY_INITIAL_ARRAYSIZE;
    blocks = (Block *)malloc(sizeof(Block)*blockArraySize);
    link->setArg(arg);
}
 
template <class TPixyInterface> void TPixy<TPixyInterface>::printBlock(Block &block)
{
    int i, j;
    char sig[6], d;
    bool flag;
    if (block.signature > PIXY_MAX_SIGNATURE) { // color code! (CC)
        // convert signature number to an octal string
        for (i = 12, j = 0, flag = false; i >= 0; i -= 3) {
            d = (block.signature >> i) & 0x07;
            if (d > 0 && !flag) {
                flag = true;
            }
            if (flag) {
                sig[j++] = d + '0';
            }
        }
        sig[j] = '\0';
        if (serial != NULL) {
            serial->printf("CC block! sig: %s (%d decimal) x: %d y: %d width: %d height: %d angle %d\n", sig, block.signature, block.x, block.y, block.width, block.height, block.angle);
        }
    } else {}// regular block.  Note, angle is always zero, so no need to print
    serial->printf("sig: %d x: %d y: %d width: %d height: %d\n", block.signature, block.x, block.y, block.width, block.height);
}
 
template <class TPixyInterface> TPixy<TPixyInterface>::~TPixy()
{
    free(blocks);
}
 
template <class TPixyInterface> bool TPixy<TPixyInterface>::getStart()
{
    uint16_t w, lastw;
    lastw = 0xffff;
    while(true) {
        w = link->getWord();
        if (w == 0 && lastw == 0) {
            wait_ms(10);
            return false;
        } else if (w == PIXY_START_WORD && lastw == PIXY_START_WORD) {
            blockType = NORMAL_BLOCK;
            return true;
        } else if (w == PIXY_START_WORD_CC && lastw == PIXY_START_WORD) {
            blockType = CC_BLOCK;
            return true;
        } else if (w == PIXY_START_WORDX) {
            if (serial != NULL) {
                serial->printf("reorder");
            }
            link->getByte(); // resync
        }
        lastw = w;
    }
}
 
template <class TPixyInterface> void TPixy<TPixyInterface>::resize()
{
    blockArraySize += PIXY_INITIAL_ARRAYSIZE;
    blocks = (Block *)realloc(blocks, sizeof(Block)*blockArraySize);
}
 
template <class TPixyInterface> uint16_t TPixy<TPixyInterface>::getBlocks(uint16_t maxBlocks)
{
    uint8_t i;
    uint16_t w, checksum, sum;
    Block *block;
 
    if (!skipStart) {
        if (getStart() == false) {
            return 0;
        }
    } else {
        skipStart = false;
    }
    for(blockCount = 0; blockCount < maxBlocks && blockCount < PIXY_MAXIMUM_ARRAYSIZE;) {
        checksum = link->getWord();
        if (checksum == PIXY_START_WORD) { // we've reached the beginning of the next frame
            skipStart = true;
            blockType = NORMAL_BLOCK;
            if (serial != NULL) {
                serial->printf("skip");
            }
            return blockCount;
        } else if (checksum == PIXY_START_WORD_CC) {
            skipStart = true;
            blockType = CC_BLOCK;
            return blockCount;
        } else if (checksum == 0) {
            return blockCount;
        }
        if (blockCount > blockArraySize) {
            resize();
        }
        block = blocks + blockCount;
 
        for (i = 0, sum = 0; i < sizeof(Block)/sizeof(uint16_t); i++) {
            if (blockType == NORMAL_BLOCK && i >= 5) { // skip
                block->angle = 0;
                break;
            }
            w = link->getWord();
            sum += w;
            *((uint16_t *)block + i) = w;
        }
 
        if (checksum == sum) {
            blockCount++;
        } else {
            w = link->getWord();
            if (serial != NULL) {
                serial->printf("cs error");
            }
        }
        if (w == PIXY_START_WORD) {
            blockType = NORMAL_BLOCK;
        } else if (w == PIXY_START_WORD_CC) {
            blockType = CC_BLOCK;
        } else {
            return blockCount;
        }
    }
    return blockCount;
}
 
template <class TPixyInterface> int8_t TPixy<TPixyInterface>::setServos(uint16_t s0, uint16_t s1)
{
    uint8_t outBuf[6];
    outBuf[0] = 0x00;
    outBuf[1] = 0xff;
    *(uint16_t *)(outBuf + 2) = s0;
    *(uint16_t *)(outBuf + 4) = s1;
    return link->send(outBuf, 6);
}
 
template <class TPixyInterface> int8_t TPixy<TPixyInterface>::setBrightness(uint8_t brightness)
{
    uint8_t outBuf[3];
    outBuf[0] = 0x00;
    outBuf[1] = 0xfe;
    outBuf[2] = brightness;
    return link->send(outBuf, 3);
}
 
template <class TPixyInterface> int8_t TPixy<TPixyInterface>::setLED(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t outBuf[5];
    outBuf[0] = 0x00;
    outBuf[1] = 0xfd;
    outBuf[2] = r;
    outBuf[3] = g;
    outBuf[4] = b;
    return link->send(outBuf, 5);
}
 
#endif
