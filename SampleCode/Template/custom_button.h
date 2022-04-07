/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "flexible_button.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

// #define ENABLE_MATRIX_KEYBOARD

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void matrix_btn_read(void);

uint8_t common_btn_read(void *arg);

void common_btn_evt_cb(void *arg);

void user_key_init(void);

