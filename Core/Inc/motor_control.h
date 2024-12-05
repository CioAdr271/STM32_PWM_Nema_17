/*
 * motor_control.h
 *
 *  Created on: Nov 11, 2024
 *      Author: Adrian
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#define MIN_FREQUENCY 100
#define MAX_FREQUENCY 1600
#define ACCELERATION_STEP 5
#define DECELERATION_STEP 5

#include "main.h"

typedef enum {
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef struct{
	int32_t velocity;
	int32_t position;
	int32_t last_counter_value;
	float rpm;
} encoder_instance;

void update_encoder(encoder_instance *encoder_value, TIM_HandleTypeDef *tim);
void reset_encoder(encoder_instance *encoder_value);

void accelerate_RPM(int target_speed_RPM);
void decelerate_to_RPM(int target_speed_RPM);
void set_direction(Direction direction);

#endif /* INC_MOTOR_CONTROL_H_ */
