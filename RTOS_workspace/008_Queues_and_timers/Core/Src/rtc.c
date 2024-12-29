/*
 * rtc.c
 *
 *  Created on: Dec 7, 2024
 *      Author: Asus
 */
#include "main.h"

void show_time_date_itm(void)
{
	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	memset(&rtc_date,0,sizeof(rtc_date));
	memset(&rtc_time,0,sizeof(rtc_time));

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

	char *format;
	format = (rtc_time.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";

	printf("%02d:%02d:%02d [%s]",rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds,format);
	printf("\t%02d-%02d-%2d\n",rtc_date.Month, rtc_date.Date, 2000 + rtc_date.Year);

}

void show_time_date(void)
{
    static char showtime[40];
    static char showdate[40];

    RTC_DateTypeDef rtc_date;
    RTC_TimeTypeDef rtc_time;

    memset(&rtc_date, 0, sizeof(rtc_date));
    memset(&rtc_time, 0, sizeof(rtc_time));

    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

    char *format = rtc_time.TimeFormat ? "PM" : "AM";

    /* Display time Format : hh:mm:ss [AM/PM] */
    sprintf(showtime, "\nCurrent Time&Date:\t %02d:%02d:%02d [%s]", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, format);

    /* Truyền địa chỉ cấp 2 của mảng */
    char **p_showtime = (char**)&showtime;
    xQueueSend(q_print, &p_showtime, portMAX_DELAY);

    /* Display date Format : date-month-year */
    sprintf(showdate, "\t%02d-%02d-%02d\n", rtc_date.Date, rtc_date.Month, rtc_date.Year + 2000);

    /* Truyền địa chỉ cấp 2 của mảng */
    char **p_showdate = (char**)&showdate;
    xQueueSend(q_print, &p_showdate, portMAX_DELAY);
}



void rtc_configure_time(RTC_TimeTypeDef *time)
{

	time->TimeFormat = RTC_HOURFORMAT12_AM;
	time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	time->StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BIN);
}


void rtc_configure_date(RTC_DateTypeDef *date)
{
	HAL_RTC_SetDate(&hrtc,date,RTC_FORMAT_BIN);
}

