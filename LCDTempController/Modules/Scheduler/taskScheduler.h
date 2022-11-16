#pragma once

#ifndef ADDON_TASKSCHEDULER_H
#define ADDON_TASKSCHEDULER_H

#include <Arduino.h>


#ifndef FUNC_LIMIT 
#define FUNC_LIMIT 10
#endif

//void taskScheduler::addFunc(VoidCallBackFunction func);


typedef void(*VoidCallBackFunction) ();



typedef struct funcInstance {
	VoidCallBackFunction function;
	long nextFrequency;
	int frequency;
	int flags;
} _funcInstance;



class taskScheduler {

private:
	funcInstance funcArray[FUNC_LIMIT];

	//VoidCallBackFunction functions[FUNC_LIMIT];
	//long nextFrequency[FUNC_LIMIT];
	//int frequency[FUNC_LIMIT];
	//
	//int flags[FUNC_LIMIT];

	int funcCount;
	bool cleanuprequired = false;

	void addFuncFF(VoidCallBackFunction func, int frequecy, int flags);

public:

	taskScheduler();

	void addFunc(VoidCallBackFunction func);

	void addFunc(VoidCallBackFunction func, int frequecy);

	void addFunc(VoidCallBackFunction func, int frequecy, int flags);

	void remFunc(VoidCallBackFunction func);

	void execute(long funcMillis);
};

#endif