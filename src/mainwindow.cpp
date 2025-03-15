#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "packageIncludes.h"


//TODO:
//add passengers and their function to select new floors, get on elevators and get off elevators.
//add multiple elevators
//special events

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    riderList = new QVector<rider*>;

    eCS = new ECS();
    QString direction;





    //addEvent adds an event to our Qmap of Events
    connect(ui->buttonAddEvent,SIGNAL(released()),this,SLOT(addEvent()));
    //startSimulation begins our timer
    connect(ui->buttonStart,SIGNAL(released()),this,SLOT(startSimulation()));
    connect(ui->buttonInitializeElevator,SIGNAL(released()),this,SLOT(initElevator()));


    //start/stop/pause/end
    connect(ui->buttonPause,SIGNAL(released()),this, SLOT(pauseSimulation()));
    connect(ui->buttonResume,SIGNAL(released()),this,SLOT(resumeSimulation()));
    connect(ui->buttonStop,SIGNAL(released()),this,SLOT(stopSimulation()));




}

MainWindow::~MainWindow()
{
    delete ui;
}

//next steps! set up a simple timer and UI that will let us test our elevator logic by manually providing some functionality to test our "iterateElevator" function.

//After confirming our elevator logic works, add our ECS and THEN our event class.


//this was used to do our intial testing of our Elevator floor queue.
void MainWindow::addFloor(){
    elevatorList[1]->addRequest(ui->numberFloorCall->value());
}


void MainWindow::iterateSimulation(){

    //check our eventList for an event with the current timeStep.
    //if found, distribute to ECS (or otherwise) as required.
    if(checkEvents()){
      handleEvent(eventList.take(timeStep));
    }
    //ecs handles distributing call events to elevators
    eCS->iterateECS();
    //iterate through elevator list to allow them to iterate their current step. (move floors, add passenger, etc.)
    for(elevator* elevator : elevatorList){
        elevator->iterateElevator();
    }
    timeStep++;
    updateDisplay();
}


void MainWindow::initializeTimer(QTimer* t) {


    //Timers have a "timeout" signal, which will trigger until the timer is stopped.
    //In this case, processEvent is called as our SLOT and timeout is our SIGNAL.
    //Our timer will output a signal call every second, which moves our simulation forward one step every second! Wow!
    connect(t, &QTimer::timeout, this, &MainWindow::iterateSimulation);

        t->start(1000);

}


void MainWindow::updateDisplay(){

    ui->displayCurrentFloor->display(elevatorList[0]->getFloor());
    ui->displayDestinationFloor->display(elevatorList[0]->getFinalDestination());
    ui->displayIdle->setText(elevatorList[0]->isIdle()?"idle":"notIdle");
    ui->displayStatus->setText(elevatorList[0]->getStatus());
    ui->displayDirection->setText(elevatorList[0]->getDirection());
    ui->displayPassengers->display(elevatorList[0]->getNumPassengers());
    ui->displayAlarm->setText(elevatorList[0]->getAlarmType());


    if(elevatorList.size()>1){
    ui->displayCurrentFloor_2->display(elevatorList[1]->getFloor());
    ui->displayDestinationFloor_2->display(elevatorList[1]->getFinalDestination());
    ui->displayIdle_2->setText(elevatorList[1]->isIdle()?"idle":"notIdle");
    ui->displayStatus_2->setText(elevatorList[1]->getStatus());
    ui->displayDirection_2->setText(elevatorList[1]->getDirection());
    ui->displayPassengers_2->display(elevatorList[1]->getNumPassengers());
    ui->displayAlarm_2->setText(elevatorList[1]->getAlarmType());
    }

    if(elevatorList.size()>2){
    ui->displayCurrentFloor_3->display(elevatorList[2]->getFloor());
    ui->displayDestinationFloor_3->display(elevatorList[2]->getFinalDestination());
    ui->displayIdle_3->setText(elevatorList[2]->isIdle()?"idle":"notIdle");
    ui->displayStatus_3->setText(elevatorList[2]->getStatus());
    ui->displayDirection_3->setText(elevatorList[2]->getDirection());
    ui->displayPassengers_3->display(elevatorList[2]->getNumPassengers());
    ui->displayAlarm_3->setText(elevatorList[2]->getAlarmType());
    }

    ui->displayTimeStep->display(timeStep);
}


//Slots

void MainWindow::startSimulation(){
    if(elevatorList.size()==0){
        qInfo()<<"Can not start simulation without adding atleast 1 elevator";
    }
    else{
        timer = new QTimer(this);
        initializeTimer(timer);
    }
}




void MainWindow::addEvent(){
    QList<QString> eventTypes;
    eventTypes << "call"<<"alarm";

    int time = ui->numberEventTime->value();
    //each tab corresponds to an event type
    QString type = eventTypes[ui->eventTabs->currentIndex()];
    //index 0 is a call event, 1 is an emergency event


    qInfo()<<"trying to add event with time: "<<time<<" and type: "<< type<<" where our index of our tab is: "<<ui->eventTabs->currentIndex();

    if(type.compare("call")==0){
     initCallEvent(time);
    }
    else if(type.compare("alarm")==0){
        initAlarmEvent(time);
    }


}

bool MainWindow::checkEvents(){

   if(eventList.contains(timeStep)){
       qInfo()<<"found event at timestep: "<<timeStep;
       return true;
   }
   return false;


}

void MainWindow::handleEvent(simEvent* newEvent){
    qInfo()<<"handling event of type: "<<newEvent->getType();
    QString type = newEvent->getType();
    eCS->addEvent(newEvent);
//    if(type.compare("call")==0){
//        qInfo()<<"adding event to eCS";
//        eCS->addEvent(newEvent);
//    }

}

//creates a "call" event, adds it to eventList. For all call events, we also create one rider.
void MainWindow::initCallEvent(int timeStep){

    int floorCall = ui->numberFloorCall->value();
    int destinationFloor = ui->numberDestinationFloor->value();
    QString direction;

  if(ui->menuDirection->currentIndex()==0){
      direction ="up";
  }
  else{
      direction = "down";
  }
    qInfo()<<"added Call event with time: "<<timeStep<<" type: "<<"call"<<"floor: "<<floorCall<<" and direction: "<<direction;
    initRider(floorCall,destinationFloor,direction);
    qInfo()<<"added rider with startingFloor: "<<floorCall<<" destinationFloor: "<<destinationFloor<<" and direction: "<<direction;

    eventList.insert(timeStep,new simEvent("call",timeStep,floorCall,direction));
}


void MainWindow::initAlarmEvent(int time){
//    simEvent(QString typeOfEvent, int time, bool alarm)
    QString type;
    switch(ui->menuAlarmType->currentIndex()){
    case 0:
        type = "fire";
        break;
    case 1:
        type = "power";
        break;
    }
    eventList.insert(time,new simEvent(type,time,true));
    qInfo()<<"Added ALARM event of type: "<<type<<" at timestep: "<<time;



}

void MainWindow::initRider(int startingFloor, int destinationFloor, QString direction){
    riderList->append(new rider(startingFloor,destinationFloor,direction));
}


void MainWindow::initElevator(){
    if(elevatorList.size()>MAX_NUM_ELEVATORS){
        qInfo("already reached maximum number of elevators allowed by simulator. Can not add elevator");
        return;
    }
    else{
        elevatorList.append(new elevator(ui->buttonElevatorStartFloor->value(),riderList->size(),riderList,ui->buttonElevatorPassengerLimit->value()));
         eCS->addElevator(elevatorList[elevatorList.size()-1]);
        qInfo("added elevator elevatorList.");
    }
    switch(elevatorList.size()){
    case 1:
        ui->checkBoxElevatorActive->setChecked(true);
        connect(ui->buttonDoorOpen,SIGNAL(released()),this->elevatorList[0], SLOT(doorOpenPressed()));
        connect(ui->buttonDoorClose,SIGNAL(released()),this->elevatorList[0],SLOT(doorClosePressed()));
        connect(ui->buttonHelp,SIGNAL(released()),this->elevatorList[0],SLOT(helpButtonPressed()));
        connect(ui->buttonDoorBlocked,SIGNAL(released()),this->elevatorList[0],SLOT(doorBlocked()));
        break;
    case 2:
        ui->checkBoxElevatorActive_2->setChecked(true);
        connect(ui->buttonDoorOpen__2,SIGNAL(released()),this->elevatorList[1], SLOT(doorOpenPressed()));
        connect(ui->buttonDoorClose__2,SIGNAL(released()),this->elevatorList[1],SLOT(doorClosePressed()));
        connect(ui->buttonHelp__2,SIGNAL(released()),this->elevatorList[1],SLOT(helpButtonPressed()));
        connect(ui->buttonDoorBlocked_2,SIGNAL(released()),this->elevatorList[1],SLOT(doorBlocked()));
        break;
    case 3:
        ui->checkBoxElevatorActive_3->setChecked(true);
        connect(ui->buttonDoorOpen__3,SIGNAL(released()),this->elevatorList[2], SLOT(doorOpenPressed()));
        connect(ui->buttonDoorClose__3,SIGNAL(released()),this->elevatorList[2],SLOT(doorClosePressed()));
        connect(ui->buttonHelp__3,SIGNAL(released()),this->elevatorList[2],SLOT(helpButtonPressed()));
        connect(ui->buttonDoorBlocked_3,SIGNAL(released()),this->elevatorList[2],SLOT(doorBlocked()));
    }
    ui->displayElevatorNumber->display(elevatorList.size());
}


//Pause, Resume, Stop buttons

void MainWindow::pauseSimulation(){
    if(timer!=nullptr){
        if(timer->isActive()){
            timer->stop();
            return;
        }
    }
}

void MainWindow::resumeSimulation(){
    if(timer!=nullptr){
        if(!timer->isActive()){
            timer->start(1000);
            return;
        }
    }
}


void MainWindow::stopSimulation(){
   for(simEvent* events: eventList){
       delete events;
   }
   delete eCS;
   for(elevator* elevators: elevatorList){
       delete elevators;
   }
   for(rider* riders: *riderList){
       delete riders;
   }
   delete riderList;


   timer->stop();
   timer->disconnect();
   delete timer;
   QCoreApplication::quit();


}
