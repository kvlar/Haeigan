#include "systemclass.h"
#include "Logger.h"
static Logger* m_logger = Logger::Get_instance();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	m_logger = Logger::Get_instance();
	m_logger->Debug("Application start");
	// Create the system object.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	m_logger->Debug("Aplication stop");
	System->Shutdown();
	delete System;
	System = 0;

	delete m_logger;
	m_logger = 0;

	return 0;
}