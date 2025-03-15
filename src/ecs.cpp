#include "ecs.h"


ECS::ECS()
{
    alarm=false;

}




//assignCall assumes that the event we are processing is a call event. This function is NOT responsible for determining if an elevator should take the call or not.
bool ECS::assignCall(simEvent* newEvent, elevator* elevator){

    if(newEvent->getType().compare("call")!=0){
        qInfo("tried to assign non call event");
        return false;
    }
    elevator->addRequest(newEvent->getFloor());
    eventList.removeOne(newEvent);
   return true;
}


void ECS::addEvent(simEvent* newEvent){
    if(newEvent->getAlarm()){
        alarm=true;
        //clear our eventList, then add our alarmEvent
        eventList.clear();
        eventList.append(newEvent);
        qInfo()<<"added ALARM event to eventList in ECS with newSize of: "<<eventList.size();
    }
    //add our new event to the back of the list!
    else {eventList.append(newEvent);
    qInfo()<<"added event to eventList in ECS with newSize of: "<<eventList.size();}
}


//look at all the call events in our eventList and see if we can assign that call based on the current direction, floor and final destination floor of each elevator. If yes, assign the call and remove it from our list.
//will make use of
void ECS::distributeCallEvents(){
    int floor=0;
    if(eventList.size()==0){

        return;
    }
    else
        for(simEvent* events : eventList){
            qInfo()<<"distributing event with time: "<<events->getTime() << " of type: "<<events->getType()<<" with floor: "<<events->getFloor()<<" and direction: "<<events->getDirection();
            if(events->getType().compare("call")!=0){
                //if our event is not a call event, go to our next entry in our eventList
                continue;
            }
            else{
                qInfo()<<"call request, searching for idle elvators.";
                floor = events->getFloor();
                //find an elevator that can take our request. Will only assign the request if our request is between the current and final destination, and in the same direction.
                for(elevator* elevator : elevatorList){
                    //parse our elevator info for readability
                    int currentFloor = elevator->getFloor();
                    int finalDestination = elevator->getFinalDestination();
                    QString direction = elevator->getDirection();
                    if(elevator->isIdle()){
                        qInfo("ECS assigned request to idle elevator");
                        assignCall(events,elevator);
                        elevator->setServiceDirection(events->getDirection());
                        break;
                    }
                    else if(events->getDirection().compare(direction)==0 && direction.compare("up")==0){
                        if(floor>currentFloor && floor <=finalDestination){
                            qInfo()<<"request for floor: "<<events->getFloor()<< " and direction: "<<events->getDirection()<<" assigned";
                            assignCall(events,elevator);
                            break;
                        }
                    }
                    else if(events->getDirection().compare(direction)==0 && direction.compare("down")==0){
                        if(floor<currentFloor && floor >=finalDestination){
                            qInfo()<<"request for floor: "<<events->getFloor()<< " and direction: "<<events->getDirection()<<" assigned";
                            assignCall(events,elevator);
                            break;
                        }
                    }
                    qInfo()<<"ECS found no suitable elevator found to handle request.";
                }
            }

        }

}

void ECS::distributeAlarmEvent(){
    qInfo()<<"distributing Alarm event";
    //send the alarmType to all of our elevators.
    for(elevator* elevator : elevatorList){
        elevator->activateAlarm(eventList.front()->getType());
    }
    eventList.clear();
}

void ECS::iterateECS(){
//if alarm is active, we ONLY want to distribute our alarm signal on each iteration. Our call eventQueue should be empty, but this is just in case.
    if(!eventList.empty()){
        if(alarm){
        distributeAlarmEvent();
            }
        //if alarm is not active, distribute call events.
        else if(!alarm){
            distributeCallEvents();
            }
    }


}



