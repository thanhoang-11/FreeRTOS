/*
 * led_effect.c
 *
 *  Created on: Dec 7, 2024
 *      Author: Asus
 */
#include "main.h"

static void turn_on_all_leds(void);
static void turn_off_all_leds(void);

static void turn_on_even_leds(void);
static void turn_on_odd_leds(void);

static void LED_control(uint8_t i);

void led_effect_stop(void)
{
	for(uint8_t i = 0; i < 4; i++)
		xTimerStop(handle_led_timer[i], portMAX_DELAY);
}

void led_effect(int n)
{
	led_effect_stop();

	xTimerStart(handle_led_timer[n - 1], portMAX_DELAY);
}

static void turn_on_all_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}
static void turn_off_all_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}

static void turn_on_even_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}
static void turn_on_odd_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}

static void LED_control(uint8_t value)
{
	for(uint8_t i = 0; i < 4; i++)
		HAL_GPIO_WritePin(LD3_GPIO_Port, LED1 << i, ((value >> i) & 0x1) );
}

void LED_effect1(void)
{
	static uint8_t flag = 1;

	(flag ^= 1) ? turn_on_all_leds() : turn_off_all_leds();
}

void LED_effect2(void)
{
	static uint8_t flag = 1;

	(flag ^= 1) ? turn_on_even_leds() : turn_on_odd_leds();
}

void LED_effect3(void)
{
	static uint8_t i = 0;

	LED_control(0x1 << (i++ % 4));
}

void LED_effect4(void)
{
	static uint8_t i = 0;

	LED_control(0x8 >> (i++ % 4));
}
