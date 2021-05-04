#ifndef Link_h         
#define Link_h

#include<Arduino.h>
#include<avr/io.h>
#include<avr/interrupt.h>

#include "pin.h"


char esp32_rx[100] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
boolean read_esp32[50] = {0,0,0,0,0,0,0,0};
boolean esp32_jump = false;

#define G_ESP32_ISCA_1_REQUEST 0
#define G_ESP32_ISCD_1_REQUEST 1

boolean get_esp32_req(unsigned char parm){
    return read_esp32[ parm ];
}

void commend_esp32_recive(){
    if( ESP32.available() > 0 ){
        String rx = ESP32.readStringUntil('\n');
        
        if( rx.length() > 2 ){ //필터처리
            DEBUG.println( rx );
            strcpy( esp32_rx , rx.c_str() ); //복붙
            
            
            if( strstr(esp32_rx , "#ISCA,1") != NULL ){
                read_esp32[G_ESP32_ISCA_1_REQUEST] = 1;
                DEBUG.println("ESP32-> #ISCA,1 OK");
                ESP32.println("$#ISCA,1");
            }
            else if( strstr(esp32_rx , "#ISCD,1") != NULL ){
                read_esp32[G_ESP32_ISCD_1_REQUEST] = 1;
                DEBUG.println("ESP32-> #ISCD,1 OK");
                ESP32.println("$#ISCD,1");
            }
            
            else if( strstr(esp32_rx , "$JUMP") != NULL ){
                esp32_jump = true;
                DEBUG.println("ESP32-> JUMP");

            }
            
        }
        

    }

}




#endif
