#include "mbed.h"
 // Purpose of code is to test PI control
 
// Initialize a pins to perform analog input and digital output fucntions
InterruptIn   ain2(p15);
DigitalIn   bin2(p16);
PwmOut mot2(p26);
DigitalOut cs2(p25);
DigitalOut rev2(p24);


InterruptIn   ain1(p28);
DigitalIn   bin1(p27);
PwmOut mot1(p23);
DigitalOut cs1(p22);
DigitalOut rev1(p21);

Ticker pwm_call;

Serial device(p9, p10);  // tx, rx
double ainold;
double binold;

double counter1;
double counter2;
double countold1;
double countold2;
double speed1;
double speed2;
double upi1;
double upi2;
double upi1_old;
double upi2_old;
double e1;
double e2;
double e1_old;
double e2_old;
double speed1_old;
double speed2_old;
double theta1;
double theta2;
double theta1_old;
double theta2_old;
double ts;
double nf;
double kp;
double ki;
double wref1_g;
double wref2_g;
double DC1;
double DC2;
double angv;
double linv;

 void update_count1()
 {
     if (bin1 < 0.5){
            counter1=counter1+1;
        } else {
            counter1=counter1-1;
        }
}

 void update_count2()
 {
     if (bin2 > 0.5){
            counter2=counter2+1;
        } else {
            counter2=counter2-1;
        }
}



double get_pwm1(double wref1, double cnew)
{
     //get angle
     theta1=(3.1415926/300)*2*cnew;
     //get speed
     speed1=(nf*(theta1-theta1_old))-(speed1_old*((nf*ts)-1));
     //get error
     e1=wref1-speed1;
     //Get PI output
     upi1=((kp+(ki*ts))*e1)-(kp*e1_old)+(upi1_old);
     //limit PI output
     if(upi1>0.7){
         upi1=0.7;
     }
     if(upi1<-0.7){
         upi1=-0.7;
     }
     
     //store old values
     upi1_old=upi1;
     e1_old=e1;
     speed1_old=speed1;
     theta1_old=theta1;
     
     //sign and value of PI
     DC1=abs(upi1);
     if(upi1<0){
         rev1=0;
     } else {
         rev1=1;
     }
     return DC1;
}             

double get_pwm2(double wref2, double cnew)
{
     //get angle
     theta2=(3.1415926/300)*2*cnew;
     //get speed
     speed2=(nf*(theta2-theta2_old))-(speed2_old*((nf*ts)-1));
     //get error
     e2=wref2-speed2;
     //Get PI output
     upi2=((kp+(ki*ts))*e2)-(kp*e2_old)+(upi2_old);
     //limit PI output
     if(upi2>0.7){
         upi2=0.7;
     }
     if(upi2<-0.7){
         upi2=-0.7;
     }
     
     //store old values
     upi2_old=upi2;
     e2_old=e2;
     speed2_old=speed2;
     theta2_old=theta2;
     
     //sign and value of PI
     DC2=abs(upi2);
     if(upi2<0){
         rev2=0;
     } else {
         rev2=1;
     }
     return DC2;
}                          

void get_pwm()
{
    mot1=get_pwm1(wref1_g, counter1);
    mot2=get_pwm2(wref2_g, counter2);
    angv=((speed1*0.13)-(0.13*speed2))/0.3;
    linv=((speed1*0.13)+(0.13*speed2))/2;
}

int main(void)
{
    //Motor INIT
    cs1 = 0;
    rev1=1;
    mot1.period_ms(1);
    mot1=0;
    
    cs2 = 0;
    rev2=1;
    mot2.period_ms(1);
    mot2=0;
    
    //PWM init values
    theta1_old=0;
    theta2_old=0;
    speed1_old=0;
    speed2_old=0;
    e1_old=0;
    e2_old=0;
    upi1_old=0;
    upi2_old=0;
    ts=0.0012;
    nf=833;
    kp=0.000073360192199404;
    ki=0.0122266986999007;
    wref1_g=0;
    wref2_g=0;
    linv=0;
    angv=0;
    
    //Init values for counters and speed
    
    wait(1);
    
    counter1=0;
    counter2=0;
    countold1=0;
    countold2=0;
    speed1=0;
    speed2=0;
    
    
    //Set up interrupts
    
    ain1.rise(update_count1);
    ain2.rise(update_count2);
    pwm_call.attach_us(get_pwm,1000);


    wait(1);
    
    
    wref1_g=40;
    wref2_g=40;
    
    wait(3);
   
    device.printf("---------------------------------------");
    device.printf("Speed1 40=%f\n\r",speed1);
    device.printf("Speed2 40=%f\n\r",speed2);
    device.printf("Linear Velocity 40=%f\n\r",linv);
    device.printf("Angular Velocity 40=%f\n\r",angv);
    
    wait(2);

    wref1_g=40;
    wref2_g=20;
    
    wait(3);
   
    device.printf("Speed1 40=%f\n\r",speed1);
    device.printf("Speed2 20=%f\n\r",speed2);
    device.printf("Linear Velocity 40/20=%f\n\r",linv);
    device.printf("Angular Velocity 40/20=%f\n\r",angv);
    
    wait(2);

    wref1_g=-20;
    wref2_g=20;
    
    wait(3);
   
    device.printf("Speed1 -20=%f\n\r",speed1);
    device.printf("Speed2 20=%f\n\r",speed2);
    device.printf("Linear Velocity -20/20=%f\n\r",linv);
    device.printf("Angular Velocity -20/20=%f\n\r",angv);
    
    wait(2);
    
    wref1_g=-20;
    wref2_g=0;
    
    wait(3);
   
    device.printf("Speed1 -20=%f\n\r",speed1);
    device.printf("Speed2 0=%f\n\r",speed2);
    device.printf("Linear Velocity -20/0=%f\n\r",linv);
    device.printf("Angular Velocity -20/0=%f\n\r",angv);
    
    wait(2);
    
    wref1_g=0;
    wref2_g=0;
    
    wait(3);
   
    device.printf("Speed1 0=%f\n\r",speed1);
    device.printf("Speed2 0=%f\n\r",speed2);
    device.printf("Linear Velocity 40=%f\n\r",linv);
    device.printf("Angular Velocity 40=%f\n\r",angv);
    
    wait(2);
}