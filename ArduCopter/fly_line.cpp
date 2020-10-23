#include "Copter.h"

uint32_t t_sta_line;                   //每段开始时间
uint32_t t_cur_line;                  //当前时间
uint32_t t_del_line = 0;             //每段当前运行时间
bool init_line = false;

//const Vector3f vel_ned_line(0.0f * 100.0f, 1.0f * 100.0f, 0.0f * 100.0f);
const Vector3f vel_ned_line(1.0f * 100.0f, 0.0f * 100.0f, 0.0f * 100.0f);
void Copter::fly_line(){
    if(flightmode->in_guided_mode()){
        if(!init_line){
            t_sta_line = AP_HAL::millis();
            init_line = true;
        }
        else
        {
            if(t_del_line <= 10 * 1000)
            {
                mode_guided.set_velocity(vel_ned_line, true, 0.0f, false, 0.0f, false, false);
                t_cur_line = AP_HAL::millis();
                t_del_line = t_cur_line - t_sta_line;
            }
        }
        
    }

}