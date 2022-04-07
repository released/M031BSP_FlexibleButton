/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "project_config.h"
#include "custom_button.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

/*
    [matrix key scanning] 
    L : ROW , INPUT
    PD0 
    PD1 
    PD2 
    PD3

    R : COLUMN , OUTPUT    
    PA0 
    PA1 
    PA2 
    PA3
*/

#define KEY_IO_ROW1         (PD0)
#define KEY_IO_ROW2         (PD1)
#define KEY_IO_ROW3         (PD2)
#define KEY_IO_ROW4         (PD3)

#define KEY_SCAN_ROW1       (0)   // INPUT
#define KEY_SCAN_ROW2       (1)   // INPUT
#define KEY_SCAN_ROW3       (2)   // INPUT
#define KEY_SCAN_ROW4       (3)   // INPUT

#define KEY_IO_COL1         (PA0)
#define KEY_IO_COL2         (PA1)
#define KEY_IO_COL3         (PA2)
#define KEY_IO_COL4         (PA3)

#define KEY_SCAN_COL1       (0)   // OUTPUT
#define KEY_SCAN_COL2       (1)   // OUTPUT
#define KEY_SCAN_COL3       (2)   // OUTPUT
#define KEY_SCAN_COL4       (3)   // OUTPUT

#define KEY_ROW_NUM         (4)
#define KEY_COL_NUM         (4)
#define DEBOUNCE_TIMEOUT    (5)

#define KEY_DELAY(ms)       (TIMER_Delay(TIMER0, 1000*ms))

#if 1
const uint16_t u16_upkey_table1[4][4] = 
{
    //if upkey not zero   
    /*
    upkey    0       10      100      1000
    downkey  1110    1       10       100
    */
    {0x0000 ,0x0010 ,0x0100  ,0x1000},  

    //if upkey not zero   
    /*
    upkey    0       20      200      2000
    downkey  2220    2       20       200
    */
    {0x0000 ,0x0020 ,0x0200  ,0x2000},

    //if upkey not zero   
    /*
    upkey    0       40      400      4000
    downkey  4440    4       40       400
    */
    {0x0000 ,0x0040 ,0x0400  ,0x4000}, 

    //if upkey not zero   
    /*
    upkey    0       80      800      8000
    downkey  8880    8       80       800
    */   

    {0x0000 ,0x0080 ,0x0800  ,0x8000},
};

const uint16_t u16_downkey_upkeyNonZero_table1[4][4] = 
{
    //if upkey not zero   
    /*
    upkey    0       10      100      1000
    downkey  1110    1       10       100
    */
    {0x1110 ,0x0001 ,0x0010  ,0x0100},  

    //if upkey not zero   
    /*
    upkey    0       20      200      2000
    downkey  2220    2       20       200
    */
    {0x2220 ,0x0002 ,0x0020  ,0x0200},

    //if upkey not zero   
    /*
    upkey    0       40      400      4000
    downkey  4440    4       40       400
    */
    {0x4440 ,0x0004 ,0x0040  ,0x0400}, 

    //if upkey not zero   
    /*
    upkey    0       80      800      8000
    downkey  8880    8       80       800
    */
    {0x8880 ,0x0008 ,0x0080  ,0x0800},
};

const uint16_t u16_downkey_upkeyZero_table1[4][4] = 
{
    //if upkey is 0
    {0x1110 ,0x1101 ,0x1011  ,0x0111},  

    //if upkey is 0
    {0x2220 ,0x2202 ,0x2022  ,0x0222},
 
    //if upkey is 0
    {0x4440 ,0x4404 ,0x4044  ,0x0444}, 
  
    //if upkey is 0
    {0x8880 ,0x8808 ,0x8088  ,0x0888},
};

const uint16_t u16_keycode_table1[4][4] = 
{
    {0xEEEF ,0xEEFE ,0xEFEE  ,0xFEEE},
    {0xDDDF ,0xDDFD ,0xDFDD  ,0xFDDD},
    {0xBBBF ,0xBBFB ,0xBFBB  ,0xFBBB},
    {0x777F ,0x77F7 ,0x7F77  ,0xF777},
};
#else

const uint16_t u16_downkey_table2[4][4] = 
{
    {0x0101 ,0x1110 ,0x1011  ,0x1111},
    {0x0202 ,0x2220 ,0x2022  ,0x2222},
    {0x0404 ,0x4440 ,0x4044  ,0x4444},
    {0x0808 ,0x8880 ,0x8088  ,0x8888},
};

const uint16_t u16_key_table2[4][4] = 
{
    {0xFEFE ,0xEEEF ,0xEFEE  ,0xEEEE},
    {0xFDFD ,0xDDDF ,0xDFDD  ,0xDDDD},
    {0xFBFB ,0xBBBF ,0xBFBB  ,0xBBBB},
    {0xF7F7 ,0x777F ,0x7F77  ,0x7777},
};
#endif

const char* key_name[4][4] = 
{
    {"K1"   ,"K2"   ,"K3"   ,"K4"},
    {"K5"   ,"K6"   ,"K7"   ,"K8"},
    {"K9"   ,"K10"  ,"K11"  ,"K12"},
    {"K13"  ,"K14"  ,"K15"  ,"K16"},
};

/*_____ D E F I N I T I O N S ______________________________________________*/

#define ENUM_TO_STR(e) (#e)
typedef enum
{
    USER_BUTTON_1 = 0,
    USER_BUTTON_2,
    USER_BUTTON_3,
    USER_BUTTON_4,
    USER_BUTTON_MAX
} user_button_t;

char *enum_event_string[] = {
    ENUM_TO_STR(FLEX_BTN_PRESS_DOWN),
    ENUM_TO_STR(FLEX_BTN_PRESS_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_DOUBLE_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_REPEAT_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_MAX),
    ENUM_TO_STR(FLEX_BTN_PRESS_NONE),
};

char *enum_btn_id_string[] = {
    ENUM_TO_STR(USER_BUTTON_1),
    ENUM_TO_STR(USER_BUTTON_2),
    ENUM_TO_STR(USER_BUTTON_3),
    ENUM_TO_STR(USER_BUTTON_4),
    ENUM_TO_STR(USER_BUTTON_MAX),
};

flex_button_t user_button[USER_BUTTON_MAX];

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern uint32_t get_tick(void);


uint8_t KEYPAD_GetRow(uint8_t pin_row)
{
    uint8_t res = 0;
    switch(pin_row)
    {
        case KEY_SCAN_ROW1:
            res = KEY_IO_ROW1;
            break;
        case KEY_SCAN_ROW2:
            res = KEY_IO_ROW2;
            break;
        case KEY_SCAN_ROW3:
            res = KEY_IO_ROW3;
            break;
        case KEY_SCAN_ROW4:
            res = KEY_IO_ROW4;
            break;
    }
    
    return res ;
}

void KEYPAD_SetColumn(uint8_t pin_column , uint8_t pin_set)
{
    switch(pin_column)
    {
        case KEY_SCAN_COL1:
            KEY_IO_COL1 = pin_set;
            break;
        case KEY_SCAN_COL2:
            KEY_IO_COL2 = pin_set;
            break;
        case KEY_SCAN_COL3:
            KEY_IO_COL3 = pin_set;
            break;
        case KEY_SCAN_COL4:
            KEY_IO_COL4 = pin_set;
            break;
    }
}


uint8_t KEYPAD_PRESSED_DISCHARGE(void)
{
    uint8_t x = 0;   
    uint8_t i = 0;  

    for(x = 0; x < KEY_COL_NUM; x++)
    {
        // Check key pressed
        if (KEYPAD_GetRow(x))
        {
            for(i = 0; i < 5; i++)
            {
                KEYPAD_SetColumn(KEY_SCAN_COL1 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL2 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , ENABLE); 
                KEY_DELAY(1);

                KEYPAD_SetColumn(KEY_SCAN_COL1 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL2 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , DISABLE); 
                KEY_DELAY(1);                
            }
            return 1;
        }
    }
    return 0;
}

uint16_t DebounceState(uint16_t currentState , uint16_t* upKeys, uint16_t* downKeys)
{
    static uint16_t lastState = 0;
    static uint16_t lastValidState = 0;
    static uint32_t lastTimestamp = 0;

    // Set defaults
    if (upKeys)
        *upKeys = 0;
    if (downKeys)
        *downKeys = 0;

    // Now lets do debouncing - this is important for good functionality
    // Matrix state should not change for some time to accept result
    if (lastState == currentState)
    {
        // Check if time has passed
        if ((get_tick() - lastTimestamp) >= DEBOUNCE_TIMEOUT)
        {
            // Let's detect up/down changes first - it's easy, just use XOR for change detection
            if (upKeys)
                *upKeys = currentState & (lastValidState ^ currentState);
            if (downKeys)
                *downKeys = (~currentState) & (lastValidState ^ currentState);
            lastValidState = currentState;
        }
    }
    else
    {
        // Current state differs from previous - reset timer
        lastTimestamp = get_tick();
    }
    lastState = currentState;

    return lastValidState;

}

uint16_t KeyboardState(uint16_t* upKeys, uint16_t* downKeys)
{
    uint16_t currentState = 0;

    #if 1   // key scan 1
    uint8_t x, y;

    for(y = 0; y < KEY_ROW_NUM; y++)
    {
        // Set appropriate row pin to H
        KEYPAD_SetColumn(y , ENABLE);
        // Just to be sure that any parasitic capacitance gets charged
        KEY_DELAY(10);
        for(x = 0; x < KEY_COL_NUM; x++)
        {
            // Check key pressed
            if (KEYPAD_GetRow(x))
                currentState |= 1 << ((y * KEY_COL_NUM) + x);
        }
        // And don't forget to set row pin back to L
        KEYPAD_SetColumn(y , DISABLE);
    }
    #else   // key scan 2
    /*      
                    COL1        COL2        COL3        COL4
        ROW1      0xFEFE      0xEEEF      0xEFEE      0xEEEE
        ROW2      0xFDFD      0xDDDF      0xDFDD      0xDDDD
        ROW3      0xFBFB      0xBBBF      0xBFBB      0xBBBB
        ROW4      0xF7F7      0x777F      0x7F77      0x7777
    */

    uint8_t key_x_num = 1;
    uint8_t key_press_num = 0;

    uint16_t CC = 0;

    while(1)   
    {
        switch(key_x_num)
        {
            case 1:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL2 , ENABLE);
                break;
            case 2:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL2 , DISABLE);
                break;

            case 3:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , ENABLE);
                break;
            case 4:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , DISABLE);
                break;

            case 5:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , ENABLE);
                break;
            case 6:
                KEYPAD_SetColumn(KEY_SCAN_COL1 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , DISABLE);
                break;

            case 7:
                KEYPAD_SetColumn(KEY_SCAN_COL2 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , ENABLE);
                break;
            case 8:
                KEYPAD_SetColumn(KEY_SCAN_COL2 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL3 , DISABLE);
                break;

            case 9:
                KEYPAD_SetColumn(KEY_SCAN_COL2 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , ENABLE);
                break;
            case 10:
                KEYPAD_SetColumn(KEY_SCAN_COL2 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , DISABLE);
                break;

            case 11:
                KEYPAD_SetColumn(KEY_SCAN_COL3 , DISABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , ENABLE);
                break;
            case 12:
                KEYPAD_SetColumn(KEY_SCAN_COL3 , ENABLE);
                KEYPAD_SetColumn(KEY_SCAN_COL4 , DISABLE);
                break;

            default:
                break;
        }

        KEY_DELAY(10);

        if (KEYPAD_GetRow(KEY_SCAN_ROW1) )
        {
            CC |= 0x01 << key_press_num;
        }
        key_press_num++;
        if (KEYPAD_GetRow(KEY_SCAN_ROW2) )
        {
            CC |= 0x01 << key_press_num;
        }
        key_press_num++;
        if (KEYPAD_GetRow(KEY_SCAN_ROW3) )
        {
            CC |= 0x01 << key_press_num;
        }
        key_press_num++;
        if (KEYPAD_GetRow(KEY_SCAN_ROW4) )
        {
            CC |= 0x01 << key_press_num;
        }
        key_x_num++;
        key_press_num++;

        if (key_x_num >= 13)
        {
            KEYPAD_SetColumn(KEY_SCAN_COL1 , DISABLE);
            KEYPAD_SetColumn(KEY_SCAN_COL2 , DISABLE);
            KEYPAD_SetColumn(KEY_SCAN_COL3 , DISABLE);
            KEYPAD_SetColumn(KEY_SCAN_COL4 , DISABLE);                        
            break;  //return CC;
        }
    }    

    currentState = CC;

    #endif

    return DebounceState(currentState,upKeys,downKeys);


}

void matrix_btn_read(void)
{
    uint16_t upKeys = 0;
    uint16_t downKeys = 0;
    uint16_t currentKeys = 0;
    uint16_t keycode_shift = 0xFFFF;    
    uint16_t downKeys_shift = 0xFFFF;     
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t row = 0;
    uint8_t col = 0;
    uint16_t u16_downkey_upkeyZero = 0;
    uint16_t u16_downkey_upkeyNonZero = 0;
    uint16_t u16_upkey = 0;

    currentKeys = KeyboardState(&upKeys, &downKeys);
    if (currentKeys != 0xFFFF)
    {
        for (x = 0; x < 4; x++)
        {
            for (y = 0; y < 4; y++)
            {
                if (currentKeys == u16_keycode_table1[x][y])
                {
                    row = x;
                    col = y;

                    u16_downkey_upkeyZero = u16_downkey_upkeyZero_table1[row][col];
                    u16_downkey_upkeyNonZero = u16_downkey_upkeyNonZero_table1[row][col];
                    u16_upkey = u16_upkey_table1[row][col];
                                        
                    if (!( (upKeys == 0) && (downKeys == 0)))
                    {
                        set_flag(flag_btn_keymapping ,ENABLE);

                        if ( (u16_downkey_upkeyNonZero == downKeys) && 
                                (u16_upkey == upKeys) )
                        {
                            set_flag(flag_btn_keymapp_partial , ENABLE);
                        }
                        else
                        {
                            if (col == 3)
                            {
                                keycode_shift = u16_downkey_upkeyZero;
                                downKeys_shift = downKeys;
                            }
                            else
                            {
                                keycode_shift = u16_downkey_upkeyZero;
                                keycode_shift >>= row*4;
                                keycode_shift >>= col;

                                downKeys_shift = downKeys;
                                downKeys_shift >>= row*4;
                                downKeys_shift >>= col;
                            }
                        }

                        #if 0 // debug , to get currentKeys , upKeys , downKeys hardcode table
                        printf("0)currentKeys:0x%04X,row:0x%02X,col:0x%02X,upKeys:0x%04X,downKeys:0x%04X\r\n" , currentKeys , row , col, upKeys , downKeys);
                        #endif
                    } 
                }
            }
        }
    } 

    if (is_flag_set(flag_btn_keymapping) && 
        KEYPAD_PRESSED_DISCHARGE())
    {
        set_flag(flag_btn_keymapping ,DISABLE);     
        
        if ( (downKeys_shift == keycode_shift) && 
                !is_flag_set(flag_btn_keymapp_partial) )
        {                        
            printf("2)keycode_shift:(0x%04X) ,downKeys_shift:(0x%04X)\r\n" , keycode_shift , downKeys_shift);
            printf("2)BTN01 : %2s (0x%04X) \r\n\r\n" , key_name[row][col] , currentKeys);
        } 

        if (is_flag_set(flag_btn_keymapp_partial))
        {
            set_flag(flag_btn_keymapp_partial ,DISABLE);
            printf("3)downKeys:(0x%04X) ,upKeys:(0x%04X)\r\n" , u16_downkey_upkeyNonZero , u16_upkey);
            printf("3)BTN02 : %2s (0x%04X) \r\n\r\n" , key_name[row][col] , currentKeys);           
        }
    }  
   
}

uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;

    flex_button_t *btn = (flex_button_t *)arg;

    switch (btn->id)
    {
        case USER_BUTTON_1:
            value = PC0;

            break;
        case USER_BUTTON_2:
            value = PC1;

            break;
        case USER_BUTTON_3:
            value = PC2;

            break;
        case USER_BUTTON_4:
            value = PC3;

            break;
        default:
            break;
    }

    return value;
}

void common_btn_evt_cb(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;

    printf("id: [%d - %s]  event: [%d - %30s]  repeat: %d\r\n", 
        btn->id, enum_btn_id_string[btn->id],
        btn->event, enum_event_string[btn->event],
        btn->click_cnt);

    if ((flex_button_event_read(&user_button[USER_BUTTON_1]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_2]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 1 and button 2\r\n");
    }

    if ((flex_button_event_read(&user_button[USER_BUTTON_1]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_3]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 1 and button 3\r\n");
    }

    if ((flex_button_event_read(&user_button[USER_BUTTON_1]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_4]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 1 and button 4\r\n");
    }

    if ((flex_button_event_read(&user_button[USER_BUTTON_2]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_3]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 2 and button 3\r\n");
    }

    if ((flex_button_event_read(&user_button[USER_BUTTON_2]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_4]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 2 and button 4\r\n");
    }

    if ((flex_button_event_read(&user_button[USER_BUTTON_3]) == FLEX_BTN_PRESS_CLICK) &&\
        (flex_button_event_read(&user_button[USER_BUTTON_4]) == FLEX_BTN_PRESS_CLICK))
    {
        printf("[combination]: button 3 and button 4\r\n");
    }
}

void user_key_init(void)
{
    #if defined (ENABLE_MATRIX_KEYBOARD)

    // ROW
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD0MFP_Msk)) | (SYS_GPD_MFPL_PD0MFP_GPIO);
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD1MFP_Msk)) | (SYS_GPD_MFPL_PD1MFP_GPIO);
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD2MFP_Msk)) | (SYS_GPD_MFPL_PD2MFP_GPIO);
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD3MFP_Msk)) | (SYS_GPD_MFPL_PD3MFP_GPIO);

    GPIO_SetMode(PD, BIT0, GPIO_MODE_INPUT);
    GPIO_SetMode(PD, BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PD, BIT2, GPIO_MODE_INPUT);
    GPIO_SetMode(PD, BIT3, GPIO_MODE_INPUT);

    // COLUMN
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk)) | (SYS_GPA_MFPL_PA0MFP_GPIO);
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA1MFP_Msk)) | (SYS_GPA_MFPL_PA1MFP_GPIO);
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA2MFP_Msk)) | (SYS_GPA_MFPL_PA2MFP_GPIO);
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk)) | (SYS_GPA_MFPL_PA3MFP_GPIO);

    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);

    #else

    int i;

    memset(&user_button[0], 0x0, sizeof(user_button));

    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk)) | (SYS_GPC_MFPL_PC0MFP_GPIO);
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO);
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC2MFP_Msk)) | (SYS_GPC_MFPL_PC2MFP_GPIO);
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk)) | (SYS_GPC_MFPL_PC3MFP_GPIO);

    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
    GPIO_SetMode(PC, BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PC, BIT2, GPIO_MODE_INPUT);
    GPIO_SetMode(PC, BIT3, GPIO_MODE_INPUT);

    for (i = 0; i < USER_BUTTON_MAX; i ++)
    {
        user_button[i].id = i;
        user_button[i].usr_button_read = common_btn_read;
        user_button[i].cb = common_btn_evt_cb;
        user_button[i].pressed_logic_level = 0;
        user_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
        user_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
        user_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);

        flex_button_register(&user_button[i]);
    }

    #endif   
}

