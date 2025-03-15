#ifndef ECS_H
#define ECS_H
#include <QObject>
#include "elevator.h"
#include "simEvent.h"
#include "packageIncludes.h"


class ECS: public QObject
{
    Q_OBJECT
public:
    ECS();
    ~ECS(){};
    //
    void addEvent(simEvent*);
    void addElevator(elevator* elevator){elevatorList.append(elevator);}
    void iterateECS();
    bool getAlarmStatus(){return alarm;}

private:

    bool alarm;

    //This holds our list of events, where int is our timestep and simEvent contains all of our event info
   QVector<simEvent*> eventList;

   QVector<elevator*> elevatorList;

   //if our event is a call event we can assign
   bool assignCall(simEvent*, elevator*);

   //function to review all of our call requests and see if any can be assigned
   void reviewRequests();
   void distributeCallEvents();
   void distributeAlarmEvent();
};

#endif // ECS_H
