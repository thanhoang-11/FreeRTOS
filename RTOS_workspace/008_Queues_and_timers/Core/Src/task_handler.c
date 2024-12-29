/*
 * task_handler.c
 *
 *  Created on: Dec 8, 2024
 *      Author: Asus
 */
#include "main.h"

static void process_command(command_t *cmd);
static int extract_command(command_t *cmd);

char const *msg_inv = "////Invalid option////\n";

void menu_task(void *param)
{
	uint32_t cmd_addr;

	command_t *cmd;

	uint8_t option;

	char const *msg_menu = "\n========================\n"
							"|         Menu         |\n"
							"========================\n"
								"LED effect    ----> 0\n"
								"Date and time ----> 1\n"
								"Exit          ----> 2\n"
								"Enter your choice here : ";
	while(1)
	{
		xQueueSend(q_print,&msg_menu,portMAX_DELAY);

		//wait for menu commands
		xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
		cmd = (command_t*)cmd_addr;

		if(cmd->len == 1)
		{
			option = cmd->payload[0] - 48;

			switch(option)
			{
				case 0:
					curr_state = sLedEffect;
					xTaskNotify(handle_led_task, 0, eNoAction);
					break;
				case 1:
					curr_state = sRtcMenu;
					xTaskNotify(handle_rtc_task, 0, eNoAction);
					break;
				case 2:
					break;
				default:
					xQueueSend(q_print,&msg_inv,portMAX_DELAY);
					continue;
			}
		}else
		{
			//invalid entry
			xQueueSend(q_print,&msg_inv,portMAX_DELAY);
			continue;
		}

		//wait to run again when some other task notifys
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

	}//while super loop
}

void led_task(void *param)
{
	char const *msg_led = "========================\n"
							  "|      LED Effect     |\n"
							  "========================\n"
							  "(none,e1,e2,e3,e4)\n"
							  "Enter your choice here : ";

	uint32_t cmd_addr;

	command_t *cmd;

	while(1)
	{
		/*wait for notification (notify wait)*/
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		/*print led menu*/
		xQueueSend(q_print, &msg_led, portMAX_DELAY);

		/*wait for LED command (notify wait*/
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

		cmd = (command_t*)cmd_addr;

		if(cmd->len <= 4)
		{
			if(! strcmp((char*)cmd->payload, "none"))
				led_effect_stop();

			else if(! strcmp((char*)cmd->payload, "e1"))
				led_effect(1);

			else if(! strcmp((char*)cmd->payload, "e2"))
				led_effect(2);

			else if(! strcmp((char*)cmd->payload, "e3"))
				led_effect(3);

			else if(! strcmp((char*)cmd->payload, "e4"))
				led_effect(4);

			else
				//invalid entry
				xQueueSend(q_print,&msg_inv,portMAX_DELAY);
		}else
		{
			//invalid entry
			xQueueSend(q_print,&msg_inv,portMAX_DELAY);
		}

		/* update state variable */
		curr_state = sMainMenu;

		/*Notify menu task */
		xTaskNotify(handle_menu_task,0,eNoAction);

	}
}


uint8_t getnumber(uint8_t *payload, uint8_t len)
{
	uint8_t value;

	if(len > 1)
	{
		value = ((payload[0] - 48) * 10 + (payload[1] - 48));
	}else
	{
		value = payload[0] - 48;
	}
	return value;
}

void rtc_task(void *param)
{
	const char* msg_rtc1 = 	"========================\n"
							"|         RTC          |\n"
							"========================\n";

	const char* msg_rtc2 = 	"Configure Time            ----> 0\n"
							"Configure Date            ----> 1\n"
							"Enable reporting          ----> 2\n"
							"Exit                      ----> 3\n"
							"Enter your choice here : ";

	const char *msg_rtc_hh = "Enter hour(1-12):";
	const char *msg_rtc_mm = "Enter minutes(0-59):";
	const char *msg_rtc_ss = "Enter seconds(0-59):";

	const char *msg_rtc_dd  = "Enter date(1-31):";
	const char *msg_rtc_mo  = "Enter month(1-12):";
	const char *msg_rtc_dow = "Enter day(1-7 sun:1):";
	const char *msg_rtc_yr  = "Enter year(0-99):";

	const char *msg_conf = "Configuration successful\n";
	const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";

	uint32_t cmd_addr;
	command_t *cmd;

	uint8_t option;
	uint8_t value;


	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	while(1)
	{
		xTaskNotifyWait(0, 0, 0, portMAX_DELAY);

		xQueueSend(q_print, &msg_rtc1, portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print, &msg_rtc2, portMAX_DELAY);

		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t*)cmd_addr;

		option = getnumber(cmd->payload, cmd->len);
		if(option <= 3)
		{
			switch(option)
			{
				case 0:
					/*************************Hours**************************/
					xQueueSend(q_print, &msg_rtc_hh, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 1 && value <= 12){
						rtc_time.Hours = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					/*************************Minutes**************************/
					xQueueSend(q_print, &msg_rtc_mm, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 0 && value <= 59){
						rtc_time.Minutes = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					/*************************Seconds**************************/
					xQueueSend(q_print, &msg_rtc_ss, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 0 && value <= 59){
						rtc_time.Seconds = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					rtc_configure_time(&rtc_time);
					xQueueSend(q_print, &msg_conf, portMAX_DELAY);
					show_time_date();
					break;

				case 1:
					/*************************Date**************************/
					xQueueSend(q_print, &msg_rtc_dd, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 1 && value <= 31){
						rtc_date.Date = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					/*************************Month**************************/
					xQueueSend(q_print, &msg_rtc_mo, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 1 && value <= 12){
						rtc_date.Month = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					/*************************Day**************************/
					xQueueSend(q_print, &msg_rtc_dow, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 1 && value <= 7){
						rtc_date.WeekDay = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					/*************************Year**************************/
					xQueueSend(q_print, &msg_rtc_yr, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					value = getnumber(cmd->payload, cmd->len);
					//valid value
					if(value >= 0 && value <= 99){
						rtc_date.Year = value;

					}else{  //invalid value
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}

					rtc_configure_date(&rtc_date);
					xQueueSend(q_print, &msg_conf, portMAX_DELAY);
					show_time_date();
					break;

				case 2: //Report over ITM printf
					xQueueSend(q_print, &msg_rtc_report, portMAX_DELAY);
					xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					if(cmd->payload[0] == 'y'){
						if(xTimerIsTimerActive(rtc_timer) == pdFALSE)
							xTimerStart(rtc_timer,portMAX_DELAY);
					}else if(cmd->payload[0] == 'n'){
						xTimerStop(rtc_timer,portMAX_DELAY);
					}else{
						xQueueSend(q_print, &msg_inv, portMAX_DELAY);
					}
					break;

				case 3: //Exit
					break;
			}

		}else
		{
			//invalid entry
			xQueueSend(q_print,&msg_inv,portMAX_DELAY);
			continue;
		}

		xTaskNotify(handle_menu_task, 0, eNoAction);
		curr_state = sMainMenu;

	}
}

void print_task(void *param)
{
	uint32_t *msg;

	while(1)
	{
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);
	}
}

void cmd_handler_task(void *param)
{
	BaseType_t ret;
	command_t cmd;

	while(1)
	{
		/*Implement notify wait */
		ret = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		if(ret == pdTRUE)
		{
			/*process the user data(command) stored in input data queue */
			process_command(&cmd);
		}
	}
}


static void process_command(command_t *cmd)
{
	extract_command(cmd);

	switch (curr_state)
	{
		case sMainMenu:
			xTaskNotify(handle_menu_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;

		case sLedEffect:
			xTaskNotify(handle_led_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;

		case sRtcMenu:

		case sRtcTimeConfig:

		case sRtcDateConfig:

		case sRtcReport:
			xTaskNotify(handle_rtc_task,(uint32_t)cmd , eSetValueWithOverwrite);
			break;
	}
}


int extract_command(command_t *cmd)
{
	uint8_t item;
	BaseType_t  status;

	status = uxQueueMessagesWaiting(q_data );
	if(!status) return -1;
	uint8_t i =0;

	do
	{
		status = xQueueReceive(q_data,&item,0);
		if(status == pdTRUE) cmd->payload[i++] = item;
	}while(item != '\n');

	cmd->payload[i-1] = '\0';
	cmd->len = i-1; /*save  length of the command excluding null char */

	return 0;
}

