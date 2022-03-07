#include <io.h>
#include <stdint.h>

/* Defines */

#define CUR_START_REG	0x0A
#define CUR_LOC_HI_REG	0x0E
#define CUR_LOC_LO_REG	0x0F

#define CRTC_ADDR_REG	0x3D4
#define CRTC_DATA_REG	0x3D5

/* Functions */
uint16_t get_cur_pos();
void disable_cur();
