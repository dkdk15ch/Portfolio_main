#include "Global.h"
#include "FileLog.h"

void Globalinit()
{
	FileLog::initPointerLog();

}

void GlobalTerm()
{
	FileLog::analyzePointerLog();
}
