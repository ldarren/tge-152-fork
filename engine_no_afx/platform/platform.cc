#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include  "platform/platformMutex.h"

S32 sgBackgroundSleepTime = 3000;
S32 sgTimeManagerProcessInterval = 0;

void Platform::initConsole()
{
   Con::addVariable("pref::backgroundSleepTime", TypeS32, &sgBackgroundSleepTime);
   Con::addVariable("pref::timeManagerProcessInterval", TypeS32, &sgTimeManagerProcessInterval);
}

S32 Platform::getBackgroundSleepTime()
{
   return sgBackgroundSleepTime;
}
