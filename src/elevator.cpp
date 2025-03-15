#include <elevator.h>

elevator::elevator(int startFloor, int name, QVector<rider*>* riderQueue, int passengerLimit): riderQueue(riderQueue), currentFloor(startFloor), passengerLimit(passengerLimit){
    idle = true;
    status = "idle";
    floorProgress =0;
    doorTimer = 0;
    this->name = name;
    typeOfAlarm = "inactive";
    alarm = false;
    //Idea was originally to use a -1 for our FDF, but using our current floor makes more sense in the case of an idle elevator.
    finalDestination = currentFloor;
    direction = "up";
    doorOpenButton=false;
    doorCloseButton=false;
    serviceDirection = "idle";
    obstructionSensor = false;
    numRiders=0;


}
elevator::~elevator(){

};


//addRequest returns true if floor did not exist in current queue and was successfully added.
//Also updates our finalDestination Floor if required. This is only to be used by our ECS and when a passenger boards the elevator.
bool elevator::addRequest(int floor){
    if(!floorQueue.isEmpty()){
        //this initial check to ensure our floor queue isn't empty is to avoid any issues with checking an empty Vector for a number.
        if(floorQueue.contains(floor)){
            //return false if floor queue contains the requested floor. If we get issues, we should remove this function as we use "removeAll" when removing a floor from our queue.
            return false;
                }
            }
        //if elevator is idle OR we are responding to a floor call and pick up our only passenger who makes a new floor our final destination floor
        if (idle == true|| floorQueue.isEmpty()){
            qInfo("Final Destination Floor updated from: %d to: %d because queue is empty",finalDestination,floor);
            finalDestination = floor;
        }

        //if elevator is moving down, and our floor is lower than our FDF update FDF
        else if(direction.compare("down")==0 && floor < finalDestination){
            qInfo("Final Destination Floor  updated from: %d to: %d",finalDestination,floor);
            finalDestination = floor;
        }
        //if elevator is moving up and floor > FDF, update FDF
        else if(direction.compare("up") ==0 && floor>finalDestination){
            qInfo("Final Destination Floor  updated from: %d to: %d",finalDestination,floor);
            finalDestination = floor;
        }
        floorQueue.append(floor);
        //return true when floor is added to floorQueue
        return true;

    }


//We want to clear our floor queue, update our elevator's alarm and idle status,
//We clear our queue so we can update it with our own preferred final destination
void elevator::activateAlarm(QString typeOfAlarm){
    this->typeOfAlarm = typeOfAlarm;
    alarm = true;
    idle = false;

    floorQueue.clear();
    //if our elevator is at a floor, we should begin to open the doors. Otherwise, continue as defined by AlarmType.
    if(status.compare("doorsClosing")==0||status.compare("doorsClosed")==0||status.compare("idle")==0){
        doorTimer=0;
        status="doorsOpening";
    }
    //special handling for power event where we want our elevator to descend to the next floor and stop there. In both cases, our finalDestination must be adjusted, but if we are moving up we must also
    //update our direction.
    //In a FIRE event, elevator should continue to next floor and let passengers off.
    else if(status.compare("moving")==0 && typeOfAlarm.compare("power")==0){
        if(direction.compare("up")==0){
            floorProgress = travelTime - floorProgress;
            finalDestination=currentFloor;
            direction="down";
        }
        else{
            finalDestination = currentFloor -1;
        }

    }
    //if it is a fire OR help event, we want to stop at the next floor.
    else if(status.compare("moving")==0 && (typeOfAlarm.compare("fire")==0||typeOfAlarm.compare("help")==0)){
        if(direction.compare("up")==0){

            finalDestination = currentFloor+1;
        }
        else{
            finalDestination = currentFloor -1;
        }
    }
    //adding a request to move to our "new" finalDestination.
    addRequest(finalDestination);
}



bool elevator::removeFloor(int floor){
    int removed = floorQueue.removeAll(floor);
    qInfo("removed floor # %d, which occured %d times in our our vector",floor,removed);
    return true;
}


//finds riders who have have requested an elevator, and are waiting at their floor. Adds those riders to our current elevator and updates their status accordingly.
//Also includes our check for the OVERLOAD status.
void elevator::checkRidersWaiting(){
    for(rider* rider: *riderQueue){
        //look at each rider. If they are not on an elevator, && starting floor is our current floor, && they are going the same direciton then add them to our riderList of this elevator.
        if(!rider->isOnElevator()){
            //if a rider is at our current floor & they are going in the same direction OR our elevator is empty and the rider has summoned us (ie. current finalDestination = riders start floor
//            if(rider->getCurrentFloor()==currentFloor && (rider->getDirection().compare(serviceDirection)==0 || (riderList.isEmpty()&& finalDestination == rider->getStartingFloor()))){
                if(rider->getCurrentFloor()==currentFloor && (rider->getDirection().compare(serviceDirection)==0)){
                if(!rider->getServiced()){
                    //add rider to riderList on Elevator, add their floor request to the queue assuming no overload..
                    addRider(rider);
                    if(riderList.size()>passengerLimit){
                        //if our riderList exceeds our passengerLimt throw Overload alarm.
                        typeOfAlarm = "OVERLOAD";
                        qInfo()<<"PASSENGER LIMIT OF "<<passengerLimit<<" EXCEEDED. RIDER MUST BE REMOVED FROM ELEVATOR";
                        return;
                    }
                    else{
                    addRequest(rider->getDestinationFloor());
                    qInfo()<<"rider added to riderList with new size: "<<riderList.size()<< " at floor: "<<currentFloor<<" and final destination: "<<rider->getDestinationFloor();
                    }
                    return;
                }
            }
        }

    }
}

void elevator::checkRidersDeparting(){
    for (rider* rider : riderList){
        if (rider->getDestinationFloor()==currentFloor){
            removeRider(rider);

        }
    }
}

void elevator::removeRiders(){
    for (rider* rider : riderList){
            removeRider(rider);

    }
}



bool elevator::removeRider(rider *rider){
    return riderList.removeOne(rider);
    rider->leaveElevator();
    rider->setFloor(currentFloor);
    rider->setServiced();
    qInfo()<<"rider departing elevator at floor: "<<currentFloor<<" who started at: "<<rider->getStartingFloor();
}


//NOTE: our addRequest function handles assigning our FDF. This function's only purpose is to get our elevator moving from an idle state. our "iterate" function is in charge of everything else.
//This is NOT responsible for stopping our movement, or altering our movement, just "getting the elevator going"

//THIS FUNCTION SHOULD ONLY BE USED TO START AN IDLE ELEVATOR AND SHOULD NOT BE USED ANYWHERE ELSE
void elevator::startMoving(int floor){
    finalDestination = floor;
    status = "moving";
    idle = false;


    if(currentFloor==finalDestination){
        status ="arrived";
    }
    else{
        status = "moving";
        if(currentFloor < finalDestination){
            direction = "up";

        }
        else {
            direction = "down";
        }

    }

}

void elevator::doorOpenPressed(){
    qInfo()<<"door open button pressed";
    if(status.compare("doorsClosing")==0){
            status="doorsOpening";
            doorTimer=0;
            return;
    }
    else if(status.compare("doorsOpen")==0){
            doorTimer=0;
            return;
    }
    //not in a doorsOpen or Closed state
    return;
}

void elevator::doorClosePressed(){
    qInfo()<<"door close button pressed";
    if(status.compare("doorsOpen")==0){
        status = "doorsClosing";
            doorTimer=0;

            return;
    }
    //not in a doorsOpen state, ignore call
    return;
}

void elevator::helpButtonPressed(){
    activateAlarm("help");

}

//toggles doorBlocked sensor when called.
void elevator::doorBlocked(){
    obstructionSensor == true ? obstructionSensor = false: obstructionSensor = true;
    if(obstructionSensor == true){
        qInfo()<<"Elevator door obstruction added!";
    }
    else{
        qInfo()<<"Elevator door obstruction Cleared!";
    }

}

//idea: use a general iterator function that changes our variables based on the "status" String to act as our elevator decision maker.
//This function will be called in our main at every "tick" of our timer.
//Originally, the idea was to have an alarm status, or different statuses for each alarm, but a decision was made to handle the majority of that explicity, and only include
// a small portion of an (alarm) check in the doorOpen status.
void elevator::iterateElevator(){

    //handles our initial scenario to start an elevator from an idle state.
     if(idle){
        if(floorQueue.isEmpty()){
            return;
        }
        else{
            startMoving(finalDestination);
            return;
        }
    }
   else if(status.compare("moving")==0){
        //if we have reached our travel time between floors, increment our current floor and reset our floorProgress meter;
        if(floorProgress>=travelTime){
            //if we are moving up, increment our current floor, else decrement it.
            direction.compare("up")==0? currentFloor ++ : currentFloor--;
            floorProgress=0;
            return;
        }
        else{
            //Do we need both of these arrived status checks?
            if(floorQueue.contains(currentFloor)){
                status = "arrived";
                removeFloor(currentFloor);
                return;
            }
            //if we are moving and we aren't at our final floor, increase our floorProgress by 1.
            floorProgress++;
            return;
        }
    }
     //This is required for if our elevator is IDLE and SUMMONED to the same floor it starts on as this will not be caught by the "moving" status check above
  else if (status.compare("arrived")==0){
        if(floorQueue.contains(currentFloor)){
           removeFloor(currentFloor);
        }
         status ="doorsOpening";

        return;
    }
  else if(status.compare("doorsOpening")==0){
        if(doorTimer>=doorOpenTime){
            status="doorsOpen";
            doorTimer=0;
            return;
        }
        else{
            doorTimer++;
            return;
        }
    }
  else if (status.compare("doorsOpen")==0){
         if(alarm){
             removeRiders();
             return;
         }

           checkRidersWaiting();
           //included so we don't check every tick.
           if(doorTimer==0){
               checkRidersDeparting();
        }

        if(doorTimer>=doorOpenInterval){
            if(typeOfAlarm=="OVERLOAD"){
                riderList.removeLast();
                typeOfAlarm = "inactive";
                qInfo()<<"Last rider removed to clear overload";
            }
            status = "doorsClosing";
            doorTimer=0;
            return;
        }
        else {
            doorTimer++;
            return;
        }
    }
    else if (status.compare("doorsClosing")==0){
         //if our door is obstructed stop closing and reset progress.
         if(obstructionSensor == true){
             doorTimer = 0;
             typeOfAlarm = "door blocked!";
            return;
         }
         if(doorTimer>=doorOpenTime){
              status = "doorsClosed";
              doorTimer=0;
              return;
          }
          else {
             //line to reset our alarm when our door block has cleared
             if(typeOfAlarm.compare("door blocked!")==0 && obstructionSensor == false){
             typeOfAlarm = "inactive";}

             //iterate as usual
              doorTimer++;
              return;
          }
      }
 else if (status.compare("doorsClosed")==0){
            if(!floorQueue.isEmpty()){
                //This specific case is to handle if somehow we have reached our FDF but somehow have an additional floor in our queue. We need to reset our state.
                if(currentFloor==finalDestination){
                    floorQueue.clear();
                    status = "idle";
                    idle=true;
                    doorTimer=0;
                    return;
                }
                //this case handles when a passenger gets on and requests to go in the opposite direction it came from.
                else if((finalDestination<currentFloor && direction.compare("up")==0) || (finalDestination>currentFloor && direction.compare("down")==0)){
                    qInfo()<<"In our special case handler for of doorsClosed";
                    startMoving(finalDestination);
                      doorTimer=0;
                }
                else{
                    //continue on where we are going!
                    status = "moving";
                    doorTimer =0;
                    return;
                }

            }
            //This could be reduced, but leave for now!
            else{
                //floorQueue is empty, set status to idle.
                status = "idle";
                idle = true;
                doorTimer=0;
                return;

            }
    }
    qInfo("case not caught by elevator Iterator");
    return;
}


