#ifndef ELEVATOR_H
#define ELEVATOR_H
#include <QObject>
#include "packageIncludes.h"
#include <rider.h>

#define travelTime 5
#define doorOpenTime 2
#define doorCloseTime 2
#define doorOpenInterval 5

class elevator: public QObject{

    Q_OBJECT

public:

    elevator(int startFloor, int name, QVector<rider*>* riderQueue, int passengerLimit);
    ~elevator();

    bool addRequest(int floor);
    //used to set our service direction for our intial calls
    bool isIdle(){return idle;};
    bool handleSpecialEvent();
    void iterateElevator();

    //This riderList is the riderList from our mainWindow
    void checkRidersWaiting();
    void checkRidersDeparting();


    //addRider to elevator riderList, update their status to boarded.
    void addRider(rider* rider){riderList.append(rider); rider->boardElevator();}
    //remove rider from riderList,
    bool removeRider(rider* rider);

    void activateAlarm(QString typeOfAlarm);

    //used by ECS when assigning calls to an idle elvator, allows us an independent check aside from the current direction the elevator is moving.
    void setServiceDirection(QString direction){serviceDirection = direction;}




    int getFloor(){return currentFloor;};
    int getNumPassengers(){return riderList.size();};
    int getFinalDestination(){return finalDestination;};
    QString getStatus(){return status;};
    QString getDirection(){return direction;};
    bool getAlarm(){return alarm;}
    QString getAlarmType(){return typeOfAlarm;}



private:

    bool idle;
    //arrived, moving, doorsOpen, doorsClosing, idle
    QString status;
    QVector <int> floorQueue;
    //local riders on our elevator
    QVector<rider*> riderList;

    //This holds a pointer to the vector containing all our riders in the building. for SURE this is bad practice.
    QVector<rider*>* riderQueue;
    QString direction;
    //used as an additional variable to determine which way our elvator is servicing floors, which is separate from our regular "direciton" which indicates movement.
    QString serviceDirection;
    bool removeFloor(int floor);
    void startMoving(int floor);
    //used in case of an alarm.
    void removeRiders();

    int currentFloor;
    int finalDestination;

    //idea: use a floorProgress indicator to keep track of how close we are to the next floor. We will count from 0->5, and on 5 we will reach the next floor.
    int floorProgress;
    int doorTimer;
    //used primarially during testing. Not a core component of uor class and could be removed.
    int name;

    //passengerLimit used to set the maximum number of passengers before overload
    int passengerLimit;
    //used in conjunction with passengerLimit to determine overload scenario. Iterated by 1, and decremented by 1 when passengers board/leave.
    int numRiders;

    //elemts used to update our elevators in case of an alarm (and appropriately display information)
    bool alarm;
    QString typeOfAlarm;

    //doorOpen/Closed/blocked button states
    bool doorOpenButton;
    bool doorCloseButton;
    bool helpButton;
    bool obstructionSensor;


public slots:
    void doorOpenPressed();
    void doorClosePressed();
    void helpButtonPressed();
    //toggles value of obstructionSensor when called
    void doorBlocked();

signals:





};


#endif // ELEVATOR_H


