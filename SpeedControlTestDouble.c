#pragma config(UserModel, "C:/Users/rstudent/code/robot-configs/35.c")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Always open the debug stream window when the program starts
#pragma DebuggerWindows("debugStream")


#include <CKGeneral.h>
#include <CKFlywheelSpeedController.h>


/*_________________________________________________________
 *
 * Robot-specific parameters
 */

const int NFlywheels = 2;
const Motor393GearBox FlywheelGearbox = M393Standard;
const int MotorsPerFlywheel = 2;

const tMotor motorPorts [NFlywheels] [MotorsPerFlywheel] = {
	//{ mFlyLT, mFlyLB, mFlyRT, mFlyRB }//32
	//{ mFlyR, mFlyL }//32A
	//{ mFlyT, mFlyB }//35A
	{ mFlyLB, mFlyLF }, { mFlyRB, mFlyRF }//35
	//{ mFlyB, mFlyM, mFlyT }//Gabewheel
};

// power = A e^( B speed )
//Left: 1.3311e0.1317x
//Right: 0.9754e0.1611x
//const float As[] = {1.3311, 0.9754};
//const float Bs[] = {0.1317, 0.1611};

// Controller coefficients
const float Kq = 0.04, Kd = 0, Ki = 0.02;

//_________________________________________________________






float speedDialValue(){
	return 11*potentiometer(speedDial); //rad/sec of motor output shaft
}


task main(){
	FlywheelSpeedController ctlrL, ctlrR;

	const tMotor motorPortsL[] = {mFlyLB, mFlyLF};
	const tMotor motorPortsR[] = {mFlyRB, mFlyRF};

	FlywheelSpeedControllerInit( ctlrL, Kq, Ki, Kd, 1.3311, 0.1317, motorPortsL, MotorsPerFlywheel, FlywheelGearbox );
	FlywheelSpeedControllerInit( ctlrR, Kq, Ki, Kd, 0.9754, 0.1611, motorPortsR, MotorsPerFlywheel, FlywheelGearbox );


	//MovingAverageInit( maBattery, 10 );


	while(true){
		clearTimer(T1);

		float speed = speedDialValue();
		setTargetSpeed( ctlrL, speed );
		setTargetSpeed( ctlrR, speed );
		update( ctlrL );
		update( ctlrR );

		writeDebugStream( "%.2f", MainBatteryVoltage() );
		writeDebugStream( "  |  %.2f \t %.2f \t %.2f", ctlrL.targetSpeed, getAverage( ctlrL.maFlywheelSpeed ), ctlrL.controlPower );
		writeDebugStream( "  |  %.2f \t %.2f \t %.2f", ctlrR.targetSpeed, getAverage( ctlrR.maFlywheelSpeed ), ctlrR.controlPower );
		writeDebugStreamLine("");

		delay( 100 - time1[T1] );
	}
}