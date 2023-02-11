#define F_CPU 8000000UL
#define D4 eS_PORTC0
#define D5 eS_PORTC1
#define D6 eS_PORTC2
#define D7 eS_PORTC3
#define RS eS_PORTC6
#define EN eS_PORTC7
#define snprintf _snprintf

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "lcd.h"

#define Trigger_pin PA0	/* Trigger pin */
#define output_pin PA7

int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
TimerOverflow++;	/* Increment Timer Overflow count */
}

long long int get_digit(double n)
{
	long long int a = floor(n);
	long long digit = 0;
	while(a > 0)
	{
		a = a/10;
		digit++;
	}
	
	return digit;
}

int main(void)
{
long count;
double distance;

DDRA = 0b10000001; /* Make trigger pin as output and PA7 as output to light up the LED*/
PORTD = 0xFF;		/* Turn on Pull-up */
DDRC = 0xFF;
Lcd4_Init();

_delay_ms(10);

sei();			/* Enable global interrupt */
TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
TCCR1A = 0;		/* Set all bit to zero Normal operation */

_delay_ms(10);

while(1)
{
/* Give 10us trigger pulse on trig. pin to HC-SR04 */
PORTA |= (1 << Trigger_pin);
_delay_us(10);
PORTA &= (~(1 << Trigger_pin));

TCNT1 = 0;	/* Clear Timer counter */
TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */

/*Calculate width of Echo by Input Capture (ICP) */

while ((TIFR & (1 << ICF1)) == 0);/* Wait for rising edge */
TCNT1 = 0;	/* Clear Timer counter */
TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */
TimerOverflow = 0;/* Clear Timer overflow count */

bool ok = true;

while ((TIFR & (1 << ICF1)) == 0)/* Wait for falling edge */
{
	if(TimerOverflow >= 1 || ICR1 >= 60000)
	{
		ok = false;
		break;
	}
}

if(!ok)
{
	PORTA &= (~(1<<output_pin));
	
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,1);
	Lcd4_Write_String("Distance: OFR!!!");
	//_delay_ms(10);
	continue;
}

count = ICR1 + (65535 * TimerOverflow);	/* Take count */
/* 8MHz Timer freq, sound speed =343 m/s */
distance = (double)count / 466.47;

if(distance <= 40 && distance>= 2)
{
	PORTA |= (1<<output_pin);
}

else
{
	PORTA &= (~(1<<output_pin));	
}

Lcd4_Clear();

double points = distance-floor(distance);

long long digit = get_digit(distance);
int ara[digit];
int dis = floor(distance);


for(int i=0;i<digit;i++)
{
	ara[i] = dis%10;
	dis = dis/10;
}

Lcd4_Set_Cursor(1,1);
Lcd4_Write_String("Distance: ");
for(int i=digit-1;i>=0;i--)
{
	Lcd4_Write_Char(ara[i]+'0');
}

Lcd4_Write_Char('.');
points*=10;
int val = floor(points);
Lcd4_Write_Char(val+'0');
points-=val;
points*=10;
val = floor(points);
Lcd4_Write_Char(val+'0');
//_delay_ms(10);

//_delay_ms(2000);
}


}