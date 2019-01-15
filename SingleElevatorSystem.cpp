#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <string>
#include <unistd.h>

using namespace std;
enum Direction
{
    UP=0,
    DOWN
};
enum DOOR
{
    CLOSED=0,
    OPEN
};
enum State
{
    MOVING=0,
    STOPED
};


class Request
{
public:
    Request(int t,int f,Direction d):
        time(t),
        floor(f),
        director(d)
        {}

    int time;
    int floor;
    Direction director;
};

//class ComparatorUP
//{
//    public:
//        int operator()( Request a, Request b)
//        {
//            return a.floor > b.floor;
//        }
//};
//class ComparatorDown
//{
//    public:
//        int operator()(Request a,Request b)
//        {
//            return a.floor < b.floor;
//        }
//};

bool ComparatorUP(Request a, Request b)
{
    return a.floor > b.floor;
}
bool ComparatorDown(Request a, Request b)
{
    return a.floor > b.floor;
}
class Elevator
{
private:
    // priority_queue<Request,vector<Request>,ComparatorUP> upQ;
    //priority_queue<Request,vector<Request>,ComparatorDown> downQ;
    //queue<Request> currentQ;

    vector<Request> upQ;
    vector<Request> downQ;
    vector<Request> currentQ;

    Direction direction=Direction::UP;
    int floor=0;
    State state=State::STOPED;
    DOOR door = DOOR::CLOSED;

    thread processThread;
    bool run=false;

public:
    void start()
    {
        if(!run)
        {
            run=true;
            processThread=thread(&Elevator::process,this);

        }
    }
    Elevator(){}
    ~Elevator()
    {
        cout<<"~Elevator"<<endl;
        processThread.join();
    }
    void emergencyStop()
    {
        cout<<"emergencyStop"<<endl;
        run=false;
        upQ.clear();
        downQ.clear();
        currentQ.clear();
    }

    void call(int flr,Direction d) // from floor
    {
        if(direction == Direction::UP)
        {
            SYSTEMTIME a;
            GetLocalTime(&a);
            int time=a.wHour*10000+a.wMinute*100+a.wSecond;
            if(flr >= floor)
            {

                currentQ.push_back(Request(time,flr,d));
            }
            else
            {
                upQ.push_back(Request(time,flr,d));
                sort(upQ.begin(),upQ.end(),ComparatorUP);
                sort(currentQ.begin(),currentQ.end(),ComparatorUP);
            }
        }
        else
        {
            SYSTEMTIME a;
            GetLocalTime(&a);
            int time=(a.wHour*10000+a.wMinute*100+a.wSecond);
            if(flr <= floor)
            {
                currentQ.push_back(Request(time,flr,d));
                sort(currentQ.begin(),currentQ.end(),ComparatorDown);
            }
            else
            {
                downQ.push_back(Request(time,flr,d));
                sort(downQ.begin(),downQ.end(),ComparatorDown);
            }
        }
    }

    void goTo(int flr) // inside elevatar
    {
        call(flr,direction);
    }

    int getLowestTimeUpQ()
    {
        int min=INT_MAX;
        for(int i=0;i<upQ.size();i++)
        {
            if(upQ[i].time<min)
                min=upQ[i].time;
        }
        return min;
    }

    int getLowestTimeDownQ()
    {
        int min=INT_MAX;
        for(int i=0;i<downQ.size();i++)
        {
            if(downQ[i].time<min)
                min=downQ[i].time;
        }
        return min;
    }

    void goToFloor(int flr) // elevator moving logic here
    {
        cout<<"goToFloor"<<endl;

        cout<<"START FLOOR "<<floor<<endl;
        door=DOOR::CLOSED;
        cout<<"DOOR::CLOSED"<<endl;
        state=State::MOVING;
        cout<<"State::MOVING"<<endl;
        usleep(5000);
        floor=flr;
        cout<<"REACHED "<<floor<<endl;
        cout<<"State::STOPED"<<endl;
        state=State::STOPED;
        door=DOOR::OPEN;
        cout<<"DOOR::OPEN"<<endl;
        usleep(2000);
        cout<<"DOOR::CLOSED Again"<<endl;
        door=DOOR::CLOSED;
    }

    void processNextQueue()
    {
        if(getLowestTimeDownQ() < getLowestTimeUpQ())
        {
            currentQ=move(upQ);
        }
        else
        {
            currentQ=move(downQ);
        }
    }

    void process()
    {
        while(run)
        {
            cout<<"process::run"<<endl;
            if(!upQ.empty() && !downQ.empty())
            {
                cout<<"process::run::if"<<endl;
                Request r=currentQ.front();
                currentQ.erase(currentQ.begin());
                goToFloor(r.floor);
            }
            else if(!currentQ.empty())
            {
                cout<<"process::run::else if"<<endl;
                Request r=currentQ.front();
                currentQ.erase(currentQ.begin());
                goToFloor(r.floor);
            }
            else
            {
                cout<<"process::run::else"<<endl;
                processNextQueue();
            }
            getchar();
        }
        currentQ.clear();
        upQ.clear();
        downQ.clear();
    }
};

int main()
{

    Elevator e;
    e.start();
    e.call(1,Direction::UP);

    //e.goTo(4);
    //e.call(3,Direction::DOWN);
    //e.goTo(0);


    //e.call(5,Direction::UP);
    //e.goTo(0);
}
