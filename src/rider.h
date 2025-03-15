#ifndef RIDER_H
#define RIDER_H

#include "packageIncludes.h"

//Our elevators will have access to a list of riders that they can "add". Riders could be stored in a MultiMap (multiple values with the same key) in our MainWindow class.

//When an elevator arrives at a floor it can check the "floor" (multimap using the floor as a key) to see if anyone is waiting to go the same direction on that floor.
//In production we should make a floor container class for each "floor" and intialize our riders on each floor. That way we can display (for each floor) the current riders on that floor.
//our riders are just "placed" on each floor with a destination in mind. Our intial event queue will pseudo call the elevator for each rider.

//To make our riders more realistic, we could add a timer to each rider that is used to "call" our elevators, or to "call" our events.

class rider
{
public:
    rider(int startingFLoor, int destinationFloor, QString direction);
    ~rider();
    bool isOnElevator(){return onElevator;}
    int getDestinationFloor(){return destinationFloor;}
    int getStartingFloor(){return startingFloor;}
    int getCurrentFloor(){return currentFloor;}
    bool getServiced(){return serviced;}
    void setServiced(){serviced = true;}
    void setFloor(int floor){currentFloor=floor;}



    void boardElevator(){onElevator=true;}
    void leaveElevator(){onElevator =false; serviced =true;}
    QString getDirection(){return direction;}


private:
    int startingFloor;
    int destinationFloor;
    int currentFloor;
    QString direction;
    bool onElevator;
    bool serviced;
};

#endif // RIDER_H
