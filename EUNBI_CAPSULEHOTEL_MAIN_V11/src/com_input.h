#ifndef com_input_h         
#define com_input_h

#include <Arduino.h>
#include "pin.h"
#include "IDLIST.h"

//일단 읽는거
unsigned char recive_raw[100][2]; //y x
boolean read_data_list[100];
boolean internal_read[15];

#define PARM1 0
#define PARM2 1

void get_internal_data(){ // 0 ~ 11까지임
    
    for(int i=0; i<12; i++){
        if(i <= 7){
            internal_read[i] = digitalRead(i + 22) ; //22 ~ 29
        }else{
            internal_read[i] = digitalRead(i + 26) ; //34 ~ 37
        }
    }
}



void load_input( int idx , unsigned char  _parm1, unsigned char _parm2){
    recive_raw[idx][PARM1] = _parm1;
    recive_raw[idx][PARM2] = _parm2;
}


boolean get_input_Internal(int _port, boolean en ){ //어느거를 불러올꺼냐 , 포트정보
    // 0 ~ 11을 1 ~ 12로 보정함

    boolean rt = false; //rturn
    if( internal_read[_port] == 0 ){ //high 일 경우  위에서 반전되서 나와서 여기서 반전함
        if(en == 0){
            rt = 0;
        }else{
            rt = 1;
        }
        

    }else{
        if(en == 0){
            rt = 1;
        }else{
            rt = 0;
        }
        
    }

    return rt;

}
boolean get_keypad(int _data_idx, boolean _reverse ){ //어느거를 불러올꺼냐 , 포트정보
    
    boolean rt = false;

    if(recive_raw[_data_idx][PARM1] == 5){ //참
        if(_reverse == true){
             rt = false;
        }else{
            rt = true;
        }
    }else{
        if(_reverse == true){
             rt = true;
        }else{
            rt = false;
        }        
    }

    return rt;
}




boolean get_tof(int _data_idx, boolean _reverse ){ //어느거를 불러올꺼냐 , 포트정보
    
    boolean rt = false;

    if(recive_raw[_data_idx][PARM1] == 5){ //참
        if(_reverse == true){
             rt = false;
        }else{
            rt = true;
        }
    }else{
        if(_reverse == true){
             rt = true;
        }else{
            rt = false;
        }        
    }

    return rt;
}


boolean get_input_ext(int _data_idx, int _port, boolean en ){ //어느거를 불러올꺼냐 , 포트정보

    int req = check_bit( recive_raw[_data_idx][PARM1]  , _port ); //질문
    int answer = 0x01 << _port; //정답

    boolean rt = false; //rturn
 
    if( req == answer ){ //high 일 경우
        if(en == 0){
            rt = 0;
        }else{
            rt = 1;
        }
        

    }else{
        if(en == 0){
            rt = 1;
        }else{
            rt = 0;
        }
        
    }

  

    return rt;

}



#endif
