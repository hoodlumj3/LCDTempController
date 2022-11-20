
#include "ButtonDebounce.h"

//
// Buttons DeBounce Stuff
// 
// 1) Put buttonsInit() in setup()
// 2) Put buttonDecoder() in loop()
// 3) Alter buttonDecoder() and put your code in there
////////////////////////////////////////////////////////////////////////////

//
// some self explanatory defines
//
#define debounceDelay 20
#define DCgap 150               // max ms between clicks for a double click event
#define holdTime 1000           // ms hold period: how long to wait for hold event
#define longHoldTime 2000       // ms long hold period: how long to wait for longhold event
#define BUTTONCOUNT 1


//
// helper macros
//
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


//
// button status, each buttons status will be one of these.
//
typedef enum {

	joyButtonNone = 0,
	joyButtonClick,
	joyButtonDoubleClick,
	joyButtonHold,
	joyButtonLongHold

} joyButton_ClickType;




//
// fill this array with the unique ids (from above) of all the buttons you want to react to
//
const uint8_t buttonIds[]{
	4
	
};

//
// button data structures
//
struct debounceButton {

	int inPin = 2;

	int status = joyButtonNone;               // status of button, will be one of enum joyButton_Type (check this for button trigger events)
	boolean buttonVal = HIGH;   // value read from button
	boolean buttonLast = HIGH;  // buffered value of the button's previous state
	boolean DCwaiting = false;  // whether we're waiting for a double click (down)
	boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
	boolean singleOK = true;    // whether it's OK to do a single click
	long downTime = -1;         // time the button was pressed down
	long upTime = -1;           // time the button was released
	boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
	boolean waitForUp = false;        // when held, whether to wait for the up event
	boolean holdEventPast = false;    // whether or not the hold event happened already
	boolean longHoldEventPast = false;// whether or not the long hold event happened already

};

debounceButton buttonData[BUTTONCOUNT];

int buttonState = 0;

//
// function : int buttonRefresh()
// call this once every 50ms via main loop
// iterates over the buttons ids array and updates 
// input
//   none
//
// output 
//   (int) but effectivly its a true/false value - check it, if not zero then 1 or more buttons were pressed
//

int buttonRefresh()
{
	for (int i = 0; i < BUTTONCOUNT; i++) {
		buttonState += (checkButton(&buttonData[i]) != 0 ? 1 : 0);
	}
	return buttonState;
}

//
// function : int buttonStatus(int buttonid)
// call (poll) this to get the status of a specific button whenever buttonRefresh() returns a non zero value.
// inputs
//   buttonid: button index you want to check
//  
// output
//   (int) this value will be one of 5 values defined in enum joyButton_ClickType 
//

int buttonStatus(int buttonid)
{
	return buttonData[buttonid].status;
}

//
// function : void buttonsInit()
// call this in setup() - initiates the buttonData structure.
//  
// Inputs:
//   none
//
// Outputs: 
//	 none
//	
void buttonsInit()
{
	for (int i = BUTTONCOUNT - 1; i >= 0; --i) {
		buttonData[i].inPin = buttonIds[i];
	}
	buttonState = 0;
}

//
// function : int checkButton(struct debounceButton *butt)
//   Update status of a button configured within the passed buttonData structure 
//
// Inputs :
//   An pointer to an initialised buttonData structure, specifically indexed.
// 
//
// Outputs :
//   status - the passed in buttons status.
//
//
int checkButton(struct debounceButton *butt) {
	// ME:found and modified from
	// http://forum.arduino.cc/index.php?topic=14479.0
	//

	//int reading = digitalRead(butt->inPin);
	//
	// figure out
	// button click
	// button double click
	// button hold
	// button long hold
	//
	//
	butt->status = 0;
	butt->buttonVal = digitalRead(butt->inPin);

	//butt->buttonVal = HIGH;


	//Serial.println("pin:" + String(butt->inPin) + ":" + butt->buttonVal + ":" + millis());

	// Button pressed down
	if (butt->buttonVal == LOW && butt->buttonLast == HIGH && (millis() - butt->upTime) > debounceDelay)
	{
		butt->downTime = millis();
		butt->ignoreUp = false;
		butt->waitForUp = false;
		butt->singleOK = true;
		butt->holdEventPast = false;
		butt->longHoldEventPast = false;
		if ((millis() - butt->upTime) < DCgap && butt->DConUp == false && butt->DCwaiting == true)  butt->DConUp = true;
		else  butt->DConUp = false;
		butt->DCwaiting = false;
	}
	// Button released
	else {
		if (butt->buttonVal == HIGH && butt->buttonLast == LOW && (millis() - butt->downTime) > debounceDelay) {
			if (!butt->ignoreUp) {
				butt->upTime = millis();
				if (butt->DConUp == false) {
					butt->DCwaiting = true;
				}
				else {
					if (!butt->holdEventPast) {
						butt->status = joyButtonDoubleClick;
						butt->DConUp = false;
						butt->DCwaiting = false;
						butt->singleOK = false;
					}
					else {
						if (butt->longHoldEventPast) {
							butt->status = joyButtonLongHold;
						}
					}
				}
			}
			else {
				if (!butt->longHoldEventPast) {
					butt->status = joyButtonHold;
				}
			}
		}
	}
	// Test for normal click event: DCgap expired
	if (butt->buttonVal == HIGH && (millis() - butt->upTime) >= DCgap && butt->DCwaiting == true && butt->DConUp == false && butt->singleOK == true && butt->status != joyButtonDoubleClick)
	{
		butt->status = joyButtonClick;
		butt->DCwaiting = false;
	}

	// Test for hold
	if (butt->buttonVal == LOW && (millis() - butt->downTime) >= holdTime) {
		// Trigger "normal" hold
		if (!butt->holdEventPast)
		{
			butt->waitForUp = true;
			butt->ignoreUp = true;
			butt->DConUp = false;
			butt->DCwaiting = false;
			//downTime = millis();
			butt->holdEventPast = true;
		}
		// Trigger "long" hold
		if ((millis() - butt->downTime) >= longHoldTime)
		{
			if (!butt->longHoldEventPast) {
				butt->status = joyButtonLongHold;
				butt->longHoldEventPast = true;
			}

		}
	}

	butt->buttonLast = butt->buttonVal;

	//    Serial.print("event:");
	//    Serial.println(event);

	return butt->status;

}



long buttonMillis = 0;

String joyClickNames[5] = { "None", "Click", "DoubleClick", "Hold", "LongHold" };

String joyButtNames[] = { "" };
//
// void buttonDecoder() 
//   check status of butone and execute functions
//   place a call to this in loop() 
//
// inputs
//   none
//
// outputs
//   none
//

void buttonDecoder()
{
	if (millis() - buttonMillis > 50) {

		buttonRefresh();



		//
		// if some buttons
		//
		if (buttonState != 0) {
			//
			// check which ones
			//

			int butStatus0 = 0;
			
			String butString = "NONE";

			butStatus0 = buttonData[0].status;

			butString = String(buttonData[0].inPin);

			if (butStatus0 == joyButtonClick) {
				Serial.println(butString + ":Click");
			}
			else
				if (butStatus0 == joyButtonDoubleClick) {
					Serial.println(butString + ":DoubleClick");
				}
				else
					if (butStatus0 == joyButtonHold) {
						Serial.println( butString + ":Hold");
					}
					else
						if (butStatus0 == joyButtonLongHold) {
							Serial.println( butString + ":LongHold");
						}


			buttonState = 0;
		}
		buttonMillis = millis();
	}
}