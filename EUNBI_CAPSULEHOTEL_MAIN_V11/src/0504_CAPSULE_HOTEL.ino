#include <MsTimer2.h>
#include <Arduino.h>
#include "pin.h"
#include "Gvar.h"
#include "Link.h"
#include "CommThread.h"

#include <avr/interrupt.h>
#include<avr/io.h>

#include "HardwareSerial.h"
#include "com_input.h"
#include "com_output.h"



int timer_cnt = 0;
int timer_scale = 0;
char str[100];

unsigned long before_millis = 0;
 
CommThread_Running thread_manager(false);
CommThread comwrite;

ISR(USART1_RX_vect){
    //char rx = UDR1;
    thread_manager.recive( UDR1 );
}


void TimerISR(){ //50ms 마다 실행 (20fps)
    
  if(++timer.scale >= 5){
      if(++timer.cnt == 2){
          
          set_bit(LED_SYSTEM_DDR, LED_SYSTEM_PIN);
          timer.cnt = 0;
      }else{
        clr_bit(LED_SYSTEM_DDR, LED_SYSTEM_PIN);
      }
    timer.scale = 0;
  }
  //--------------------------
    timer.en = true;
    
}


// ------------------------

void setup() {
  pin_init();
  serial_init();

  delay(1500);
  sprintf(str, "ISC_MAIN_v%d.%d Start..." , PROGRAM_VER / 10 , PROGRAM_VER % 10);   DEBUG.println(str);
  
  delay(1500);
  sprintf(str, "DEVICE Scan start!!" );   DEBUG.println(str);

  start_up_device();
  device_writer_schedule(); //여기서 장치 값 넣음

  MsTimer2::set(50, TimerISR);
  MsTimer2::start();

  delay(4000);

  
  before_millis = millis();
  comwrite.idx_reset();
}

//보내고 수신이 하든 안하든 false out이나서 false 출력함.
//Thread[0] 은 무조건 input 요청임


void loop() {
    if(before_millis + 100 <= millis() ){
        get_internal_data(); //내부핀 읽어오기 ><
        thread_manager.command_raw_loop(); //앞으로 이친구가 관리할것 입니다 하하.
        read_schedule();
        before_millis = millis(); //update
    }

    if(esp32_jump == true){
        comwrite.jump(); //점프해랑 ><
        esp32_jump = false;
    }

   device_writer_schedule(); //여기서 장치 값 넣음
   commend_esp32_recive(); //esp32 에서 값 읽음

    //샘플코드
    // comwrite.wait(p1, 131900); p1++;
    // comwrite.com_sound(p1, 1, D_MUSIC_BGM + 1, 1,1,1,1); p1++;
    // comwrite.finish(p1, false);
    // comwrite.com_relay_raw(p1, 1, D_RELAY_SET_1, S_RELAY_OFF, read_data_list[15],1,1,1); p1++;
                             //아래의 앤퍼센드는 스레드 특징임
    // com_12_th.com_motor_raw(0, &comwrite.dev.motor_raw[1], D_MOTOR_SET_4, S_MOTOR_OPEN, read_data_list[12], 1,1,1);
    // com_12_th.finish(1, false);


    // A   /    중국 비디오  /   LOW
    int p1 = 0;
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 벽조명 켜짐
    comwrite.com_sound();// BGM1 시작

    // B   /   휴대폰 상자의 자석스위치 닫아서 / LOW
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 이엠락 1번 잠김
    comwrite.com_relay_raw();// 라운지 홀조명 두개 켜짐
    comwrite.com_relay_raw();// 라운지 벽조명 꺼짐
    comwrite.com_sound();// 라운지 스피커에서 따르릉 네~입니다 소리 들림
    comwrite.com_relay_raw();// 멘트 끝나면, 이엠락 2 열림

    // C   /   가정용 기성품 스위치를 켜면 / ????
    comwrite.com_motor_raw();// 강가문 액추에이터 줄어든다.
    comwrite.com_relay_raw();// 라운지 홀조명 두 개 오프
    comwrite.com_sound();// A5에서 켜졌던 라운지 BGM 스피커 오프
    comwrite.com_relay_raw();// 강가 태양 조명 ON
    comwrite.com_sound();// 강가 스피커 켜서 BGM2 나옴
    
    // D   /   강가자물쇠 풀고 기성품 스위치 켜기 / ???? 
    comwrite.com_sound();// C의 강가 스피커 꺼짐
    comwrite.com_relay_raw(); // C의 강가 태양조명 꺼짐
    comwrite.com_relay_raw(); // C의 라운지 홀조명 두 개 켜짐
    comwrite.com_sound();// C의 라운지 스피커 켜짐 BGM1
    comwrite.com_relay_raw();// B의 이엠1번이 열린다.
    comwrite.wait();// 10초 딜레이
    comwrite.com_motor_raw();// 강가 문 액추에이터 길어짐

    /////// E (삭제된 시퀀스임)

    // F   /   라운지 중국 비디오 꺼지면 / LOW
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; //라운지 LED바 (초기값)OFF -> ON
    comwrite.com_sound();//라운지 스피커 음악 변경 BGM1이 -> BGM3으로 변경
    comwrite.com_relay_raw();//라운지 가장 안쪽 원탁등 (초기값 OFF)->켜짐
    comwrite.com_relay_raw();//원탁등 왼쪽에 있는 이엠락 3번이 (초기값 닫힘) -> 열림 

    ////// G (중국시퀀스이므로 생략)

    // H   /   중국 IC5구 / LOW
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 원탁등 꺼짐
    comwrite.com_relay_raw();// F2에서 켜진 LED바가 뚝 꺼진다.
    comwrite.com_sound();// F의 BGM3스피커 꺼짐
    comwrite.com_relay_raw();// 중국 비디오 스위치에 전원을 넣어준다 (Relay OFF -> ON)
    // 중국 비디오가 중국 비디오 스위치에 의해 재생된다.
    comwrite.com_motor_raw();// 중국 영상 끝나면, 2차 HIGH -> LOW 받아서, 라운지 대형 액추에이터 축소되어있던게 확장된다.
    comwrite.com_sound();// 캡슐호텔방 천장 스피커 BGM4번 켜짐

    ///// I (중국시퀀스이므로 생략)

    // J   /   말풍선 서랍 자석스위치 떨어지면 / HIGH
    comwrite.wait(); // 5초 딜레이
    // ESP8266 내 장비가 태블릿에 신호를 준다.
    // 캡슐방 태블릿 (초기값)대기화면이 검정색임 -> 켜짐

    // K   /   중국 시계 맞추면 / LOW
    comwrite.com_relay_raw();// 캡슐호텔 네 칸 중 1층 우측 칸, 서랍에 이엠락5 닫혀있다가 열림.
    comwrite.com_sound();// BGM4번 나오고 있는 스피커에서 BGM5로 바뀜 / bgm5 삭제, bgm4계속 재생
    comwrite.com_relay_raw();// 캡슐호텔 LED 1개 (초기값 켜져있던 거) 꺼짐, 테마 입구와 가까운 쪽만 꺼짐.

    // L   /   골목 좌측 입구 자석스위치 열리면 / HIGH
    // 골목 좌측 입구의 자석스위치가 초기값은 닫혀있음.
    // 아날로그 퍼즐을 풀면 자석스위치가 열리면,
    // 자석스위치 닫히면, ((정리하면, 자석스위치 HIGH -> LOW -> HIGH ))
    comwrite.com_relay_raw();//골목 좌측 입구의 EM락6번 (초기값 열림) -> 닫힘
    comwrite.com_sound();//캡슐의 스피커 BGM4가 꺼짐 
    comwrite.com_sound();//펍의 스피커에서 BGM5이 나옴

    ////// 중국시퀀스 혹은 생략 M,N,O

    // O `   /   펍 상자 안 기성품스위치 / ????
    // 앱에서 나온 힌트로, 상자 퍼즐을 풀어서, 그 안에 들어있던 기성품 스위치를 OFF->ON
    // 펍 태블릿이 신호를 받는다.
    
    // P   /   골목 창문 열면    
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 중국 비디오 나오고
    comwrite.wait();// 5초 딜레이
    comwrite.com_sound();// 펍에서 나오고 있는 BGM5이 꺼진다.
    comwrite.com_relay_raw();// 펍의 조명이 꺼진다.
    comwrite.com_relay_raw();// 골목 왼쪽입구 (초기값 꺼짐)맞은편 벽등 켜진다.
    comwrite.com_sound();// 캡슐방 스피커 & 골목 쪽 스피커 BGM6켜짐
    comwrite.com_sound();

    // Q   /   골목 IC5구 / LOW
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 골목 우측 입구 이엠락7번 (초기값닫혀있던게) 열림

    // R   /   캡슐 2층 우측 IC3구 / ????
    comwrite.com_relay_raw(p1, ???, D_RELAY_SET_???, S_RELAY_OFF, read_data_list[???],1,1,1); p1++; // 캡슐 LED바 테마 입구에서 먼 쪽 1개 꺼짐.
    comwrite.com_relay_raw();// 캡슐 프로젝터와 연결된. 중국 비디오용 릴레이 켜짐.
    comwrite.wait();// 딜레이 20초
    comwrite.com_relay_raw();// 캡슐 LED바 2개가 깜빡 깜빡 

    ////// S (중국시퀀스이므로 생략)

    // T   /   캡슐 2층 좌측 방에 있는 가정용 스위치 / ????
    comwrite.wait();// (중국 영상 나오는 동안)딜레이 약 1분
    comwrite.com_motor_raw();// 대형 액추에이터 펼쳐진 것이 -> 접힌다.
    comwrite.com_relay_raw();// 강가 태양조명 켜짐 + 강가로 가는 계단 조명 함께 켜짐
    comwrite.com_relay_raw();
    comwrite.com_sound();// 캡슐 BGM6번 꺼짐
    comwrite.wait();// 딜레이 5초
    comwrite.com_sound();// 강가 스피커 BGM9 / bgm9삭제, c에서 나온 bgm2다시 재생

    // U   /   강가 2층 벽면에 레버가 정답이 되면 / ????
    comwrite.com_sound();// 강가 스피커 BGM2꺼짐
    comwrite.com_relay_raw();// 강가 태양조명 꺼짐
    comwrite.com_relay_raw();// 중국 영상 나옴
    comwrite.com_motor_raw();// 테마 입구 오른쪽 벽면에 있는 스위치로, 대형 액추에이터를 접는다

  //---------아래는 샘플코드---------------------------------

    //comwrite.com_relay_raw(0, 1, D_RELAY_SET_1, S_RELAY_OFF, read_data_list[15],1,1,1);
    //comwrite.com_relay_raw(1, 1, D_RELAY_SET_2, S_RELAY_OFF, read_data_list[0],1,1,1);
    // comwrite.com_sound_ch_raw(0, 1, 0b10101111, 1, 1, 1, 1);
    // comwrite.com_sound(1, 1, D_MUSIC_BGM + 1, 1,1,1,1); 
    // comwrite.wait(2, 2500);
    // comwrite.com_sound_ch_raw(3, 1, 0b00000011, 1, 1, 1, 1);
    // comwrite.com_sound(4, 1, D_MUSIC_EVENT + 2, 1,1,1,1); 
    // comwrite.wait(5, 4500);
    // comwrite.com_sound_ch_raw(6, 1, 0b00001111, 1, 1, 1, 1);

    // comwrite.finish(7, false);


    //comwrite.com_sound(4, 1, D_MUSIC_BGM + 2, 1,1,1,1); 
    //comwrite.com_relay_pwm_all(5, &comwrite.dev.relay_pwm_all[1], MODE_DIMMIG, 1, read_data_list[4],1,1,1); 
  //_------------------------------------------------------------------
}

void device_writer_schedule(){ // 사용할 보드 등록
  //장비 목록 : 인풋 1개, 릴레이 2개, 모터 1개, 사운드 1개 
    thread_manager.add_raw_schedule(0, ISC_INPUT, 1, I_INPUT_RAW, 1); // 인풋보드 1개
    
    thread_manager.add_raw_schedule(1, ISC_RELAY, 1, W_RELAY_RAW, comwrite.dev.relay_raw[1] ); // 릴레이보드 2개
    thread_manager.add_raw_schedule(2, ISC_RELAY, 2, W_RELAY_RAW, comwrite.dev.relay_raw[2] );
    
    thread_manager.add_raw_schedule(3, ISC_AUDIO, 1, W_SOUND_BGM, comwrite.dev.sound_mp3[1]); // 오디오 1개
    thread_manager.add_raw_schedule(4, ISC_AUDIO, 1, W_SOUND_SET_CH, comwrite.dev.sound_ch[1] );
    
    thread_manager.add_raw_schedule(5, ISC_MOTOR, 1, W_MOTOR_RAW, comwrite.dev.motor_raw[1] ); // 모터 1개
    
    load_input(0, thread_manager.get_parm1(0), thread_manager.get_parm2(0) ); //인풋보드 1개
    //thread_manager.add_raw_schedule(0, ISC_INPUT, 1, I_INPUT_RAW, 1); // 인풋보드가 한 개 이므로.

    //thread_manager.add_raw_schedule(1, ISC_RELAY, 1, W_PWM_CH_ALL, comwrite.dev.relay_pwm_all[1] );
    //thread_manager.add_raw_schedule(2, ISC_RELAY, 1, W_RELAY_RAW, comwrite.dev.relay_raw[1] );
    //thread_manager.add_raw_schedule(0, ISC_AUDIO, 1, W_SOUND_BGM, comwrite.dev.sound_mp3[1] );
    //thread_manager.add_raw_schedule(1, ISC_AUDIO, 1, W_SOUND_SET_CH, comwrite.dev.sound_ch[1] ); //채널

    //thread_manager.add_raw_schedule(7, ISC_VIDEO, 1, W_VIDEO_PLAY, comwrite.dev.video_play[1] );
    //thread_manager.add_raw_schedule(8, ISC_MOTOR, 1, W_MOTOR_RAW, comwrite.dev.motor_raw[1] );
        
    //load_input(0, thread_manager.get_parm1(0), thread_manager.get_parm2(0) ); //외부 인풋1
}

void read_schedule(){ // 반복 상태 읽기
    read_data_list[0] = get_input_Internal(GET_PORT_1, IN_LOW); // 1번포트 : 1번중국비디오 / 초기값 high
    read_data_list[1] = get_input_Internal(GET_PORT_2, IN_LOW); // 2번포트 : 자석스위치(입구폰상자) / high
    read_data_list[2] = get_input_Internal(GET_PORT_3, IN_HIGH); // 3번포트 : 앤틱1층 강가문여는 가정용 스위치 / low
    read_data_list[3] = get_input_Internal(GET_PORT_4, IN_HIGH); // 4번포트 : 강가방에 있는 가정용 스위치 / LOW
    read_data_list[4] = get_input_Internal(GET_PORT_5, IN_HIGH); // 5번포트 : 대형액추에이터 탈출용 손님스위치 / LOW
    read_data_list[5] = get_input_Internal(GET_PORT_6, IN_HIGH); // 6번포트 : 펍 상자 속 가정용 스위치(태블릿) / LOW
    read_data_list[6] = get_input_Internal(GET_PORT_7, IN_LOW); // 7번포트 : 중국 ic 5구 / high
    read_data_list[7] = get_input_Internal(GET_PORT_8, IN_HIGH); // 8번포트 : 말풍선 서랍 자석스위치 / LOW
    read_data_list[8] = get_input_Internal(GET_PORT_9, IN_LOW); // 9번포트 : 중국 시계 / high
    read_data_list[9] = get_input_Internal(GET_PORT_10, IN_HIGH); // 10번포트 : 골목 좌측 입구의 자석스위치 / LOW

                                 //첫 번째 인자는 인풋보드의(load_input) idx다
    read_data_list[10] = get_input_ext(0, GET_PORT_1, IN_LOW); // 캡슐 2층 우측의 ic 3구 / 초기값 high
    read_data_list[11] = get_input_ext(0, GET_PORT_2, IN_LOW); // 골목 IC 5구 / high
    read_data_list[12] = get_input_ext(0, GET_PORT_3, IN_LOW); // 캡슐 2층 좌측방 가정용 스위치 / high
    read_data_list[13] = get_input_ext(0, GET_PORT_4, IN_LOW); // (강가 레버스위치) 중국비디오 2차 / high
    read_data_list[14] = get_input_ext(0, GET_PORT_5, IN_HIGH); // 골목에 있는 창문 자석스위치 / loW
   
    //read_data_list[0] = get_input_Internal(GET_PORT_12, IN_LOW); // 직원용 스위치 누르면
    //read_data_list[5] = get_input_Internal(GET_PORT_3, IN_HIGH); // 운수 / 자석-운수회사 자석스위치2 ???하면
    //read_data_list[6] = get_input_ext(0, GET_PORT_7, IN_HIGH); // 구멍 / 자석 매대상단 스위치 ???하면
    //read_data_list[7] = get_input_ext(0, GET_PORT_6, IN_HIGH); // 장미 / 술집바닥문왼쪽 쇼파옆자석 ???하면
  
  //----------- 아래는 log 추적용
  sprintf(str, "<in0> %d %d %d %d %d %d %d %d", read_data_list[0], read_data_list[1], read_data_list[2], read_data_list[3],
  read_data_list[4], read_data_list[5], read_data_list[6], read_data_list[7]  ); DEBUG.println(str);

  // sprintf(str, "<in8> %d %d %d %d %d %d %d %d %d ", read_data_list[8], read_data_list[9], read_data_list[10], read_data_list[11],
  // read_data_list[12], read_data_list[13], read_data_list[14], read_data_list[15] , read_data_list[16]   ); DEBUG.println(str);

  // sprintf(str, "<key> %d %d %d %d S%d ", read_data_list[17], read_data_list[18], read_data_list[19], read_data_list[20] , read_data_list[21] ); DEBUG.println(str);
}

void start_up_device(){ // 초기 세팅

  DEBUG.println("RESET....");
  int p1 = 0;
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_1, S_RELAY_OFF, 1,1,1,1); p1++; // r1-1 이엠락 1번 폰상자 / 초기값 LOW
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_2, S_RELAY_ON, 1,1,1,1); p1++; // r1-2 이엠락 2번 앤틱 위에서 1번째 / HIGH
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_3, S_RELAY_ON, 1,1,1,1); p1++; // r1-3 이엠락 3번 원탁등 아치서랍 / HIGH
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_4, S_RELAY_ON, 1,1,1,1); p1++; // r1-4 이엠락 4번 캡슐 1층 우측칸 서랍 / HIGH
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_5, S_RELAY_OFF, 1,1,1,1); p1++; // r1-5 이엠락 5번 골목 좌측 입구 / LOW
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_6, S_RELAY_ON, 1,1,1,1); p1++; // r1-6 이엠락 6번 골목 우측 입구 / HIGH
  comwrite.com_relay_raw(p1, 1, D_RELAY_SET_7, S_RELAY_ON, 1,1,1,1); p1++; // r1-7 이엠락 7번 캡슐 2층 좌측 / HIGH
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_1, S_RELAY_OFF, 1,1,1,1); p1++; // r2-1 라운지홀조명 두 개 / LOW
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_2, S_RELAY_OFF, 1,1,1,1); p1++; // r2-2 라운지벽조명 / LOW
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_3, S_RELAY_OFF, 1,1,1,1); p1++; // r2-3 라운지 가장 안쪽 원탁등 / LOW
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_4, S_RELAY_OFF, 1,1,1,1); p1++; // r2-4 라운지 LED바 / LOW
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_5, S_RELAY_ON, 1,1,1,1); p1++; // r2-5 캡슐방 LED바 1번 / HIGH
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_6, S_RELAY_ON, 1,1,1,1); p1++; // r2-6 캡슐방 LED바 2번 / HIGH
  comwrite.com_relay_raw(p1, 2, D_RELAY_SET_7, S_RELAY_OFF, 1,1,1,1); p1++; // r2-7 강가 태양 조명 / LOW
  
  comwrite.com_motor_raw(p1, 1, D_MOTOR_SET_1, S_MOTOR_OPEN, 1,1,1,1); p1++; // m1-1 강가문 액추 / LONG 
  comwrite.com_motor_raw(p1, 1, D_MOTOR_SET_2, S_MOTOR_CLOSE, 1,1,1,1); p1++; // m1-2 라운지 대형 액추 / SHORT
  
  comwrite.com_sound_ch_raw(p1, 1, 0b00000000, 1,1,1,1); p1++; // s1-1 라운지 스피커 / OFF (0b00 = 고정 | 뒤에 6칸은 포트다.)
                                                              // 이벤트를 특정한 스피커에서 재생하고 싶으면 다른 곳은 다 꺼둬야 한다.
  comwrite.finish(p1, false);
  // s1-2 캡슐방 천장 스피커 / OFF
  // s1-3 펍 스피커 / OFF
  // s1-4 골목 스피커 / OFF
  // s1-5 강가 스피커 / OFF


  // 아래는 샘플 코드
  //comwrite.com_relay_raw(p1, 3, D_RELAY_SET_8, S_RELAY_OFF, 1,1,1,1); p1++; 
  //comwrite.com_relay_raw(p1, 3, D_RELAY_SET_3, S_RELAY_ON, 1,1,1,1); p1++; // 길거리로 나가는 em락 닫아두
  //comwrite.com_video(p1, 1, COM_VIDEO_NOLOOP, 0, 1,1,1,1); p1++;

  DEBUG.println("RESET OK!");
  delay(1000);
}