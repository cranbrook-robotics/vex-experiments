#ifndef __LCDBattery__
#define __LCDBattery__

#include <CKVex.h>

void LCDInitialize()
{
	bLCDBacklight = true;
	clearLCDLine(0);
	clearLCDLine(1);
}

task ManageLCD()
{
	string lcdBatteryVoltages;
	while(true)
	{
		sprintf(lcdBatteryVoltages, "M: %.2f P: %.2f", MainBatteryVoltage(), SensorValue(pPowerExp)/283.2);
		clearLCDLine(0);
		clearLCDLine(1);
		displayLCDString(0,0,lcdBatteryVoltages);
		delay(300);
	}
}



#endif
