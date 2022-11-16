#pragma once

#ifndef ADDON_FUNCTIONENABLE_H
#define ADDON_FUNCTIONENABLE_H

#include <Arduino.h>


#ifndef FUNC_LIMIT 
#define FUNC_LIMIT 5
#endif

//void functionEnable::addFunc(VoidCallBackFunction func);


typedef void(*VoidCallBackFunction) ();



typedef struct funcInstance {
	VoidCallBackFunction function;
	long nextFrequency;
	int frequency;
	int flags;
} _funcInstance;



class functionEnable {

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

	functionEnable();

	void addFunc(VoidCallBackFunction func);

	void addFunc(VoidCallBackFunction func, int frequecy);

	void addFunc(VoidCallBackFunction func, int frequecy, int flags);

	void remFunc(VoidCallBackFunction func);

	void execute(long funcMillis);

};

#endif