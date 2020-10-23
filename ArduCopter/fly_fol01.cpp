#include "Copter.h"


//状态0：调整高度；状态1：调整位置；状态3：调整航向
int state_fol01 = 0;

bool init_fol01 = false;
bool result_ac_fol01 = false;
bool print_fol01 = true;

uint32_t t_sta_fol01;                   //每段开始时间
uint32_t t_cur_fol01;                  //当前时间
uint32_t t_del_fol01;                 //每段当前运行时间


//调试所设置的固定点（经纬度、高度）
uint32_t leader_lat = 10;

void Copter::fly_fol01(){
    
    
    if(print_fol01){
        hal.console->printf("current_state:%d\n",state_fol01);
        print_fol01 = false;
    }
    switch (state_fol01)
    {
    case 0:
        if (flightmode->in_guided_mode())
        {
            
        }
        
        break;
    
    default:
        break;
    }

}