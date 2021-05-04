#ifndef CommThread_h         
#define CommThread_h

#include <Arduino.h>
#include "pin.h"
#include "IDLIST.h"
/*
    분류: 크게 2가지
        recive 수신 받을때 까지 무한 딜레이
        com: led 명령 보내는거
        //-----------------------
        데이터를 보내는 com은 보내기만함(수신은 딱히 안함)
*/


#define MODE_SETUP 0 //설정 모드
#define MODE_RUNNING 1 //실행 모드

#define MAX_DEVICE_SCAN 20
#define FUNCTION_SCAN 253

#define RS485_BUFFER_raw 1
#define RS485_BUFFER_RUNNING 2

#define INDEX_RX_WHO 3
#define INDEX_RX_ID 2




//commThread_running: 실제적으로 보내고 하는 친구
class CommThread_Running{ //이친구가 진짜로 운영하는 부분
    private:
        boolean debug_en = false;
        unsigned char buffer[8];
        char str[50];
        //----------------------
        struct DEVICE_S{
            unsigned char id[100];
            unsigned char who[100];
            unsigned char instr[100];
            unsigned char parm1[100];
            int max;
        };

        struct DEVICE_RX_S{
            unsigned char parm1[100];
            unsigned char parm2[100];
        };

        struct RX_S{
            unsigned char buffer;
            unsigned char before_buffer;
            unsigned char mode;
            unsigned char cnt;
            unsigned char data[200];
            unsigned char checksum;

            boolean enable;
            unsigned char parm1;
            unsigned char parm2;

        };

        struct manager_s{
            boolean writer;
            int w_return;
            int cnt;
        };

        struct DEVICE_RX_S dev_rx;
        struct RX_S rx;
        struct DEVICE_S dev;
        struct manager_s manager;
        
        

    public:
    CommThread_Running(boolean _DEBUG_EN){ //생성자
        debug_en = _DEBUG_EN;
        manager.writer = true; //보낼 준비

    }

    unsigned char get_parm1( int _idx ){
        return dev_rx.parm1[_idx];
    }

    unsigned char get_parm2( int _idx ){
        return dev_rx.parm2[_idx];
    }


    void add_raw_schedule(int _index , char _id, char _who , char _instr, char _parm1 ){ //스케줄 관리 하는거
        dev.id[_index] = _id;
        dev.who[_index] = _who;
        dev.instr[_index] = _instr;
        dev.parm1[_index] = _parm1;

        if(dev.max <= _index){
              dev.max = _index;
        }
    }
    
    void command_raw_loop(){ //특정 주기가 왔을때 데이터를 전송한다
        if(manager.writer == true){ //처음 부팅시만 실행할거 같음 아마도
             rs485_buffer( dev.id[manager.cnt], dev.who[manager.cnt], dev.instr[manager.cnt], dev.parm1[manager.cnt] );
             rs485_send();

            //sprintf(str, "$%d SSEND:A%d I%d D%d P%d ", manager.cnt,  dev.id[manager.cnt], dev.who[manager.cnt], dev.instr[manager.cnt], dev.parm1[manager.cnt] );
            //ESP32.println(str);
            //DEBUG.println(str);

             manager.writer = false; //다 보냄

        }else{
            if(rx.enable == true){ //받았을때
                
                dev_rx.parm1[manager.cnt] = rx.parm1; //값 저장함
                dev_rx.parm2[manager.cnt] = rx.parm2;
                
               // sprintf(str, "$%d RECV:P%d P%d ", manager.cnt, dev_rx.parm1[manager.cnt]  , dev_rx.parm2[manager.cnt]  );
               // ESP32.println(str);
               // DEBUG.println(str);
                
            }else{

                 sprintf(str, "$%d FAIL:A%d D%d ",manager.cnt,  dev.id[manager.cnt], dev.who[manager.cnt]  );
                 ESP32.println(str);
                 DEBUG.println(str);   

            }
            //====================================================
            if( ++manager.cnt > dev.max){ //증가하고
                manager.cnt = 0;
            }
            rs485_buffer( dev.id[manager.cnt], dev.who[manager.cnt], dev.instr[manager.cnt], dev.parm1[manager.cnt] );
            rs485_send();
            
            //sprintf(str, "$%d SEND:A%d I%d D%d P%d ", manager.cnt,  dev.id[manager.cnt], dev.who[manager.cnt], dev.instr[manager.cnt], dev.parm1[manager.cnt] );
            //ESP32.println(str);
            //DEBUG.println(str);


        }

        //-----------------------------
        rx.enable = false;

    }

    void rs485_buffer(unsigned char id , unsigned char who, unsigned char instr, unsigned char parm1  ){
        //ISC 2세대 프로트콜(로보티즈 기반)
        //0XFF 0XFF ID Who Instruction parm1 checksum
        buffer[0] = 0xFF; buffer[1] = 0xFF;
        buffer[2] = id; buffer[3] = who;
        buffer[4] = instr; buffer[5] = parm1;
        buffer[6] = ~(id + who + instr + parm1); //checm sum
        buffer[7] = 0; //null값
    }

    void rs485_send(){
        //보내는데 9 ~ 10ms 소요
        //rs485는 tx pin이 HIGH = 보내기 / LOW = 받기 모드임
        set_bit(RS485_TXPIN_DDR, RS485_TXPIN_PIN);
        //_delay_us(500);
        _delay_ms(1);

        //TX1_STR(buffer);
        for(int i=0; i<7; i++){
            TX1_CH( buffer[i] );
            _delay_ms(1);
        }
        
        _delay_ms(1);
        clr_bit(RS485_TXPIN_DDR, RS485_TXPIN_PIN);
    }

    void recive( unsigned char _rx ){
        rx.buffer = _rx;
        //sprintf(str, "%d ", rx.buffer); DEBUG.println(str);

        if(rx.buffer == 0xFF && rx.before_buffer == 0XFF){
            rx.mode = RS485_BUFFER_raw;
            rx.cnt = 0;
        }
        //-----------------------------
        else if(rx.mode == RS485_BUFFER_raw){
            rx.data[rx.cnt] = rx.buffer;
            if(++rx.cnt >= 5){
                //sprintf(str, "- %d %d %d %d %d \r\n" , rx.data[0],rx.data[1],rx.data[2],rx.data[3],rx.data[4] ); DEBUG.println(str);

                rx.mode = RS485_BUFFER_RUNNING;
                if( rx.data[0] == buffer[INDEX_RX_ID] ){ //보낸쪽의 ID 가 맞는지
                    if(rx.data[1] == buffer[INDEX_RX_WHO] ){ //보낸쪽의 who 가 맞는지
                        rx.checksum = ~(rx.data[0] + rx.data[1] + rx.data[2] + rx.data[3]);
                        if( rx.checksum == rx.data[4]  ){ //SCHECU SUM 까지 맞다면
                            //아래부터 받음 처리 영역
                            rx.parm1 = rx.data[2]; //넣기
                            rx.parm2 = rx.data[3]; //넣기
                            rx.enable = true;
                            //DEBUG.println("recive ok");


                        }
                    }
                } 
            }
        }

        rx.before_buffer = rx.buffer; //저장


    }



    void TX1_CH(char c){ while(!(UCSR1A&0x20)); UDR1=c; } // 1바이트 송신
    void TX1_STR(char *s){ while(*s)TX1_CH(*s++); } // 문자열 송신





};

#endif
