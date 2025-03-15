#include <simEvent.h>

//better practice would be to make Event subclasses for Elevator events and ECS events, but for this draft we can just keep some "unused" variables and have one large class


simEvent::simEvent(QString typeOfEvent, int time, bool alarm){
    eventName = typeOfEvent;
    timeOfEvent = time;
    this->alarm = alarm;
}


//floorCall Event
simEvent::simEvent(QString typeOfEvent, int time, int floor, QString direction){
    eventName = typeOfEvent;
    timeOfEvent = time;
    this->direction = direction;
    this->floor = floor;

}



simEvent::~simEvent(){};

