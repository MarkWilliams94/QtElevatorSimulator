#include "rider.h"

rider::rider(int startingFLoor, int destinationFloor, QString direction): startingFloor(startingFLoor), destinationFloor(destinationFloor), direction(direction)
{
onElevator = false;
serviced = false;
currentFloor = startingFloor;
}

rider::~rider(){}
