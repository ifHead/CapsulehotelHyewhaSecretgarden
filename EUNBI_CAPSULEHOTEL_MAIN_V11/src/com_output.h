#ifndef com_output_h         
#define com_output_h

#include <Arduino.h>
#include "IDLIST.h"
#include "pin.h"
// 여러개 스레드 + 


class CommThread_mini{
   private:
     struct  run_s
     {
         int status;
     };

     run_s run;
     unsigned long before_time = 0;
     char str[100];
     
    public:
        CommThread_mini(){ //생성자

        }

        void millis_load(){
            before_time = millis();
        }

        void wait(int idx , unsigned long delay_time){
            if(idx == run.status){          
                if(before_time + delay_time <= millis() ){
                    sprintf(str, "$MT-W%d WAIT ",idx );
                    ESP32.println(str);
                    DEBUG.println(str);  
                    set_Next_Run_mode();
                }
            }

        }

        void com_sound(int idx , int *_data , int sound , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true)){ 
                    int da = *_data;
                    da = sound;
                    
                    sprintf(str, "$MT-W%d SOUND: %d ", idx , da );
                    ESP32.println(str);
                    DEBUG.println(str);  
                    
                    *_data = da;
                    set_Next_Run_mode();
                }
            }
        }      

        void com_relay_pwm_all(int idx , int *_data , int mode, int _pwm , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){  
                if( (term1==true && term2==true && term3==true && term4==true)){ 
                    int da = *_data;
                    if(mode == MODE_DIMMIG){ 
                        da = _pwm;
                    }else{
                        da = 50 + _pwm;
                    }
                
                    sprintf(str, "$MT-W%d PWMALL: %d ",idx , _pwm );
                    ESP32.println(str);
                    DEBUG.println(str);  

                    *_data = da;
                    set_Next_Run_mode();
                }
            }
        }

        void com_motor_raw(int idx , int *_data, unsigned char motor_ch , unsigned char motor_mode , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( term1==true && term2==true && term3==true && term4==true ){ 
                    int da = *_data;
                    int mt = motor_ch + 4;
                    if(motor_mode == 1){
                        set_bit( da ,motor_ch); //반전 시키기 위해
                        clr_bit( da ,mt);
                        sprintf(str, "M setbit %d " , da );
                        DEBUG.println(str); 
                    }
                    else if(motor_mode == 0){
                        clr_bit(da , motor_ch);
                        clr_bit( da ,mt);
                        sprintf(str, "M CLRbit %d " , da );
                        DEBUG.println(str);
                    }
                    else if(motor_mode == 3){
                        set_bit( da ,mt);
                        sprintf(str, "M OFFbit %d " , da );
                        DEBUG.println(str);
                    }
                    sprintf(str, "$MT-W%d MOTOR[%d] %d->%d ",idx , da, motor_ch, motor_mode);
                    ESP32.println(str);
                    DEBUG.println(str); 

                    *_data = da;
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }

  
        void com_relay_raw(int idx , int *_data, unsigned char relay_ch , boolean relay_mode , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( term1==true && term2==true && term3==true && term4==true ){ 
                    int da = *_data;
                    if(relay_mode == 1){
                        set_bit( da ,relay_ch); //반전 시키기 위해
                    }else{
                        clr_bit(da , relay_ch);
                    }
                    sprintf(str, "$W%d RL_R %d %d ",idx , relay_ch, relay_mode);
                    ESP32.println(str);
                    DEBUG.println(str); 
                    *_data = da;
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }


        void finish(int idx , boolean loop_mode){
            if(idx <= run.status ){
                  if(loop_mode == true){
                      run.status = 0;
                  }else{
                      run.status = idx; //끝
                  }

            }
        }
  
        void set_Next_Run_mode(){
            before_time = millis();
            run.status++;
        }
};

class CommThread{
    private:
     struct  run_s
     {
         int status;
     };

     run_s run;
     long before_time = 0;
     char str[100];
     
    public:
        struct dev_s{
            int relay_raw[50];
            int relay_pwm[50][3];
            int relay_pwm_all[50];
            int sound_mp3[50];
            int sound_ch[50];
            int video_play[50];
            int motor_raw[50];
            int sleep[50];
            int relay_pwm_warring[50];
        };

        dev_s dev;
        boolean jump_pass = false;


        CommThread(){ //생성자

        }

        void millis_load(){
            before_time = millis();
        }

         void com_sleep_set(int idx , int id , int sp_md , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                
                    dev.sleep[id] = sp_md;

                    sprintf(str, "$W%d SLEEP: %d ",idx , sp_md );
                    ESP32.println(str);
                    DEBUG.println(str);  

                    jump_pass = false;
                    set_Next_Run_mode();
                }
            }
        }       
        
        void wait(int idx , unsigned long delay_time){
            if(idx == run.status){
                
                if(before_time + delay_time <= millis() ){
                    sprintf(str, "$W%d WAIT ",idx );
                    ESP32.println(str);
                    DEBUG.println(str);  
                    set_Next_Run_mode();
                }
            }

        }

        void com_esp32(int idx , int id , char port,  boolean term1, boolean term2, boolean term3, boolean term4 ){
            if(idx == run.status){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){
                    
                    char st[50];
                    sprintf(st, "#EWK,%d,%d," , id, port);
                    ESP32.println(st); //보내주세요
                    DEBUG.println(st);
                    
                    jump_pass = false;
                    set_Next_Run_mode();
                }
            }
        }



        void com_motor_raw(int idx , int id, unsigned char motor_ch , int motor_mode , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                    unsigned char mt = motor_ch + 4;
                    if(motor_mode == 1){
                        set_bit( dev.motor_raw[id] ,motor_ch);
                        clr_bit( dev.motor_raw[id] ,mt);
                        sprintf(str, "M setbit %d " , dev.motor_raw[id]);
                        DEBUG.println(str); 
                    }
                    else if(motor_mode == 0){
                        clr_bit(dev.motor_raw[id] , motor_ch);
                        clr_bit( dev.motor_raw[id] ,mt);  
                        sprintf(str, "M CLRbit %d " , dev.motor_raw[id]);
                        DEBUG.println(str);
                    }
                    else if(motor_mode == 3){
                        set_bit( dev.motor_raw[id] ,mt);  
                        sprintf(str, "M oOFFbit %d " , dev.motor_raw[id]);  
                        DEBUG.println(str);
                    }


                    sprintf(str, "$W%d MOTOR[%d] %d->%d ",idx , dev.motor_raw[id], motor_ch, motor_mode);
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }

        
        void com_video(int idx , int id , int mode, int play_num , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){  
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                    if(mode == COM_VIDEO_LOOP){ 
                            dev.video_play[id] = play_num;
                    }else{
                        dev.video_play[id] = 50 + play_num;
                    }
                    
                 
                     
                    sprintf(str, "$W%d VIDEO: %d ",idx , play_num );
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
        }       

        void com_sound(int idx , int id , int sound , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                
                    dev.sound_mp3[id] = sound;

                    sprintf(str, "$W%d SOUND: %d ",idx , sound );
                    ESP32.println(str);
                    DEBUG.println(str);  

                    jump_pass = false;
                    set_Next_Run_mode();
                }
            }
        }      

        void com_sound_ch_raw(int idx , int id, unsigned char sound_ch , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                        dev.sound_ch[id] = sound_ch;
              
                    sprintf(str, "$W%d SOUND_CH %d",idx , sound_ch);
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }    

        void com_relay_pwm_all(int idx , int id , int mode, int _pwm , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){  
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                    if(mode == MODE_DIMMIG){ 
                            dev.relay_pwm_all[id] = _pwm;
                    }else{
                        dev.relay_pwm_all[id] = 50 + _pwm;
                    }
                
                    sprintf(str, "$W%d PWMALL: %d ",idx , _pwm );
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
        }                

        void com_relay_pwm_warring(int idx , int id, int _set , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                
                    dev.relay_pwm_warring[id] = _set;

                    sprintf(str, "$W%d R_WARR %d ",idx ,_set );
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }

        void com_relay_raw(int idx , int id, unsigned char relay_ch , boolean relay_mode , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                
                    if(relay_mode == 1){
                        set_bit( dev.relay_raw[id] ,relay_ch); //반전 시키기 위해
                    }else{
                        clr_bit(dev.relay_raw[id] , relay_ch);
                    }
                    sprintf(str, "$W%d RL_R %d %d ",idx , relay_ch, relay_mode);
                    ESP32.println(str);
                    DEBUG.println(str); 

                    jump_pass = false; 
                    set_Next_Run_mode();
                }
            }
            //dev.relay_raw[id]


        }

        
        void com_relay_all(int idx , int id , boolean relay_mode , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                
                    if(relay_mode == 1){
                        dev.relay_raw[id] = 255;
                    }else{
                        dev.relay_raw[id] = 0;
                    }

                    sprintf(str, "$W%d RL_ALL: %d ",idx , relay_mode );
                    ESP32.println(str);
                    DEBUG.println(str);  
                    jump_pass = false;
                    set_Next_Run_mode();
                }
            }
        }

        void com_relay_pwm(int idx , int id , unsigned char pwm_port , char pwm_dimming , boolean term1, boolean term2, boolean term3, boolean term4 ){ //릴레이를 제어함
            if( idx == run.status ){
                if( (term1==true && term2==true && term3==true && term4==true) || jump_pass == true ){ 
                    dev.relay_pwm[id][pwm_port] = pwm_dimming;

                    sprintf(str, "$W%d RL_ALL: %d %d ",idx , pwm_port , pwm_dimming);
                    ESP32.println(str);
                    DEBUG.println(str);  
                    jump_pass = false;
                    set_Next_Run_mode();
                }
            }
        }        


        void finish(int idx , boolean loop_mode){
            if(idx <= run.status ){
                  if(loop_mode == true){
                      run.status = 0;
                  }else{
                      run.status = idx; //끝
                  }

            }
        }
  
        void set_Next_Run_mode(){
            before_time = millis();
            run.status++;
        }


        int getStatus(){
            return run.status;
        }

        void jump(){
            jump_pass = true;
        }

        void idx_reset(){
            run.status = 0;
        }


};






#endif