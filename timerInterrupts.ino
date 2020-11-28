/*
This sketch shows examples of using interrupts with the Ticker library
https://links2004.github.io/Arduino/dd/de3/class_ticker.html

The intention is to mimic a button debounce.
I use a 1s regular timer is simulating a button press (Ticker buttonPress)
(this would usually be attachInterrupt from a pin) which calls the onDebounce ISR.
The onDebounce ISR then calls a Ticker debounce ISR that holds the code to run when the button is pressed.

The code in loop sends output to the Serial when the interrupts flags are set.
Each time the button press timer fires, the Serial outputs the button press count
The button press ISR triggers the debounce Ticker and ISR, and Serial output of debounce count will follow button count.
*/


#include <Ticker.h>

Ticker buttonPress;		// a Ticker instance that simulates pressing a button regularly
Ticker debounce;		// this timer is used to delay by a short period


const int buttonInterval = 1000;				// interval in ms (since we use attach_ms)
int countButtonPress = 0;
volatile bool printButtonOutput = false;    	// volatile so that optimiser leaves it alone

const int debounceInterval = 300;				// interval in ms (since using once_ms)
int countDebounceInterrupts = 0;
volatile bool printDebounceOutput = false;    	// volatile so that optimiser leaves it alone


// ISR to Fire when Ticker "buttonPress" is triggered.  NOTE that ticker ISRs do not need "ICACHE_RAM_ATTR"
void onButtonPress(){
	printButtonOutput = true;
	// this for loop is not necessary, but simulates multiple 'bouncing' triggers from the button
	for(int n=0; n<5; n++){	
		debounce.once_ms(debounceInterval, onDebounce);	// (time in ms, ISR to call)
	}
}

// ISR to Fire when Ticker "debounce" is triggered.  NOTE that ticker ISRs do not need "ICACHE_RAM_ATTR"
void onDebounce(){
	printDebounceOutput = true;
	// here is where your code to read the button state would go
	// e.g buttonState = digitalRead(buttonPin);
}


void setup()
{
	Serial.begin(115200);

	// activate the buttonPress Ticker 
 	buttonPress.attach_ms(buttonInterval, onButtonPress);	// (time in ms, ISR to call)
}


void loop()
{
    if(printButtonOutput){
        countButtonPress++;
	    Serial.print("Total Ticks from Ticker: ");
	    Serial.println(countButtonPress);
        printButtonOutput = false;
    }
    
	if(printDebounceOutput){
		countDebounceInterrupts++;
		Serial.print("Total Ticks from Debounce: ");
		Serial.println(countDebounceInterrupts);
		printDebounceOutput = false;
	}
}