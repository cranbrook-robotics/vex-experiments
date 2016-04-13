#pragma config(UserModel, "C:/Users/rstudent/code/robot-configs/35A-in-and-out.c")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Always open the debug stream window when the program starts
#pragma DebuggerWindows("debugStream")

#define POWER_EXPANDER_A1
#include <CKFlywheelSpeedController.h>


/*_________________________________________________________
 *
 * Robot-specific parameters
 */

const Motor393GearBox FlywheelGearbox = M393HighSpeed;
const int MotorsPerFlywheel = 4;

const tMotor motorPorts[] =
	//{ mFlyR, mFlyL }//32A
	{ mFly1, mFly2, mFly3, mFly4 }//35A
	//{ flyFour, flyOneYThree, flyTwo }//35B
	//{ mFlyB, mFlyM, mFlyT }//Gabewheel
;

// power = A e^( B speed )
// 0.7555e0.1314x
const float A = 0.7555, B = 0.1314;

// Controller coefficients
const float Kq = 0.15, Kd = 0., Ki = 0.01;

//_________________________________________________________



FlywheelSpeedController ctlr;



float speedDialValue(){
	return 16*potentiometer(speedDial); //rad/sec of motor output shaft
}

task control(){
	while(true){
		setTargetSpeed( ctlr, speedDialValue() );
		update( ctlr );
		delay(50);
	}
}

task main(){
	FlywheelSpeedControllerInit( ctlr, Kq, Ki, Kd, A, B, motorPorts, MotorsPerFlywheel, FlywheelGearbox );
	setFlywheelBatteryConfig( ctlr, vPowerExpander, 0.5 );

	startTask(control, kHighPriority);

	while(true){
		writeDebugStream( "%.2f", flywheelBatteryVoltage(ctlr) );
		FlywheelSpeedController& c = ctlr;
		writeDebugStreamLine( "\t | \t %.2f \t %.2f \t %.2f", c.targetSpeed, getAverage( c.maFlywheelSpeed ), c.controlPower );

		delay( 100 );
	}
}
