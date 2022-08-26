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

#define DAIKIN_FUNIKI_OFF		8
#define DAIKIN_FUNIKI_ON		9

#define MITSUBISHI_OFF		10
#define MITSUBISHI_ON			11

#define GENERAL_OFF				12
#define GENERAL_ON				13

#define NAGAKAWA_LC_OFF		14
#define NAGAKAWA_LC_ON		15

#define NAGAKAWA_LM_OFF		16
#define NAGAKAWA_LM_ON		17

#define MITSUBISHI_HEAVY_INDUSTRIES_OFF	  18	
#define MITSUBISHI_HEAVY_INDUSTRIES_ON		19

#define PANASONIC_OFF	  20	
#define PANASONIC_ON		21

#define REETECH_YKR_OFF	  22	
#define REETECH_YKR_ON		23

#define CARRIER_OFF	  24	
#define CARRIER_ON		25

#define GREE_OFF		26
#define GREE_ON			27

#define GALANZ_OFF		28
#define GALANZ_ON			29

#define NAGAKAWA_R51M_OFF		30
#define NAGAKAWA_R51M_ON		31

#define SAMSUNG_OFF		32
#define SAMSUNG_ON		33

#define LG_OFF				34
#define LG_ON					35

#define TCL_OFF			  36
#define TCL_ON				37

/* some individual define */
#define SHARP_CMD_LEN	13
#define FUNIKI_CMD_LEN	12
#define DAIKIN_ARC433A46_CMD_LEN	35
#define DAIKIN_ARC423A5_CMD_LEN		20
#define REETECH_CMD_LEN				6
#define MITSUMISHI_CMD_LEN	18

#define GENERAL_CMD_OFF_LEN	6
#define GENERAL_CMD_ON_LEN	15

#define NAGAKAWA_LC_CMD_LEN	14
#define NAGAKAWA_LM_CMD_LEN	28

#define MITSUBISHI_HEAVY_INDUSTRIES_CMD_OFF_LEN	11
#define MITSUBISHI_HEAVY_INDUSTRIES_CMD_ON_LEN	11

#define PANASONIC_CMD_OFF_LEN	27
#define PANASONIC_CMD_ON_LEN	27

#define REETECH_YKR_OFF_LEN	13
#define REETECH_YKR_ON_LEN	13

#define CARRIER_OFF_LEN	6
#define CARRIER_ON_LEN	6

#define GALANZ_OFF_LEN	14
#define GALANZ_ON_LEN	14

#define GREE_OFF_LEN	8
#define GREE_ON_LEN		8

#define NAGAKAWA_R51M_OFF_LEN	6
#define NAGAKAWA_R51M_ON_LEN	6

#define SAMSUNG_OFF_LEN		21
#define SAMSUNG_ON_LEN		21

#define LG_OFF_LEN	7
#define LG_ON_LEN	  7

#define TCL_CMD_LEN		14
typedef struct{
uint8_t air_id;
uint8_t cmd;
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
