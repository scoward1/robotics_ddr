#ifndef _PIXY_H
#define _PIXY_H

#include "TPixy.h"
#include "TPixyInterface.h"
#include <cstdint>

// bluetooth setup
Serial blue(p9, p10);           // TX, RX communication from/to the bluetooth module

// led setup 
DigitalOut led1(LED1);          // mbed LEDs - using just for bluetooth testing at the moment
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

#define X_CENTER        ((PIXY_MAX_X-PIXY_MIN_X)/2)
#define Y_CENTER        ((PIXY_MAX_Y-PIXY_MIN_Y)/2)
 
/** The SPI Pixy interface for the Pixy camera
  * @code
  * #include "Pixy.h"
  * Serial serial(USBTX, USBRX);
  * SPI spi(p5, p6, p7);
  * PixySPI pixy(&spi, &serial);
  *
  * int main() {
  *     runPanTiltDemo();
  * }
  * @endcode
  */
  
class PixySPI : public TPixy<PixyInterfaceSPI>
{
public:
    /** Constructor for the PixySPI class
      * Instantiates a Pixy object that communicates via an SPI connection
      * @param SPI* spi the pointer to the SPI connection
      * @param Serial* serialOut the optional serial output pointer to print out Pixy camera data
      * @param int arg an optional integer argument used for custom implementations of the Pixy library
      */
    PixySPI(SPI* spi, Serial* serialOut = NULL, uint16_t arg = PIXY_DEFAULT_ARGVAL) :
        TPixy<PixyInterfaceSPI>((new PixyInterfaceSPI(spi)), serialOut, arg) {}
};
    
class ServoLoop
{
public:
    /** Constructor for a ServoLoop object
      * Creates a ServoLoop object for easy control of the Pixy servos
      * @param pgain the proportional gain for the control
      * @param dgain the derivative gain for the control
      */
    ServoLoop(int32_t pgain, int32_t dgain);
    /** Update method for a ServoLoop object
      * Updates the m_pos variable for setting a Pixy servo
      * @param error the error between the center of the camera and the position of the tracking color
      */
    void update(int32_t error);
 
    int32_t m_pos;
    int32_t m_prevError;
    int32_t m_pgain;
    int32_t m_dgain;
};
 
/** Basic Pan/Tilt Demo code
  * Runs the Pan/Tilt demo code
  * @param pixy the pointer to the pixy object to run the demo on
  * @param serial the optional serial pointer to display output to
  */


template <class TPixyInterface> 
int32_t ddrControl(TPixyInterface pixy, Serial* serial = NULL)
{
    //ServoLoop panLoop(-150, -300);
    //ServoLoop tiltLoop(200, 250);
    //static int i = 0;
    //int j;
    uint16_t blocks;
    int32_t xError, yError;
    pixy.init();
    blocks = pixy.getBlocks();
    if (blocks) {
        xError = X_CENTER - pixy.blocks[0].x;
        yError = pixy.blocks[0].y - Y_CENTER;
        blue.printf("xError: %d\t", xError);
        blue.printf("yError: %d\n\r", yError);

        return(xError);
 
            //panLoop.update(panError);
            //tiltLoop.update(tiltError);
 
            //pixy.setServos(panLoop.m_pos, tiltLoop.m_pos);
            //i++;
 
            /*if (i % 50 == 0 && serial != NULL) {
                blue.printf("Detected %d:\n", blocks);
                //toPC.printf(buf);
                for (j = 0; j < blocks; j++) {
                    blue.printf("  block %d: ", j);
                    pixy.printBlock(pixy.blocks[j]);
                }
            }*/
    }
}
#endif
