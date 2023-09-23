//Advanced Mission System Developed by AIS Studios @2023

#pragma once

//interface should be implemented by tools 
class IExampleModuleListenerInterface
{
public:
	virtual void OnStartupModule() {};
	virtual void OnShutdownModule() {};
};