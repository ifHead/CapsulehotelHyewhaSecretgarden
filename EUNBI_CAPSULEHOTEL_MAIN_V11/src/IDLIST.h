#ifndef IDLIST_h         
#define IDLIST_h


//-----------------------
#define ISC_MAIN 0
#define ISC_INPUT 1
#define ISC_RELAY 2
#define ISC_AUDIO 3
#define ISC_VIDEO 4
#define ISC_MOTOR 5
#define ISC_TOF 6
#define ISC_KEYPAD 7
#define ISC_SLEEP 8

//---------------INPUT 관련한 명렁어
#define I_INPUT_RAW 5
#define I_KEYPAD_PASSWORD_1 1
#define I_KEYPAD_PASSWORD_2 2

#define I_TOF_REQ 1
#define I_SLEEP_REQ 2


#define IN_HIGH 1
#define IN_LOW 0

#define GET_PORT_1 0
#define GET_PORT_2 1
#define GET_PORT_3 2
#define GET_PORT_4 3
#define GET_PORT_5 4
#define GET_PORT_6 5
#define GET_PORT_7 6
#define GET_PORT_8 7
#define GET_PORT_9 8
#define GET_PORT_10 9
#define GET_PORT_11 10
#define GET_PORT_12 11





//-------------- OUTPUT 에 관련한거
#define W_RELAY_RAW 1
#define W_PWM_CH_1 2
#define W_PWM_CH_2 3
#define W_PWM_CH_3 4
#define W_PWM_CH_ALL 5
#define W_PWM_WARRNG 6

#define W_SOUND_BGM 1
#define W_SOUND_SET_CH 3
#define W_VIDEO_PLAY 1
#define W_MOTOR_RAW 1
#define W_SLEEP_MODE_SET 5  


#define W_PASSWORD_1_REQ 1
#define W_PASSWORD_2_REQ 2


#define MODE_DIMMIG 0
#define MODE_NORMAL 1

#define D_SLEEP_SET_OFF 3
#define D_SLEEP_SET_MIDDLE 1
#define D_SLEEP_SET_FULL 2
#define D_SLEEP_SET_MID_RESET 4




#define D_RELAY_SET_1 4
#define D_RELAY_SET_2 5
#define D_RELAY_SET_3 6
#define D_RELAY_SET_4 7
#define D_RELAY_SET_5 3
#define D_RELAY_SET_6 2
#define D_RELAY_SET_7 1
#define D_RELAY_SET_8 0


#define D_RELAY_PWM_SET_1 0
#define D_RELAY_PWM_SET_2 1
#define D_RELAY_PWM_SET_3 2

#define COM_VIDEO_LOOP 1
#define COM_VIDEO_NOLOOP 2


#define S_RELAY_ON 1
#define S_RELAY_OFF 0

#define D_MUSIC_BGM 0
#define D_MUSIC_EVENT 60

#define D_MOTOR_SET_1 0
#define D_MOTOR_SET_2 1
#define D_MOTOR_SET_3 2
#define D_MOTOR_SET_4 3

#define S_MOTOR_CLOSE 1
#define S_MOTOR_OPEN 0
#define S_MOTOR_OFF 3




#define D_UDP_PLAYER_DSP 1
#define D_UDP_PLAYER_AVP 2
#define D_UDP_PLAYER_UTP_AP 3

//---------------------- 아래는 통합
#define D_VERSION 249
#define MODE_RS485_WRITE 1
#define MODE_RS485_READ 2
#define MODE_INTERNAL 3

#endif