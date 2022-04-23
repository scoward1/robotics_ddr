// Authors: Eric Zundel 3558914, Sienna Coward 3593593
// Course: ECE 4333

/* NOTES: use putty to send chars without pressing enter. connect via bluetooth to hc-06, 
got to more bluetooth settings, COM ports, then find the hc-06 outgoing port. open putty, 
use the serial option, and then select the correct COM port
*/

// TODO: look into using an interrupt for the to receive faster/more often
// TODO: put bluetooth setup in a pixy.h file and then use that to print to putty

#include "mbed.h"
#include "SPI.h"
#include "Pixy.h"

// SPI
#define SPI_MOSI_PIN p5
#define SPI_MISO_PIN p6
#define SPI_SCK_PIN p7
#define SPI_SS_PIN p8
//Serial serial(USBTX, USBRX);
SPI spi(SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN);
PixySPI pixy(&spi, &blue);

// motor output setup
PwmOut mot2(p26);
DigitalOut cs2(p25);
DigitalOut rev2(p24);
PwmOut mot1(p23);
DigitalOut cs1(p22);
DigitalOut rev1(p21);
DigitalOut statusLED1(LED1);
DigitalOut statusLED2(LED2);

// led setup 
/*
DigitalOut led1(LED1);          // mbed LEDs - using just for bluetooth testing at the moment
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
*/

char c;
bool mode;                      // following mode vs rc car mode
 
int main() 
{    
    int response = 0;
    ddrControl(pixy);
    
    // activate chip selects
    cs1 = 0;
    cs2 = 0;
    mot1.period_ms(1);
    mot2.period_ms(1); 
    
    mode = true;
    
    // read in the character, toggle lights accordingly
    while (1) 
    {
        while(mode){
            //testing the pixy spi connection
            //pixy_spi.lock();
            //response = pixy_spi.write(0xFF);
            //pixy_spi.unlock();

            if(response){
                led1 = !led1;
            }
            else{
                led2 = !led2;
            }
            wait(1);

            if (blue.readable()){
                c = blue.getc();        // read in character      
                switch(c){
                    case 'a':           // right motor only (turn left)
                        led1 = !led1;
                        rev1 = 0;
                        rev2 = 0;
                        mot1 = 0;
                        mot2 = 0.3;
                        break;
                    case 's':           // both motors back
                        led2 = !led2;
                        rev1 = 1;
                        rev2 = 1;
                        mot1 = 0.3;
                        mot2 = 0.3;
                        break;
                    case 'd':           // left motor only (turn right)
                        led3 = !led3;
                        rev1 = 0;
                        rev2 = 0;
                        mot1 = 0.3;
                        mot2 = 0;
                        break;
                    case 'w':
                        led4 = !led4;   // both motors forward
                        rev1 = 0;
                        rev2 = 0;
                        mot1 = 0.3;
                        mot2 = 0.3;
                        break;
                    case 'l':
                        led1 = !led1;
                        led2 = !led2;
                        led3 = !led3;
                        led4 = !led4;
                        mode = false;
                        mot1 = 0;
                        mot2 = 0;
                        break;
                    case ' ':
                        mot1 = 0;
                        mot2 = 0;
                    default:
                        break;
                }
            }
        }
        while(!mode){
            if (blue.readable()){
                c = blue.getc();        // read in character
                switch(c){
                    case 'l':
                        mode = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
