#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "packageIncludes.h"
#include <simEvent.h>
#include <elevator.h>
#include <rider.h>
#include <ecs.h>
#include <floor.h>
#define MAX_NUM_ELEVATORS 2


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<rider*>* getRiderList(){return riderList;}




protected:

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    int timeStep=0;
    ECS* eCS;


    int newFloor=0;

    //main eventList
    QMap<int,simEvent*> eventList;
    QVector<elevator*> elevatorList;
    QVector<rider*>* riderList;
//    QMultiMap<int,rider*> riderList;



    void iterateSimulation();
    void updateDisplay();
    void floorSelection(int);
    void initializeTimer(QTimer* t);

    void handleEvent(simEvent* newEvent);
    void iterateEvents();
    bool checkEvents();

    void initCallEvent(int timeStep);
    void initAlarmEvent(int time);
    void initRider(int startingFloor, int destinationFloor, QString direction);



private slots:
    void addEvent();
    void addFloor();
    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();

    //this should really be divided into an initElevator to create our elevator object, and an addElevator that handles gathering our "information"
    void initElevator();


    void stopSimulation();

signals:




};
#endif // MAINWINDOW_H
