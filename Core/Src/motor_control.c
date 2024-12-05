/*
 * motor_control.c
 *
 *  Created on: Nov 11, 2024
 *      Author: Adrian
 */

#include "motor_control.h"
#include "stm32h7xx_hal.h"
#include "string.h"

extern TIM_HandleTypeDef htim3;

void accelerate_RPM(int target_speed_RPM) {

	//Porneste generarea de semnal PWM
	int target_frequency = (target_speed_RPM / 60.0) * 200;
	int current_frequency  = 1000000 / (__HAL_TIM_GET_AUTORELOAD(&htim3)) +1;

	if(target_frequency > MAX_FREQUENCY){
		target_frequency = MAX_FREQUENCY;
	}
	else if(target_frequency < MIN_FREQUENCY){
		target_frequency = MIN_FREQUENCY;
	}

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);


	while (current_frequency < target_frequency) {

		current_frequency += ACCELERATION_STEP;

		if (current_frequency > target_frequency) {
			current_frequency = target_frequency;
		}

		//Actualizeaza ARR cu noua valoare (1000000 este frecventa ceasului folosita pentru pwm -64mhz cu prescalar de 64)
		__HAL_TIM_SET_AUTORELOAD(&htim3, (1000000 / current_frequency) - 1);
		HAL_Delay(10);
	}
}

void decelerate_to_RPM(int target_speed_RPM) {
	int target_frequency = 0;
	int current_frequency  = 1000000 / (__HAL_TIM_GET_AUTORELOAD(&htim3)) +1;

	if (target_speed_RPM == 0) {
		target_frequency = MIN_FREQUENCY;
	} else {
		target_frequency = (target_speed_RPM / 60.0) * 200;
	}

	while (current_frequency > target_frequency) {

		current_frequency -= DECELERATION_STEP;

		if (current_frequency < target_frequency) {
			current_frequency = target_frequency;
		}

		//Actualizeaza ARR cu noua valoare (1000000 este frecventa ceasului folosita pentru pwm -64mhz cu prescalar de 64)
		__HAL_TIM_SET_AUTORELOAD(&htim3, (1000000 / current_frequency) - 1);
		HAL_Delay(10);
	}

	if (target_speed_RPM == 0 && target_frequency == MIN_FREQUENCY) {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	}
}

void set_direction(Direction direction) {
    if (direction == DIRECTION_LEFT) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    } else if (direction == DIRECTION_RIGHT) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
}


void update_encoder(encoder_instance *encoder_value, TIM_HandleTypeDef *htim) {
    static int32_t last_time = 0;
    int32_t current_time = HAL_GetTick();
    float dt = (current_time - last_time) / 1000.0f;

    last_time = current_time;


    int32_t temp_counter = __HAL_TIM_GET_COUNTER(htim);
    static int32_t first_time = 0;

    if (!first_time) {
        encoder_value->velocity = 0;
        encoder_value->rpm = 0;
        first_time = 1;
    } else {
        if (temp_counter == encoder_value->last_counter_value) {
            encoder_value->velocity = 0;
        } else if (temp_counter > encoder_value->last_counter_value) {
            if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
                encoder_value->velocity = -encoder_value->last_counter_value -
                                         (__HAL_TIM_GET_AUTORELOAD(htim) - temp_counter);
            } else {
                encoder_value->velocity = temp_counter - encoder_value->last_counter_value;
            }
        } else {
            if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
                encoder_value->velocity = temp_counter - encoder_value->last_counter_value;
            } else {
                encoder_value->velocity = temp_counter +
                                         (__HAL_TIM_GET_AUTORELOAD(htim) - encoder_value->last_counter_value);
            }
        }
    }

    encoder_value->position += encoder_value->velocity;
    encoder_value->last_counter_value = temp_counter;

    // Conversie în RPM
    encoder_value->rpm = (encoder_value->velocity / 4000.0f) * (60.0f / dt);
}



void reset_encoder(encoder_instance *encoder_value){
	encoder_value -> velocity =0;
	encoder_value ->position = 0;
	encoder_value ->last_counter_value =0;
	encoder_value->rpm = 0;
}
