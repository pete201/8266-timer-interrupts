/*
This sketch shows examples of using interrupts both using 8266 'built in' timer1, and also the Ticker library

When Ticker fires, the count of times it has fired is output to the Serial Monitor.
Each time Ticker fires, it then fires timer1 which outputs 0.5s later

The code in loop outputs the ISR counts to Serial by inspecting the interrupts flags, 


NOTE - timer1 interrupts may interfere with other functionality (PWM for example) depending on the timer chosen to configure.
ESP8266 has 2 x Timers available:
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
*/

//#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker timer;		// this timer is used to print the number of interrupts every 0.5s
Ticker updateOutput;	// create a Ticker instance that will be used to update the Serial output regularly

const int tickerInterval = 1000;		// interval in ms (since we use attach_ms)
int countTickerInterrupts = 0;
volatile bool printTickerOutput = false;    	// volatile so that optimiser leaves it alone

//timer1 variables
volatile bool printTimer1_Output = false;
int countTimer1Interrupts = 0;



// ISR to Fire when Timer1 is triggered
void ICACHE_RAM_ATTR onTimer1() {
	printTimer1_Output = true;
	
}

// ISR to Fire when Ticker "updateOutput" is triggered.  NOTE that ticker ISRs do not need "ICACHE_RAM_ATTR"
// Ticker ISR then fires timer1 ISR
void tickerUpdateOutput(){
	printTickerOutput = true;
	// Fire timer1 (as using TIM_SINGLE)
	timer1_write(2500000);//12us
}


void setup()
{
	Serial.begin(115200);
	//Initialize Timer1 every 0.5s
	timer1_attachInterrupt(onTimer1); // Add ISR Function
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
	/* Dividers:
		TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
		TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
		TIM_DIV256 = 3  //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
	Reloads:
		TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
		TIM_LOOP	1 //on interrupt the counter will start with the same value again
	*/
	
	// Arm  Timer1 for our 0.5s Interval
	timer1_write(2500000); // 2500000 / 5 ticks per us from TIM_DIV16 == 500,000 us interval 

	// Ticker setup
	updateOutput.attach_ms(tickerInterval, tickerUpdateOutput);	// (time in ms, ISR to call)
}


void loop()
{
    if(printTickerOutput){
        countTickerInterrupts++;
	    Serial.print("Total Ticks from Ticker: ");
	    Serial.println(countTickerInterrupts);
        printTickerOutput = false;
    }
    
	if(printTimer1_Output){
		countTimer1Interrupts++;
		Serial.print("Total Ticks from Timer1: ");
		Serial.println(countTimer1Interrupts);
		printTimer1_Output = false;
	}
}