
#include "taskScheduler.h"



taskScheduler::taskScheduler() {
	funcCount = 0;
}



void taskScheduler::addFuncFF(VoidCallBackFunction func, int frequecy, int flags) {

	funcInstance *fi = &this->funcArray[this->funcCount];

	fi->function = func;

	fi->frequency = frequecy;

	fi->flags = flags;

	fi->nextFrequency = frequecy;

	this->funcCount++;

}



void taskScheduler::addFunc(VoidCallBackFunction func) {


	this->addFuncFF(func, 0, 0);	// always run - no special conditions


}



void taskScheduler::addFunc(VoidCallBackFunction func, int frequecy) {

	this->addFuncFF(func, frequecy, 0);

}


void taskScheduler::addFunc(VoidCallBackFunction func, int frequecy, int flags) {

	this->addFuncFF(func, frequecy, flags);

}


void taskScheduler::remFunc(VoidCallBackFunction func) {

	Serial.println("1.funcCount :" + String(this->funcCount));

	for (int f = 0; f < this->funcCount; f++) {

		funcInstance *fi = &this->funcArray[f];

		Serial.println("this idx:" + String(f));

		if (this->funcArray[f].function == func) {

			this->funcArray[f] = this->funcArray[this->funcCount - 1];

			Serial.println("removing idx:" + String(f));

			//				this->function = this->functions[this->funcCount-1];
			//				this->flags[f] = this->flags[this->funcCount - 1];
			//				this->frequency[f] = this->frequency[this->funcCount - 1];
			//				this->nextFrequency[f] = this->nextFrequency[this->funcCount - 1];

			this->funcCount--;

			break;

		}

	}

	Serial.println("2.funcCount :" + String(this->funcCount));
}


void taskScheduler::execute(long funcMillis) {

	//long funcMillis = millis();
	//Serial.println("1.execute! funcCount:" + String(this->funcCount));

	for (int f = 0; f < this->funcCount; f++) {

		funcInstance *fi = &this->funcArray[f];

		if ((fi->frequency == 0 || funcMillis > fi->nextFrequency) && (fi->flags & 0x02) == 0) {

			// 
			// call sheduled func
			//
			fi->function();

			//
			//  reschedule 
			//

			if ((fi->flags & 0x01) != 0) {

				fi->flags |= 0x02;

				this->cleanuprequired = true;

				Serial.println("cleanuprequired!");


			}
			else {

				fi->nextFrequency = fi->frequency == 0 ? 0 : funcMillis + fi->frequency;

			}

		}

	}

	if (this->cleanuprequired) {

		Serial.println("1.cleanup! funcCount:" + String(this->funcCount));

		//
		// remove single run (flags == 0x02) funcs from list
		//

		int remCount = 0;

		for (int f = 0; f < this->funcCount; f++) {

			funcInstance *fi = &this->funcArray[f];

			if ((fi->flags & 0x02) != 0) {

				this->remFunc(fi->function);

				remCount++;

			}

		}

		//Serial.println("2.cleanup! funcCount:" + String(this->funcCount));

//			this->funcCount -= remCount;

		this->cleanuprequired = false;

		Serial.println("2.cleanup! funcCount:" + String(this->funcCount));

	}

	//Serial.println("2.execute! funcCount:" + String(this->funcCount));

}


