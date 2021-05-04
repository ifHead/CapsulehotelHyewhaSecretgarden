#ifndef pin_h         
#define pin_h

#include<Arduino.h>
#include<avr/io.h>


#define PROGRAM_VER 11 // 프로그램 버전


#define set_bit(reg,bit) reg |= (1<<bit)
#define clr_bit(reg,bit) reg &= ~(1<<bit)
#define check_bit(reg,bit) (reg&(1<<bit))


//---------------------- 아래는 핀
#define LED_SYSTEM 38
#define LED_SYSTEM_DDR PORTD
#define LED_SYSTEM_PIN 7


#define BUZZER 43
//----------------설정하는부분
#define THREAD_COUNT 3  //쓰레드 몇개 사용할것이냐
#define DEBUG_READ_EN 0 //읽는 디버그 허용 할지
#define TIMEOUT_MS 500
#define IDX_FINISH 150

#define RS485_WRITE_ADDR 0
#define RS485_WRITE_ID 1
#define RS485_WRITE_MODE 0
#define RS485_WRITE_DATA 1

#define RS485_READ_OK 1
#define RS485_READ_TIME_OUT 2


//--------------------
#define RS485_TXPIN 44
#define RS485_TXPIN_DDR PORTL
#define RS485_TXPIN_PIN 5

#define RS485_TX_ON digitalWrite(RS485_TXPIN, HIGH)
#define RS485_TX_OFF digitalWrite(RS485_TXPIN, LOW)


#define RS485_START 252
#define RS485_END 254
#define RS485_READ_START 252
//-------------------
#define DEBUG Serial //기존 serial3
#define ESP32 Serial2

//------------------
#define TRUE 1
#define FALSE 0

#define THREAD_READ_ONEY 0



void pin_init(){
    pinMode(LED_SYSTEM, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(RS485_TXPIN, OUTPUT);
    digitalWrite(RS485_TXPIN, LOW); //기본셋팅값

    DDRC = 0X00;
    DDRA = 0X00;
    
}

void timer_init(){
    //2560 기준으로  0,2 = 8bit // 1, 3, 4, 5= 16bit
    // 타이머1 오버플로우 인터럽트 50msec
    //16000000Hz/64/12500=20Hz,(65536-12500)=53036 
    //TCCR1B=3; TCNT1=53036; 
    
    TCCR1B=5; TCNT1= 3036;  //16000000/256/62500=   1Hz=1sec, (65536-62500)= 3036
    
    TIMSK1=1; //50ms 마다 주기 실행

    //https://cafe.naver.com/circuitsmanual?iframe_url=/ArticleList.nhn%3Fsearch.clubid=18968931%26search.menuid=328%26search.boardtype=L
    

    // 타이머1 A매치 인터럽트
    //TCCR1B=0x0C; OCR1A= 3124; TIMSK1=0X02; //16000000/256/(1+ 3124)=  20Hz=50ms


}


void serial_init(){
    //시리얼 요약
    //DEBUG: UART0 , RS232 = UART1, ESP32= UART2 
    DEBUG.begin(38400);
    ESP32.begin(38400);

    //UCSR0A=0; UCSR0B=0x98; UBRR0H=0; UBRR0L= 25; //DEBUG 용
    //UCSR2A=0; UCSR2B=0x98; UBRR2H=0; UBRR2L= 25; //ESP32 용
    UCSR1A=0; UCSR1B=0x98; UBRR1H=0; UBRR1L= 25; //RS485 는 Serial1 사용(uart1) 38400 통신 속도 사용함
                                                //115200은 오차가 너무 심함
                                                //https://cafe.naver.com/circuitsmanual?iframe_url=/ArticleList.nhn%3Fsearch.clubid=18968931%26search.menuid=328%26search.boardtype=L
    SREG = 0X80; //전체 인터럽트 허용함
    
}






#endif
