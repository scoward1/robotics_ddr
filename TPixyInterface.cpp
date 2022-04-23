#include "TPixyInterface.h"
 
PixyInterfaceSPI::PixyInterfaceSPI(SPI* interface) : TPixyInterface(), spi(interface) {}
 
void PixyInterfaceSPI::init()
{
    spi->frequency(1000000);
}
 
uint16_t PixyInterfaceSPI::getWord()
{
    // ordering is different (big endian) because Pixy is sending 16 bits through SPI
    // instead of 2 bytes in a 16-bit word as with I2C
    uint16_t w;
    if (inQ.read(&w)) {
        return w;
    }
    return getWordHw();
}
 
uint8_t PixyInterfaceSPI::getByte()
{
    //return SPI.transfer(0x00);
    return spi->write(0x00);
}
 
int8_t PixyInterfaceSPI::send(uint8_t *data, uint8_t len)
{
    int i;
    // check to see if we have enough space in our circular queue
    if (outQ.freeLen() < len) {
        return -1;
    }
    for (i = 0; i < len; i++) {
        outQ.write(data[i]);
    }
    flushSend();
    return len;
}
 
void PixyInterfaceSPI::setArg(uint16_t arg) {}
 
uint16_t PixyInterfaceSPI::getWordHw()
{
    // ordering is different (big endian) because Pixy is sending 16 bits through SPI
    // instead of 2 bytes in a 16-bit word as with I2C
    uint16_t w;
    uint8_t c, c_out = 0;
 
    if (outQ.read(&c_out)) {
        w = spi->write(PIXY_SYNC_BYTE_DATA);
        //w = SPI.transfer(PIXY_SYNC_BYTE_DATA);
    } else {
        w = spi->write(PIXY_SYNC_BYTE);
        //w = SPI.transfer(PIXY_SYNC_BYTE);
    }
    w <<= 8;
    c = spi->write(c_out);
    //c = SPI.transfer(cout);
    w |= c;
    return w;
}
 
void PixyInterfaceSPI::flushSend()
{
    uint16_t w;
    while(outQ.len) {
        w = getWordHw();
        inQ.write(w);
    }
}
 
template <class BufType> CircularQ<BufType>::CircularQ()
{
    len = 0;
    writeIndex = 0;
    readIndex = 0;
}
 
template <class BufType> bool CircularQ<BufType>::read(BufType *c)
{
    if (len) {
        *c = buf[readIndex++];
        len--;
        if (readIndex == PIXY_BUF_SIZE) {
            readIndex = 0;
        }
        return true;
    } else {
        return false;
    }
}
 
template <class BufType> uint8_t CircularQ<BufType>::freeLen()
{
    return PIXY_BUF_SIZE - len;
}
 
template <class BufType> bool CircularQ<BufType>::write(BufType c)
{
    if (freeLen() == 0) {
        return false;
    }
    buf[writeIndex++] = c;
    len++;
    if (writeIndex == PIXY_BUF_SIZE) {
        writeIndex = 0;
    }
    return true;
}
