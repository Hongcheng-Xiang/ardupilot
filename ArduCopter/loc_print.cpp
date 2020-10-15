#include "Copter.h"


void Copter::loc_print(){
    
    hal.console->printf("lng:%d\n",copter.current_loc.lng);

    hal.console->printf("lat:%d\n",copter.current_loc.lat);

    hal.console->printf("alt:%d\n",copter.current_loc.alt);

    //ahrs.yaw  弧度
    hal.console->printf("yaw:%f\n",ahrs.yaw);
}