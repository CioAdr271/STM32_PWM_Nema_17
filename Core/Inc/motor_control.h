/*
 * motor_control.h
 *
 *  Created on: Nov 11, 2024
 *      Author: Adrian
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#define MIN_FREQUENCY 16
#define MAX_FREQUENCY 1500
#define ACCELERATION_STEP 1
#define DECELERATION_STEP 1

void accelerate_RPM(int target_speed_RPM);
void decelerate_to_RPM(int target_speed_RPM);


#endif /* INC_MOTOR_CONTROL_H_ */
