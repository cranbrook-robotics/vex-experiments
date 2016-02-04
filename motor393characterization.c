#pragma config(UserModel, "C:/Users/rstudent/code/vex-edr-team-robot-configs/32.c")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// Always open the debug stream window when the program starts
#pragma DebuggerWindows("debugStream")


#include <CKGeneral.h>
#include <CKVex.h>
#include <CKVexMotors.h>
#include <CKMovingAverage.h>






task main(){

	tMotor motorPorts[] = { mFlyLT, mFlyLB, mFlyRT, mFlyRB };

	IMEMotorSet imems;
	IMEMotorSetInit( imems, motorPorts, 4 );

	MovingAverage maAcceleration;
	MovingAverageInit( maAcceleration, 8 );

	MovingAverage maVelocity;
	MovingAverageInit( maVelocity, 8 );

	const long MinRunTime = 2000;

	for( int rep = 0; rep < 1; ++rep ){
		for( float po = 0.3; po <= 1.001; po += 0.1 ){
			setPower( imems, po );

			long startTime = nPgmTime;
			float avgAccel = 0;
			float vSum = 0;
			int vCount = 0;
			int cruising = 0;

			while( true ){
				long iTime = nPgmTime;
				vSum += MainBatteryVoltage();
				++vCount;
				measure(imems);
				nextSample( maVelocity, imems.ime.velocity );
				nextSample( maAcceleration, imems.ime.acceleration );
				avgAccel = getAverage( maAcceleration );
				if( nPgmTime - startTime > MinRunTime ){
					if( !cruising ){
						if( avgAccel < 0 ){
							cruising = 1;
						}
					} else if( ++cruising > 30 ){
						break; // cruise for 3 seconds after reaching 0ish acceleration.
					}
				}
				long iterationDuration = nPgmTime - iTime;
				delay( 100 - iterationDuration );
			}

			float voltAvg = vSum / vCount;
			float vel = getAverage( maVelocity );
			writeDebugStreamLine("%.2f\t%.2f\t%.3f", voltAvg, po, vel);

			//gradualStop( testMotor );
			setPower( imems, 0 );
			waitUntilRest( imems );
			//writeDebugStreamLine("");
		}
	}
}
