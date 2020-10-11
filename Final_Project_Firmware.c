# FINAL-Firmware-ECE388

/*
******************************************************
Cameron Whittle ECE388
--Final Project code--
This code is written in c for the Atmega328PB and is 
the final firmware for the pick and place 
system. Kaley Lindsay wrote the Ultrasonic Sensor code.
******************************************************
*/

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define MOSI1 PINE3 //Setup for display and MAX7221 driver
#define SCK1 PINC1
#define SS1 PINE2

//PROTOTYPES
void GoDefault();
void WriteToDisplay();
void exec(unsigned char cmd, unsigned char data);
void Loop();
volatile uint8_t tot_overflow;
volatile uint32_t distance;
volatile uint8_t trigger;

//Movement values for all motors
int DEF[6] = {0, 2900, 2900, 1099, 4799, 2900}; //Default location of arm
int mALoc[20] = {4450, 4475, 4100, 4100, 3750, 3750, 3450, 3450, 3100, 3050, 2650, 2650, 2370, 
	2370, 2020, 2050, 1700, 1650, 1250, 1200}; //Motor A locations
int mBLoc[20] = {2900, 3700, 2900, 3700, 2900, 3700, 2900, 3700, 2900, 3700, 2900, 3700, 2900, 
	3700, 2900, 3700, 2900, 3700, 2900, 3700}; //Motor B locations
int mCLoc[20] = {1099, 1900, 1099, 1900, 1099, 1900, 1099, 1900, 1099, 1900, 1099, 1900, 1099, 
	1900, 1099, 1900, 1099, 1900, 1099, 1900}; //Motor C locations
int GmCLoc[20] = {1500, 2500, 1500, 2500, 1500, 2500, 1500, 2500, 1500, 2500, 1500, 2500, 1500, 
	2500, 1500, 2500, 1500, 2500, 1500, 2500}; //Motor C grab position
int mDLoc = 4799; //Motor D locations
int mELoc = 2900; //Motor E locations
int GmELoc = 4799; //Motor E grab locations
int START = 0; //Initialize the display variables
int DEST = 0;
int DEST1 = 0;
int DEST2 = 0;
int START1 = 0;
int START2 = 0;
volatile uint8_t Enable = 0; //For Sensor
volatile uint8_t cont = 0;

//***************************************************************************************************************
void timer0_init()
{
	// set up timer with pre-scaler = 64
	TCCR0B |= (1 << CS01)|(1 << CS00);
	
	// initialize counter
	TCNT0 = 0;
}
//***************************************************************************************************************
int main(void) 
{
  //TIMER1 Setup - Fast PWM - NonInverting - TOP of ICRn - PreScaler of /8
  //mA on PB1 - OCR1A
  //mB on PB2 - OCR1B
  DDRB |= (1 << DDB1) | (1 << DDB2);
  TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
	
  //TIMER3 Setup - Fast PWM - NonInverting - TOP of ICRn - PreScaler of /8
  //mC on PD0 - OCR3A
  //mD on PD2 - OCR3B
  DDRD |= (1 << DDD0) | (1 << DDD2);
  TCCR3A |= (1 << WGM31) | (1 << COM3A1) | (1 << COM3B1);
  TCCR3B |= (1 << WGM32) | (1 << WGM33) | (1 << CS31);
	
  //TIMER4 Setup - Fast PWM - NonInverting - TOP of ICRn - PreScaler of /8
  //mE on PD1 - OCR4A
  DDRD |= (1 << DDD1); 
  TCCR4A |= (1 << WGM41) | (1 << COM4A1); 
  TCCR4B |= (1 << WGM43) | (1 << WGM42) | (1 << CS41);
  
  DDRC   =  0xFF;						// Port C all output but C5
  DDRC  &= ~(1<<DDC5);				// Set Pin C5 as input to read Echo
  PORTC |= (1<<PORTC5);				// Enable pull up on C5
  PORTC &= ~(1<<PORTC4);	            // Init C4 as low (trigger)

  cli();
  PCICR = (1<<PCIE1);					// Enable PCINT[14:8] we use pin C5 which is PCINT13
  PCMSK1 = (1<<PCINT13);				// Enable C5 interrupt
  sei();								// Enable Global Interrupts

  distance = 0;
  trigger  = 0;
  
  PORTD |= (1 << PIND3) | (1 << PIND4) | (1 << PIND5); //GO on PD5, START and DEST on PD3&4
  
  ICR1 = 39999; //Set TOPs and start PWMs
  ICR3 = 39999;
  ICR4 = 39999; 
  GoDefault();
  
  //Pick and Place System is in operation
  Loop();
  return 0;
}

//***************************************************************************************************************
void GoDefault() //Arm moves to default location
{
  OCR1B = DEF[2];
  _delay_ms(500);
  OCR3A = DEF[3];
  _delay_ms(500);
  OCR3B = DEF[4];
  _delay_ms(500);
  OCR4A = DEF[5];
  _delay_ms(500);
  OCR1A = DEF[1];
  _delay_ms(500);
  OCR1B = DEF[2];
  _delay_ms(500);
	
}
//***************************************************************************************************************
void Loop() 
{
    START = 0;
    DEST = 0;
    PORTC |= (1 << PINC0); //LED on
    while(!(bit_is_clear(PIND, PIND5))) //Controller is in use
    {
	  if(bit_is_clear(PIND, PIND3)) //START loc
	  {
		  START++;
		  if(START >= 20)
		  {
			  START = 0;
		  }
		  while(bit_is_clear(PIND, PIND3)); //increments only once
	  }
	  _delay_ms(1);
	  if(bit_is_clear(PIND, PIND4)) //DEST loc
	  {
		  DEST++;
		  if(DEST >= 20)
		  {
			  DEST = 0;
		  }
		  while(bit_is_clear(PIND, PIND4)); //increments only once
	  }
	  _delay_ms(1);
	  WriteToDisplay();
	}
	
    if(bit_is_clear(PIND, PIND5)) //Start button is pressed
    {
		sei();
	        PORTC &= ~(1 << PINC0); //LED off
		//Go to START loc
		OCR1A = mALoc[START]; //mA 
		_delay_ms(500);  
		OCR3A = mCLoc[START]; //mC
		_delay_ms(500);
		OCR1B = mBLoc[START]; //mB
		_delay_ms(500);
		OCR3B = mDLoc; //mD
		_delay_ms(500);
		OCR4A = mELoc; //mE
		_delay_ms(1500);
		
		cont=0;         //start distance measurement, active low
		trigger = 0;   //used to control first initialization (done once only)
		Enable = 0;  //Allows to grab items, active high

		while(cont == 0){    	
		//While cont = 0, stay in while loop till you get a valid distance, or it times out (no object found)
		if (trigger == 0)
		    {
			// send command to trigger
			PORTC &= ~(1<<PORTC4);	        // Set trigger low
			_delay_us(2);		        // for 20uS
			
			timer0_init();			//Timer 0 is an 8 bit counter
			
			PORTC |= (1<<PORTC4);		// Set trigger high
			_delay_us(10);			// for 20uS
			PORTC &= ~(1<<PORTC4);	        // Set trigger low
			//_delay_ms(50);
			trigger = 1;
		    }
		}
		
		if(Enable == 1)
		{	
		  //Grab Object
		  OCR3A = GmCLoc[START]; //mCGrab
		  _delay_ms(500);
		  OCR4A = GmELoc; //mEGrab
		  _delay_ms(500);
		  //Enable = 0;	
		}
		else //Dont move
		{
		  GoDefault();
         	  Loop();		  
		}
		
		//Go to DEST loc
		if((START%2 == 0) && (DEST%2 !=0)) //CASE: If even start to odd loc
		{
		  OCR1A = mALoc[DEST]; //mA
		  _delay_ms(500); 
		  OCR3A = GmCLoc[DEST]; //mC
		  _delay_ms(500);	
		  OCR1B = mBLoc[DEST]; //mB
		  _delay_ms(500);
		  OCR3B = mDLoc; //mD
		  _delay_ms(500);
		  OCR4A = GmELoc; //mE
		  _delay_ms(500);
		}
		else if((START%2 != 0) && (DEST%2 ==0))
		{
		  OCR1A = mALoc[DEST]; //mA
		  _delay_ms(500);
		  OCR1B = mBLoc[DEST]; //mB
		  _delay_ms(500);
		  OCR3A = GmCLoc[DEST] + 65; //mC
		  _delay_ms(500);
		  OCR3B = mDLoc; //mD
		  _delay_ms(500);
		  OCR4A = GmELoc; //mE
		  _delay_ms(500);	
		}
		else
		{
		  OCR1A = mALoc[DEST]; //mA
		  _delay_ms(500);
		  OCR1B = mBLoc[DEST]; //mB
		  _delay_ms(500);
		  OCR3A = GmCLoc[DEST]; //mC
		  _delay_ms(500);
		  OCR3B = mDLoc; //mD
		  _delay_ms(500);
		  OCR4A = GmELoc; //mE
		  _delay_ms(500);	
		}
		//Drop Object
		OCR4A = mELoc; //mE
		_delay_ms(500);
		OCR3A = mCLoc[DEST]; //mC
		_delay_ms(500);
		//Go back to start
		GoDefault();
		Loop();
	}
}

//***************************************************************************************************************
void exec(unsigned char cmd, unsigned char data) //Send data packet to MAX7221
{
	PORTE &= ~(1 << SS1);

	SPDR1 = cmd; //send cmd, wait for end
	while(!(SPSR1 & (1 << SPIF1)));
	SPDR1 = data; //send data, wait for end
	while(!(SPSR1 & (1 << SPIF1)));

	PORTE |= (1 << SS1);
}
//***************************************************************************************************************
void WriteToDisplay() //Displays the values held in START and DEST
{
	DDRE |= (1 << MOSI1) | (1 << SS1); //initializes SPI packet
	DDRC |= (1 << SCK1);
	SPCR1 |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	exec(0x09, 0b00001111); //enables first 4 digits
	exec(0x0B, 0x04);
	exec(0x0A, 0xFF); //sets brightness
	exec(0x0C, 0x01);
	DEST1 = DEST / 10;
	DEST2 = DEST % 10;
	START1 = START / 10;
	START2 = START % 10;
	exec(0x01, START1); //Sends all 4 digits to MAX7221
	exec(0x02, START2);
	exec(0x03, DEST1);
	exec(0x04, DEST2);	
}
//***************************************************************************************************************
ISR(TIMER0_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
}
//***************************************************************************************************************
ISR(PCINT1_vect)
{
	TCCR0B = 0;
	if ((PINC & (1<<PINC5))==1)				// Checks on rising edge of echo
	{
		// initialize timer
		tot_overflow = 0;
	}
	else // falling edge
	{
		distance = TCNT0+256*tot_overflow;
		
		if (distance<=110)    //if distance to target is seen, (within 3 inches or 7.5 cm)
		{
			//PORTC |= (1 << PINC0); //LED ON
			Enable = 1;      //Good value, enable grabber
			cont = 1;        //break out of the while loop
		}
		else
		{
			Enable = 0;	  //Do not enable grabber
			cont = 1;          //break out of the while loop    
		}
	}
}
//***************************************************************************************************************
