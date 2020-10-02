// https://www.visualmicro.com/page/Timer-Interrupts-Explained.aspx

/*
NOTE - Timer interrupts may interfere with other functionality (PWM for example) depending on the timer chosen to configure.
ESP8266 gas 2 x Timers available:
    0 (Used by WiFi)
    1 is available to configure.

Calculations
As these timers are hardware based, all timing is related to the clock of the timer:
ESP8266 - 80Mhz

timer speed (Hz) = Timer clock speed (Mhz) / prescaler
The prescaler / divider is what the above frequency is divided by to form a "tick" of the timer (increment its counter).  
The ISR is then configured to fire after a specific number of ticks.

The prescaler is used, as the timers can only store up to 8/16 bits in their counters, meaning they would overflow every 
256/16000000 s (16us) for 8 bit counters, and 65536 / 16000000 s (4us) for 16 bit counters, which is often far more than needed. 
The prescaler allows this to be scaled to allow longer intervals.

This example simply shows the count of times it has fired in the Serial Monitor, and is configured to fire once per second.

The code in loop is simply to output to the user, and like with External Interrupts, loop can simply inspect the interrupts flag, 
and perform an action based on this as needed
*/

//#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker timer;

volatile int interrupts;    // volatile so that optimiser leaves it alone

// ISR to Fire when Timer is triggered
void ICACHE_RAM_ATTR onTime() {
	interrupts++;
	Serial.print("Total Ticks:");
	Serial.println(interrupts);
	// Re-Arm the timer as using TIM_SINGLE
	timer1_write(2500000);//12us
}


void setup()
{
	Serial.begin(115200);
	//Initialize Ticker every 0.5s
	timer1_attachInterrupt(onTime); // Add ISR Function
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
	/* Dividers:
		TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
		TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
		TIM_DIV256 = 3  //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
	Reloads:
		TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
		TIM_LOOP	1 //on interrupt the counter will start with the same value again
	*/
	
	// Arm the Timer for our 0.5s Interval
	timer1_write(2500000); // 2500000 / 5 ticks per us from TIM_DIV16 == 500,000 us interval 
}


void loop()
{
}