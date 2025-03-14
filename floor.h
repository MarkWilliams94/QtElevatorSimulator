#ifndef FLOOR_H
#define FLOOR_H

#include <packageIncludes.h>
#include <rider.h>


//our floors hold our riders. This is essentially a container class for our riders to interface with our elevators. But, it does not solve our problem that our current program architecture doesn't
//allow us to access our list of floors OR our list of passengers easily.
class floor
{
public:
    floor();


private:


    QVector<rider*> riderList;

};

#endif // FLOOR_H
