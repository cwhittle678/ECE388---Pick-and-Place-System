# Controller Prototype for 328PB in c
# Full Controller operation in c for 328PB

/*
******************************************************
Cameron Whittle ECE388
Full Controller Prototype
This code is written in .c for the Atmega328PB and is 
the full prototype of the arm controller. This includes 
the 4 digit 7 segment display as well as 3 buttons and 
a light. The functionality is the same as described in 
the project specifications 
******************************************************
*/

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>

#define MOSI1 PINE3
#define SCK1 PINC1
#define SS1 PINE2
int START = 0;
int DEST = 0;

int main(void)
{
	DDRC |= (1 << PINC0); //LED on PC0
    PORTC |= (1 << PINC5) | (1 << PINC4) | (1 << PINC3); //Start on PC3, left and right on PC5&4
	PORTC |= (1 << PINC0); //LED ON
	while(!(bit_is_clear(PINC, PINC3))) //Controller is in use
	  {	
		if(bit_is_clear(PINC, PINC5)) //START loc
		  {
		    START++;
			if(START >= 20)
			  {
			    START = 0;
			  }
		    while(bit_is_clear(PINC, PINC5)); 
		  }
		_delay_ms(1);
		if(bit_is_clear(PINC, PINC4)) //DEST loc
		  {
			  DEST++;
			  if(DEST >= 20)
			  {
				  DEST = 0;
			  }
		    while(bit_is_clear(PINC, PINC4)); 
		  }
		_delay_ms(1);

		WriteToDisplay();
	  }
	PORTC &= ~(1 << PINC0); //LED OFF - arm in motion
	return (0);
}
//**************************************************************************************************
void exec(unsigned char cmd, unsigned char data) //Send data packet to MAX7221
{
	PORTE &= ~(1 << SS1);

	SPDR1 = cmd;
	while(!(SPSR1 & (1 << SPIF1)));
	SPDR1 = data;
	while(!(SPSR1 & (1 << SPIF1)));

	PORTE |= (1 << SS1);
}
//**************************************************************************************************
void WriteToDisplay() //Displays the values held in START and DEST
{
  DDRE |= (1 << MOSI1) | (1 << SS1);
  DDRC |= (1 << SCK1);
  SPCR1 |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  exec(0x09, 0b00001111);
  exec(0x0B, 0x04);
  exec(0x0A, 0xFF);
  exec(0x0C, 0x01);
  switch(START)
  {
    case(0):
	exec(0x01, 0x00);
	exec(0x02, 0x00);
	break;

	case(1):
	exec(0x01, 0x00);
	exec(0x02, 0x01);
	break;

	case(2):
	exec(0x01, 0x00);
	exec(0x02, 0x02);
	break;

	case(3):
	exec(0x01, 0x00);
	exec(0x02, 0x03);
	break;

	case(4):
	exec(0x01, 0x00);
	exec(0x02, 0x04);
	break;

	case(5):
	exec(0x01, 0x00);
	exec(0x02, 0x05);
	break;

	case(6):
	exec(0x01, 0x00);
	exec(0x02, 0x06);
	break;

	case(7):
	exec(0x01, 0x00);
	exec(0x02, 0x07);
	break;

	case(8):
	exec(0x01, 0x00);
	exec(0x02, 0x08);
	break;

	case(9):
	exec(0x01, 0x00);
	exec(0x02, 0x09);
	break;

	case(10):
	exec(0x01, 0x01);
	exec(0x02, 0x00);
	break;

	case(11):
	exec(0x01, 0x01);
	exec(0x02, 0x01);
	break;

	case(12):
	exec(0x01, 0x01);
	exec(0x02, 0x02);
	break;

	case(13):
	exec(0x01, 0x01);
	exec(0x02, 0x03);
	break;

	case(14):
	exec(0x01, 0x01);
	exec(0x02, 0x04);
	break;

	case(15):
	exec(0x01, 0x01);
	exec(0x02, 0x05);
	break;

	case(16):
	exec(0x01, 0x01);
	exec(0x02, 0x06);
	break;

	case(17):
	exec(0x01, 0x01);
	exec(0x02, 0x07);
	break;

	case(18):
	exec(0x01, 0x01);
	exec(0x02, 0x08);
	break;

	case(19):
	exec(0x01, 0x01);
	exec(0x02, 0x09);
	break;
  }

  switch(DEST)
  {
	  case(0):
	  exec(0x03, 0x00);
	  exec(0x04, 0x00);
	  break;

	  case(1):
	  exec(0x03, 0x00);
	  exec(0x04, 0x01);
	  break;

	  case(2):
	  exec(0x03, 0x00);
	  exec(0x04, 0x02);
	  break;

	  case(3):
	  exec(0x03, 0x00);
	  exec(0x04, 0x03);
	  break;

	  case(4):
	  exec(0x03, 0x00);
	  exec(0x04, 0x04);
	  break;

	  case(5):
	  exec(0x03, 0x00);
	  exec(0x04, 0x05);
	  break;

	  case(6):
	  exec(0x03, 0x00);
	  exec(0x04, 0x06);
	  break;

	  case(7):
	  exec(0x03, 0x00);
	  exec(0x04, 0x07);
	  break;

	  case(8):
	  exec(0x03, 0x00);
	  exec(0x04, 0x08);
	  break;

	  case(9):
	  exec(0x03, 0x00);
	  exec(0x04, 0x09);
	  break;

	  case(10):
	  exec(0x03, 0x01);
	  exec(0x04, 0x00);
	  break;

	  case(11):
	  exec(0x03, 0x01);
	  exec(0x04, 0x01);
	  break;

	  case(12):
	  exec(0x03, 0x01);
	  exec(0x04, 0x02);
	  break;

	  case(13):
	  exec(0x03, 0x01);
	  exec(0x04, 0x03);
	  break;

	  case(14):
	  exec(0x03, 0x01);
	  exec(0x04, 0x04);
	  break;

	  case(15):
	  exec(0x03, 0x01);
	  exec(0x04, 0x05);
	  break;

	  case(16):
	  exec(0x03, 0x01);
	  exec(0x04, 0x06);
	  break;

	  case(17):
	  exec(0x03, 0x01);
	  exec(0x04, 0x07);
	  break;

	  case(18):
	  exec(0x03, 0x01);
	  exec(0x04, 0x08);
	  break;

	  case(19):
	  exec(0x03, 0x01);
	  exec(0x04, 0x09);
	  break;
  }
}
//**************************************************************************************************
