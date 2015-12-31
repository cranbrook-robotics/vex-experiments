#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    pSpeedPot,      sensorPotentiometer)
#pragma config(Sensor, dgtl12, pActivator,     sensorDigitalIn)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           mpTest,        tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Always open the debug stream window when the program starts
#pragma DebuggerWindows("debugStream")


#include <CKGeneral.h>
#include <CKVex.h>

#define CKAveragerSampleSize 3
#include <CKAverager.h>




const float TicksPerRev = TicksPerRev_393Standard;

const int MaxMotorPower = 127;





struct MotorState {
	int port;
	float power;

	float imeRevs;
	float imeVelocity; // revs per second
	float imeAccel; // revs/sec^2

	// Used for calculating velocity
	unsigned long imeTime0;
};

void updateMotorPower( MotorState& self );

void MotorStateInit( MotorState& self, int port ){
	self.port = port;
	self.power = 0;
	self.imeRevs = 0;
	self.imeVelocity = 0;

	self.imeTime0 = nPgmTime;

	nMotorEncoder[ self.port ] = 0;

	updateMotorPower( self );
}

void updateMotorPower( MotorState& self ){
	motor[ self.port ] = (int)round( self.power * MaxMotorPower );
}


void setPower( MotorState& self, float power ){
	self.power = bound( power, -1, 1 );
	updateMotorPower( self );
}


void measureVelocity( MotorState& self ){
	unsigned long now = nPgmTime;
	self.imeRevs = nMotorEncoder[ self.port ] / TicksPerRev;
	nMotorEncoder[ self.port ] = 0;

	float dt = (float)(now - self.imeTime0) / 1000.0;
	if( dt == 0 ) dt = 1;
	float velocity0 = self.imeVelocity;
	self.imeVelocity = (float)self.imeRevs / dt;
	self.imeAccel = (self.imeVelocity - velocity0) / dt;

	self.imeTime0 = now;
}


float potentiometer( int port ){
	return SensorValue[port] / 1024.0;
}



task main(){

	MotorState testMotor;
	MotorStateInit( testMotor, mpTest );

	Averager accelAverager;
	AveragerInit( accelAverager );

	Averager velocityAverager;
	AveragerInit( velocityAverager );

	//float v = MainBatteryVoltage();

	while( !isPressed(pActivator) )  delay(10);
	while(  isPressed(pActivator) )  delay(10);

	float p = potentiometer(pSpeedPot);
	float target, measured, error, Kp = 0.1, Kd = 0.1, power = 0, lastError, dt, dError;
	long t0 = nPgmTime;

	wait1Msec(100);

	while( !isPressed(pActivator) ){
		p = potentiometer(pSpeedPot);
		target = p;

		measureVelocity(testMotor);
		addSample( velocityAverager, testMotor.imeVelocity );
		addSample( accelAverager, testMotor.imeAccel );

		measured = testMotor.imeVelocity; //average( velocityAverager );
		error = target - measured;
		dt = nPgmTime - t0;
		t0 = nPgmTime;
		dError = (error - lastError) / dt;
		lastError = error;
		power += Kp * error;// - Kd * dError;

		setPower(testMotor, power);

		writeDebugStreamLine("%.3f\t%.3f\t%.3f", target, measured, power);
		wait1Msec(50);
	}

	setPower( testMotor, 0 );
}
