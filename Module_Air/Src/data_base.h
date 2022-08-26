#ifndef DATA_BASE_H
#define DATA_BASE_H
#include "main.h"

/* define for length cmd */
#define MAX_LEN_CMD	   50 /* max 50 bytes */
/* define type for air */
#define SHARP_AIR_OFF	0
#define SHARP_AIR_ON	1

#define DAIKIN_ARC433A46_OFF	2
#define DAIKIN_ARC433A46_ON		3

#define DAIKIN_ARC423A5_OFF		4
#define DAIKIN_ARC423A5_ON		5

#define REETECH_OFF		6
#define REETECH_ON		7

/* some individual define */
#define SHARP_CMD_LEN	13
#define DAIKIN_ARC433A46_CMD_LEN	35
#define DAIKIN_ARC423A5_CMD_LEN		20
#define REETECH_CMD_LEN				6
typedef struct{
uint8_t cmd_id;
uint8_t cmd_content[MAX_LEN_CMD];
uint8_t len_cmd;
}cmd_ctrl_air_t;

enum mode{
	auto_mode = 0,
	cool_mode,
	dry_mode,
};
/* kho du lieu */
#endif /* DO_H */
