#ifndef SIMEVENT_H
#define SIMEVENT_H
#include "packageIncludes.h"


class simEvent{

public:
    simEvent(QString typeOfEvent, int time, bool alarm);
    //callbox event
    simEvent(QString typeOfEvent, int time, int floor, QString direction);
    ~simEvent();

    //getters
    QString getType(){return eventName;}
    int getTime(){return timeOfEvent;}
    int getFloor(){return floor;}
    QString getDirection(){return direction;}
    bool getAlarm(){return alarm;}

private:


    //eventName: call, fire, power, overload
    QString eventName="notset";
    //This variable is redundant, as we are going to hold our events in a QMap where the key is our timestamp, and the value is our event. Keeping for redundency for now incase we choose to allow multiple events per timestamp.
    int timeOfEvent=0;

    //reserved for Fire Alarm or Power Out events
    bool alarm= false;

    //reserved for floor calls
    int floor =0;
    QString direction="down";






};



#endif // SIMEVENT_H
