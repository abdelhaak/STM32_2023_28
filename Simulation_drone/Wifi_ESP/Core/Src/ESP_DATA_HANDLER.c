/**
  ******************************************************************************

  File:		ESP DATA HANDLER
  Author:   ControllersTech
  Updated:  3rd Aug 2020

  ******************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ******************************************************************************
*/



#include "ESP_DATA_HANDLER.h"
#include "UartRingbuffer.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart2;

#define wifi_uart &huart2

#define maxnumberofusers  10  // Max number of users

char buffer[20];
userDetails user[maxnumberofusers];

int usernumber = 0;

int sizeofuser (userDetails *user)
{
	int size=0;
	while (user[size].firstname[0] != '\0') size++;
	return size+1;
}

char *home = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 USER DATA COLLECTION</h1>\n\
		<p>Enter the Details in the form below: </p>\n\
		<form action=\"/page1\">\n\
		<label for=\"fname\">First Name:</label><br>\n\
		<input type=\"text\" id=\"fname\" name=\"fname\" value=\"\"><br><br>\n\
		<label for=\"lname\">Last Name:</label><br>\n\
		<input type=\"text\" id=\"lname\" name=\"lname\" value=\"\"><br><br>\n\
		<label for=\"age\">Age:</label><br>\n\
		<input type=\"number\" id=\"age\" name=\"age\" value=\"\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</form><br><br>\n\
		<form action=\"/page2\">\n\
		<input type=\"submit\" value=\"View Data\">\n\
		</form>\n\
		</body></html>";

char *page1 = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 USER DATA COLLECTION</h1>\n\
		<h2> DATA STORED Successfully </h2>\n\
		<p> Click Below to Submit again </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Submit Again\">\n\
		</form><br><br>\n\
		<form action=\"/page2\">\n\
		<input type=\"submit\" value=\"View Data\">\n\
		</form>\n\
		</body></html>";

char *page2_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 USER DATA COLLECTION</h1>\n\
		<h2> DATA CCOLLECTED is Shown BELOW </h2>\n";

char *page2_end = "<p> Click Below to Submit again </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Submit again\">\n\
		</body></html>";

char *table = "<style>table {  font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{  background-color: #dddddd;}</style><table><tr><th>Name</th><th>Age</th></tr>";


/*****************************************************************************************************************************************/

void ESP_Init (char *SSID, char *PASSWD, char *STAIP)
{
	char data[80];

	Ringbuf_init();

	Uart_sendstring("AT+RST\r\n");
	HAL_Delay(2000);

	/********* AT **********/
	Uart_flush();
	Uart_sendstring("AT\r\n");
	while(!(Wait_for("OK\r\n")));


	/********* AT+CWMODE=1 **********/
	Uart_flush();
	Uart_sendstring("AT+CWMODE=1\r\n");
	while (!(Wait_for("OK\r\n")));

	/* Set Static IP Address */
	/********* AT+CWSTAIP=IPADDRESS **********/
	Uart_flush();
	sprintf (data, "AT+CIPSTA=\"%s\"\r\n", STAIP);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));

	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_flush();
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));

	/********* AT+CIPMUX **********/
	Uart_flush();
	Uart_sendstring("AT+CIPMUX=1\r\n");
	while (!(Wait_for("OK\r\n")));

	/********* AT+CIPSERVER **********/
	Uart_flush();
	Uart_sendstring("AT+CIPSERVER=1,80\r\n");
	while (!(Wait_for("OK\r\n")));

}




int Server_Send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	Uart_flush();
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data);
	while (!(Wait_for(">")));
	Uart_sendstring (str);
	while (!(Wait_for("SEND OK")));
	Uart_flush();
	sprintf (data, "AT+CIPCLOSE=%d\r\n",Link_ID);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));
	return 1;
}

void Server_Handle (char *str, int Link_ID)
{
	char datatosend[4096] = {0};
	if (!(strcmp (str, "/page1")))
	{
		sprintf(datatosend, page1);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/page2")))
	{
		char localbuf[2048];
		sprintf(datatosend, page2_Top);
		strcat (datatosend, table);
		int bufsize = (sizeofuser (user));
		for (int i=0; i<bufsize; i++)
		{
			sprintf (localbuf, "<tr><td>%s %s</td>	<td>%s</td></tr>",user[i].firstname,user[i].lastname,user[i].age);
			strcat (datatosend, localbuf);
		}
		strcat (datatosend, "</table>");
		strcat(datatosend, page2_end);
		Server_Send(datatosend, Link_ID);
	}
	else
	{
		sprintf (datatosend, home);
		Server_Send(datatosend, Link_ID);
	}

}

void Server_Start (void)
{
	char buftostoreheader[128] = {0};
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID)));

	Link_ID -= 48;
	while (!(Copy_upto(" HTTP/1.1", buftostoreheader)));
	if (Look_for("/page1", buftostoreheader) == 1)
	{
		GetDataFromBuffer("fname=", "&", buftostoreheader, user[usernumber].firstname);
		GetDataFromBuffer("lname=", "&", buftostoreheader, user[usernumber].lastname);
		GetDataFromBuffer("age=", " HTTP", buftostoreheader, user[usernumber].age);
		usernumber++;
		if (usernumber >9) usernumber = 0;
		Server_Handle("/page1",Link_ID);
	}

	else if (Look_for("/page2", buftostoreheader) == 1)
	{
		Server_Handle("/page2",Link_ID);
	}

	else if (Look_for("/home", buftostoreheader) == 1)
	{
		Server_Handle("/home",Link_ID);
	}

	else if (Look_for("/favicon.ico", buftostoreheader) == 1);

	else
	{
		Server_Handle("/ ", Link_ID);
	}
}



