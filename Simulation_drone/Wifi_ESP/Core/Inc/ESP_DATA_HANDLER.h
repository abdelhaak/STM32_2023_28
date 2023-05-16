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

#ifndef INC_ESP_DATA_HANDLER_H_
#define INC_ESP_DATA_HANDLER_H_


void ESP_Init (char *SSID, char *PASSWD, char *STAIP);
void Server_Start (void);

typedef struct
{
	char firstname[15];
	char lastname[15];
	char age[3];
}userDetails;


#endif /* INC_ESP_DATA_HANDLER_H_ */
