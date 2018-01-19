//****************************************************************************
//
// Program:		Appian registry values header.	 
//				Copyright 1996 Appian/ETMA. All rights reserved.
//
// Description:	This file contains all of the registry values (variable names that are actually written
//				into the registry) for Appian's "Display Driver Registry Key".  This file should be 
//				included into both application and driver code.
//
//***************************************************************************/

//****************************************************************************
//
//                        DEFINES
//
//****************************************************************************

//
// OUR REGISTRY VALUE STRINGS MUST BE UNICODE IN THE NT DRIVER AND ANSI IN THE APPLICATIONS
//
#define NO_TAG( string ) 		L#string 
#define DEF_TAG( string ) 		L"DefaultSettings."L#string 
#ifdef NINE_MONITORS_ONLY
#define MONX_TAG( string ) 		L"MonitorX."L#string 
#else
#define MONX_TAG( string ) 		L#string 
#endif
#define PERF_TAG( string ) 		L"Performance."L#string 

#ifdef NINE_MONITORS_ONLY
//
// "REG_MONITOR_DOT_POS"
//
// This is the position in the following string "MonitorX.##string##"
//	where the monitor number that you want to read from the registry is
//	inserted. This is a fast way to locate the monitor number.
//
// NOTE: Good only for 9 monitors!
//	If more than 9 are desired the code will have to search for 'X' and
//	insert the two digit number.
//
#define REG_MONITOR_DOT_POS 7
#endif

//****************************************************************************************************
//
//				  		APPIAN "DRIVER" REGISTRY ENTRIES
//
//****************************************************************************************************

//****************************************************************************************************
//
//				  		APPIAN "DRIVER" REGISTRY ERRORS
//
//****************************************************************************************************

//There was an error reading a registry item
#define DRV_ERROR_REG_READ				0x01
//The number of Monitors Needed for Orientation is Greater Than
//the number of Monitors found  in the Registry
#define DRV_ERROR_REQUIRED_GT_FOUND		0x02
//The number of Monitors Needed for Orientation is Greater Than
//The number of Monitors that were enabled in the registry
#define DRV_ERROR_REQUIRED_GT_ENABLED	0x04
//The number of Monitors Needed for Orientation is Less Than
//The number of Monitors that were enabled in the registry
#define DRV_ERROR_REQUIRED_LT_ENABLED	0x08
//The number of Monitors Needed for Orientation is Greater Than
//the total number of boards detected in the system
#define DRV_ERROR_REQUIRED_GT_MAX		0x10
//Trouble Mapping Physical Address
#define DRV_ERROR_MAPPING_ADDRESS		0x20
//Trouble with Virtual Desktop Dimensions
#define DRV_ERROR_5462_VIRT_Y_BAD_RES	0x40
#define DRV_ERROR_VIRTUAL_DESKTOP		0x40
//Trouble with Virtual Desktop Square-ness
#define DRV_ERROR_5462_VIRT_Y_UNSQUARE	0x80
