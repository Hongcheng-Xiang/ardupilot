#include "Copter.h"

int state_usr = 0;                          //状态 0：若处在guided模式，解锁起飞； 1：起飞完成，切换成circle模式；
                                           //2:绕圈180°之后，切回guided模式，飞直线（地面x轴负向）   3：完成任务后，降落，上锁；
bool init_usr = false;                    //当前行动初始化完成的标志（比如circle模式）
bool result_ac_usr = false;              //行动执行结束的标志，判断以切换至下一个状态
bool print_usr = true;                  //控制台输出状态使能

uint32_t t_sta_usr;                   //每段开始时间
uint32_t t_cur_usr;                  //当前时间
uint32_t t_del_usr;                 //每段当前运行时间
int alt_tar_usr = 20;
int delt_alt = 2;
float radius_usr = 5000.0;       //cm
const Vector3f vel_ned_usr(-5.0f * 100.0f, 0.0f * 100.0f, 0.0f * 100.0f);      //mode_guided.set_velocity()的第一个参数，单位cms

//motors.armed(true); 解锁  motors.armed(false); 上锁
void Copter::fly_to_swh(){
    if(print_usr){
        hal.console->printf("current_state: %d\n",state_usr);
        print_usr = false;
    }
    switch(state_usr){
        case 0:  //若处在guided模式，解锁起飞；
            if(flightmode->in_guided_mode()){
                if(!init_usr){                                         //初始化：解锁
                    motors->armed(true);
                    init_usr = true;
                }
                else{                                              //初始化完成：起飞至目标高度
                    if(!result_ac_usr){
                        if((copter.current_loc.alt - alt_tar_usr * 100.0f) >= -delt_alt * 100.0f)    //减去目标高度 差值小于Δalt时视为达到指定高度
                            result_ac_usr = true;
                        copter.start_takeoff(alt_tar_usr);     //takeoff()函数在正常起飞过程中返回true，不在guided模式或者起飞完成返回false
                    }
                    else{                                    //执行完当前命令，进入下一个模式，切换模式、行动执行标志初始化
                        state_usr += 1;
                        init_usr = false;
                        result_ac_usr = false;
                        print_usr = true;                //控制台输出状态使能
                    }
                }
            }
            break;

        case 1:                                                 //起飞完成，切换成circle模式；
            //hal.console->printf("t_usr: %d\n",t_usr);
            if(!init_usr){                                    //初始化：设置转弯半径
                copter.circle_nav->set_radius(radius_usr);   //set_radius(float radius_cm)
                copter.circle_nav->set_rate(-9);            //set_rate(float deg_per_sec)
                t_sta_usr = AP_HAL::millis();              //AP_HAL::millis()单位ms
                t_del_usr = 0;
                init_usr = true;
                if(print_usr){
                    hal.console->printf("enter case1 init\n");
                    print_usr = false;
                }
            }
            else{    //设置完成：切换成circle模式
                if(!((t_del_usr > 30 * 1000) && (result_ac_usr))){     
                    result_ac_usr = copter.set_mode(Mode::Number::CIRCLE, ModeReason::MISSION_CMD);
                    t_cur_usr = AP_HAL::millis();             
                    t_del_usr = t_cur_usr - t_sta_usr;
                }
                else{
                    state_usr += 1;
                    init_usr = false;
                    result_ac_usr = false;
                    print_usr = true;               //控制台输出状态使能
                }
            }
            break;

        case 2:                                //飞完半圆，切换成guided模式，飞直线；
            if(!init_usr){                    //初始化：设置转弯半径
                t_sta_usr = AP_HAL::millis();               
                t_del_usr = 0;
                init_usr = copter.set_mode(Mode::Number::GUIDED, ModeReason::MISSION_CMD);
                if(print_usr){
                    hal.console->printf("enter case2 init\n");
                    print_usr = false;
                }
            }
            else{                                        //设置完成：设定x轴的速度  貌似是地面系
                if(!(t_del_usr > 20 * 1000)){     
                    //vel_ned_usr.x=1.5f;
                    //vel_ned_usr.y=0.0f;
                    //vel_ned_usr.z=0.0f;
                    mode_guided.set_velocity(vel_ned_usr, true, 0.0f, false, 0.0f, false, false);
                    t_cur_usr = AP_HAL::millis();
                    t_del_usr = t_cur_usr - t_sta_usr;
                }
                else{
                    state_usr += 1;
                    init_usr = false;
                    result_ac_usr = false;
                    print_usr = true;              //控制台输出状态使能
                }
            }
            break;

        case 3:                               //飞行计划结束，进行降落
            if(!init_usr){
                if(copter.current_loc.alt == 0)
                    init_usr = true;
                if(print_usr){
                    hal.console->printf("enter case3 init\n");
                    print_usr = false;
                }
                result_ac_usr = copter.set_mode(Mode::Number::LAND, ModeReason::MISSION_CMD);
            }
            else{                           //执行完当前命令，进入下一个模式，切换模式、行动执行标志初始化
                motors->armed(false);
                state_usr += 1;
                init_usr = false;
                result_ac_usr = false;
                print_usr = true;       //控制台输出状态使能
            }
            break;

        default:
            break;
    }
}