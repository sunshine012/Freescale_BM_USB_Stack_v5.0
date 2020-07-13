/**HEADER********************************************************************
* 
* Copyright (c) 2004 - 2009, 2013 Freescale Semiconductor;
* All Rights Reserved
*
*
*************************************************************************** 
*
*
* @file composite_app.h
*
* @author
*
* @version
*
* @date 
*
* @brief The file contains function headers needed by application
*
*****************************************************************************/


#ifndef _COMPOSITE_APP_H
#define _COMPOSITE_APP_H

#define  HIGH_SPEED           (0)
		
#if HIGH_SPEED
#define CONTROLLER_ID         USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID         USB_CONTROLLER_KHCI_0
#endif

extern void TestApp_Init(void);
extern void TestApp_Task(void);
#endif
