#include <stdio.h>                                                      
#include <stdlib.h>                                                                                            
#include <xc.h>                                                            
#include <math.h>                                                                                             
#include <p18f4620.h>                                                         
#include <usart.h>  

#pragma config OSC = INTIO67
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE 


#define NS_RED PORTAbits.RA5
#define NS_GREEN PORTBbits.RB0       
                                                                                // Defines the NSPED_SW in PORTA (RA5)  

#define NSLT_RED PORTBbits.RB1
#define NSLT_GREEN PORTBbits.RB2     
                                                                                // Defines the NSLT_SW in PORTD (RB1) 

#define EW_RED PORTBbits.RB4      
#define EW_GREEN PORTBbits.RB5     
                                                                                // Defines the EWPED_SW in PORTB (RB0) 

#define EWLT_RED PORTEbits.RE0    
#define EWLT_GREEN PORTEbits.RE2
                                                                                // Defines the EWLT_SW in PORTD (RB2) 

#define NS_PED PORTAbits.RA1                                                    
#define NS_LT PORTAbits.RA2                                                     

#define EW_PED PORTAbits.RA3                                                        
#define EW_LT PORTAbits.RA4

#define SEC_LED PORTDbits.RD7      
#define MODE_LED PORTBbits.RB7

#define SEG_G PORTEbits.RE1

#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3
                                                                                //identify all ports. 
char number[10] = {0x7F, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18}; //numbers for 7 segment 

//timers

void Wait_Half_Second() 
{                                                                               //Wait half second function 
 T0CON = 0x02;                                                                  // Timer 0, 16-bit mode, prescaler 1:8 
 TMR0L = 0xDB;                                                                  // set the lower byte of TMR 
 TMR0H = 0x0B;                                                                  // set the upper byte of TMR 
 INTCONbits.TMR0IF = 0;                                                         // clear the Timer 0 flag 
 T0CONbits.TMR0ON = 1;                                                          // Turn on the Timer 0 
 while (INTCONbits.TMR0IF == 0);                                                // wait for the Timer Flag to be 1 for done 
 T0CONbits.TMR0ON = 0;                                                          // turn off the Timer 0 
}

void Wait_One_Second() 
{ 
 SEC_LED = 0;                                                                   // First, turn off the SEC LED 
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec) 
 SEC_LED = 1;                                                                   // then turn on the SEC LED 
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec) 
} 


void Wait_N_Seconds (char seconds)                                              //uses internal timer to make user's desired second met.
{ 
char I;                                                                         //identify variable I for counter
 for (I = 0; I< seconds; I++)                                                   //for loop which repeats the waitonesecond function.
 { 
 Wait_One_Second();                                                             //calling wait one second function
 } 
}

                                                                                //set the led's 

void Set_NS(char color)                                                         //sets NS to different colors depending on case
{ 
 switch (color) 
 { 
 case OFF: NS_RED =0;NS_GREEN=0;break;                                          // Turns off the NS LED 
 case RED: NS_RED =1;NS_GREEN=0;break;                                          // Sets NS LED RED 
 case GREEN: NS_RED =0;NS_GREEN=1;break;                                        // sets NS LED GREEN 
 case YELLOW: NS_RED =1;NS_GREEN=1;break;                                       // sets NS LED YELLOW 
 } 
} 

void Set_NSLT(char color)                                                       //sets nslt to different colors depending on case
{ 
 switch (color) 
 { 
 case OFF: NSLT_RED =0;NSLT_GREEN=0;break;                                      // Turns off the NSLT LED 
 case RED: NSLT_RED =1;NSLT_GREEN=0;break;                                      // Sets NSLT LED RED 
 case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;                                    // sets NSLT LED GREEN 
 case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;                                   // sets NSLT LED YELLOW 
 } 
} 

void Set_EW(char color)                                                         //sets ew to different colors depending on case
{ 
 switch (color) 
 { 
 case OFF: EW_RED =0;EW_GREEN=0;break;                                          // Turns off the EW LED 
 case RED: EW_RED =1;EW_GREEN=0;break;                                          // Sets EW LED RED 
 case GREEN: EW_RED =0;EW_GREEN=1;break;                                        // sets EW LED GREEN 
 case YELLOW: EW_RED =1;EW_GREEN=1;break;                                       // sets EW LED YELLOW 
 } 
} 
void Set_EWLT(char color)                                                       //sets EWLT to different colors depending on case 
{ 
 switch (color) 
 { 
 case OFF: EWLT_RED =0;EWLT_GREEN=0;break;                                      // Turns off the EWLT LED 
 case RED: EWLT_RED =1;EWLT_GREEN=0;break;                                      // Sets EWLT LED RED 
 case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;                                    // sets EWLT LED GREEN 
 case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;                                   // sets EWLT LED YELLOW 
 } 
} 

void Init_UART() 
{ 
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
USART_BRGH_HIGH, 25); 
 OSCCON = 0x60;                                                                 //adjusts oscillation speed.
}  

void putch (char c)
{
    while (!TRMT);                      
    TXREG = c;
}                                                                               //Used for terra term

unsigned int get_full_ADC(void) 
{ 
int result; 
ADCON0bits.GO=1;                                                                // Start Conversion 
while(ADCON0bits.DONE==1);                                                      // wait for conversion to be completed 
result = (ADRESH * 0x100) + ADRESL;                                             // combine result of upper byte and 
                                                                                // lower byte into result 
 return result;                                                                 // return the result. 
} 

void Init_ADC(void)
{
    ADCON0 = 0x01;                                                              // initiates inputs for dip switch
    ADCON2 = 0xA9;                                                              //conversion frequency
    ADCON1 = 0x0E;                                                              //Set AN0 and set bit 1 to 0 and bit 0 to 1
                                                                                 
}

void Select_ADC_Channel(char channel)
{
    ADCON0 = channel * 4 + 1;                                                   //shift acon0 to the left 2 bits. determines which port to allow analog input. +1 turns adcon on
}  



void Display_Lower_Digit(char digit)
{
    PORTD = (PORTD & 0x80)  |  number[digit];                                   //Displays lower digits. PortD is masked by 0x80 which gets rid of msb 
}

void Display_Upper_Digit(char digit)
{
    PORTC = number[digit] & 0x3F;                                               //Displays upper digit masks the top 2 digits.
    if ((number[digit] & 0x40) == 0x40) SEG_G = 1;                              //if statement which identifies when seg_g needs to be on or off
    else SEG_G =0;
}

void TURN_7SEG_OFF(char digit)                                                  //This function is important for turning off the 7 segment after the countdown ends.
{
    PORTC = 0xFF;                                                               //Turns upper 7 segment off
    PORTD = 0xFF;                                                               //Turns lower 7 segment off
    SEG_G = 0xFF;                                                               //Turns the g segment off
}
void Activate_Buzzer() 
{ 
PR2 = 0b11111001 ; 
T2CON = 0b00000101 ; 
CCPR2L = 0b01001010 ; 
CCP2CON = 0b00111100 ; 
} 

void Deactivate_Buzzer() 
{ 
CCP2CON = 0x0; 
 PORTBbits.RB3 = 0; 
} 

void Wait_One_Second_With_Beep() 
{ 
 SEC_LED = 1;                                                                   // First, turn on the SEC LED 
Activate_Buzzer();                                                              // Activate the buzzer
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec) 
 SEC_LED = 0;                                                                   // then turn off the SEC LED 
 Deactivate_Buzzer ();                                                          // Deactivate the buzzer 
 Wait_Half_Second();                                                            // Wait for half second (or 500 msec) 
} 

void PED_CONTROL(char Direction, char Num_Sec)                                  //will set which direction pedestrian counter is addressed for
{
    for(char i = Num_Sec-1; i>0; i--)                                           //loop is depicted by num_sec 
    {
        if(Direction == 0)                                                      //if direction is off then
        {
            Display_Upper_Digit(i);                                             //turns on the upper digit countdowns and stops when 7 segment turns 1
            Wait_One_Second_With_Beep();                                        //beep at each digit change
        }
        else
        {                                                                       //if 1 then 
            Display_Lower_Digit(i);                                             //turns on the lower digit. stops when 7 segment turns 1
            Wait_One_Second_With_Beep();                                        //beep at each digit change
        }
    }
    TURN_7SEG_OFF(0);                                                           //turns off the 7 segment.
    Wait_One_Second_With_Beep();                                                //beeps after 7 segment turns off
    
}





void Day_Mode()
{
    TURN_7SEG_OFF(0);
    Set_NS(GREEN);                                                              // Step 1 and Step 2
    Set_NSLT(RED);              
    Set_EW(RED);
    Set_EWLT(RED);
    
    if(NS_PED==1)
    {
        PED_CONTROL(0,8);                                                       // Counts down from 7 to 0
    }
    Wait_N_Seconds(9);
    
    Set_NS(YELLOW);                                                             // Step 3
    Wait_N_Seconds(3);
    
    Set_NS(RED);                                                                // Step 4
    
    if(EW_LT==1)                                                                // Step 5    //both east-west switches have a wait calls while NS dont
    {
        Set_EWLT(GREEN);                                                        // Step 6  
        Wait_N_Seconds(7);
        
        Set_EWLT(YELLOW);                                                       // Step 7 //cant get D4 to work with switch
        Wait_N_Seconds(3);
        
        Set_EWLT(RED);                                                          // Step 8
    }
    
    
    Set_EW(GREEN);                                                              // Step 9
    
    if(EW_PED==1)
    {
        PED_CONTROL(1,9);                                                       // Count from 8 to 0
    
           
    }  
    Wait_N_Seconds(8);                                                          // Step 10
        
        Set_EW(YELLOW);                                                         // Step 11
        Wait_N_Seconds(3);
        
        Set_EW(RED);                                                            // Step 12 

    if(NS_LT==1)                                                                // Step 13
    {
        Set_NSLT(GREEN);                                                        // Step 14
        Wait_N_Seconds(7);
        
        Set_NSLT(YELLOW);                                                       // Step 15
        Wait_N_Seconds(3); 
        
        Set_NSLT(RED);                                                          // Step 16
    }
    

}

void Night_Mode()
{
    Set_NSLT(RED);                                                              //step 1 
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NS(GREEN);
    Wait_N_Seconds(6);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EW_LT==1)                                                                //if EW_LT ==1 then turn light on, cycle to red.
    {
         Set_EWLT(GREEN);
         Wait_N_Seconds(6);
         Set_EWLT(YELLOW);
         Wait_N_Seconds(3);
         Set_EWLT(RED);     
    }
    
    Set_EW(GREEN);                                                              //if EW_LT == 0 then turn EW on, cycle to red.
    Wait_N_Seconds(6);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    Set_EW(RED); 
    
    if(NS_LT == 1)                                                              //If NS_LT ==1 then turn light on, cycle to red.
    {
        Set_NSLT(GREEN);
        Wait_N_Seconds(8);
        Set_NSLT(YELLOW);
        Wait_N_Seconds(3);
        Set_NSLT(RED);  
    }                                                                           //end of loop
}

void Init_TRIS()
{
    TRISA = 0x1F;                                                               //initializes port A ra0-ra4 to input 
    TRISB = 0x00;                                                               //initializes port b as output 
    TRISC = 0x00;                                                               // initializes port c as output
    TRISD = 0x00;                                                               // initializes port d as output
    TRISE = 0x00;                                                               // initializes port e as output
    
}

void main() 
{   
    Init_ADC();                                                                 //this sets analog input
    Init_UART()     ;                                                           //Call UART for Tera term
    Init_TRIS();                                                                //Call TRISA-TRISE
 /*while (1) 
{ 
 for (int i=0;i<4;i++)                                                          //test 1 and 2 
 { 
 Set_NS(i);                                                                     // Set color for North-South direction 
 Set_NSLT(i);                                                                   // Set color for North-South Left-Turn direction 
 Set_EW(i);                                                                     // Set color for East-West direction 
 Set_EWLT(i);                                                                   // Set color for East-West Left-Turn direction 
 Wait_N_Seconds(1);                                                             // call Wait-N-Second routine to wait for 1 second 
 } 
}       for (int i=0; i<4; i++)
      {
          Set_NS(i);
          Set_NSLT(i);
          Set_EW(i);
          Set_EWLT(i);
         Wait_One_Second();
         Wait_One_Second_With_Beep();
  *      }
     PED_Control(0,8);
     PED_Control(1,6);*/
                                                        
   
    while(1)                                                                    //main loop
    { 
      int num_step = get_full_ADC();                                            //reads analog input 1 or the photo resistor
      float voltage = (num_step * 4.88) / 1000;                                 // voltage equation for photo resistor
      printf("Voltage = %f V \r\n",voltage);                                    // print the photo resistor voltage which signifies if it's light or dark out. 


    if (voltage < 2.5)                                                          //if it is light out 
    {
        MODE_LED = 1;                                                           //Turns Mode_LED on.
        Day_Mode();                                                             //call the day mode function which loops through the lights
    }
    else                                                                        //if it is dark outside
    {
        MODE_LED = 0;                                                           //turns mode led off
        Night_Mode();                                                           //Call the night mode function
    }
  }
}