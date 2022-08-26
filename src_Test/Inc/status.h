#ifndef __STATUS_H
#define __STATUS_H

typedef enum{
	DOOR_NORMAL = 0,
	DOOR_OPEN = 1,
	DOOR_NA = 2
}door_status;

typedef enum{
	SS_NORMAL = 0,
	SS_DISCONNECT = 0,
	SS_CONNECT = 1,
	SS_NA = 2,
	SS_ALARM = 1
}sensor_status;

#endif 
