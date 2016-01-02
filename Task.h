/*-----------------------------------------------------------------------------
 * Class: tasks.h
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#ifndef LAB_3_TEST_TASK_H
#define LAB_3_TEST_TASK_H

#include <string>
#include <vector>

using namespace std;

/*-----------------------------------------------------------------------------
 * Description:
 * A struct that can contain 1 of 4 activities which are Initiate,
 * Request, Release and Terminate.
 *
 * That activity will have a type, resource type and resource amount.
 -----------------------------------------------------------------------------*/
struct Activity {
    string Activity;            // Initiate, request, release, or terminate.
    int Resource_Type;          // Resource type
    int Resource_Amount;        // Resource amount requested or released
};

/*-----------------------------------------------------------------------------
 * Description:
 * A task can run one of three activities per cycle and Termination does not
 * utilize a cycle. A task takes in the number of algorithms. In this lab
 * there are two, Banker's and FIFO algorithms. Below contains data structures
 * and variables to be used in the program.
 -----------------------------------------------------------------------------*/
class Task {
public:

    Task(int num_of_algorithms);    // Constructor

    void process();                 // Process the current activity

    //------------------------------Getters------------------------------
    Activity getCurrentActivity();

    int getTotal_Time(int algorithm);

    int getTimeWaiting(int algorithm);

    bool getWasAborted(int algorithm);

    bool getWasTerminated(int algorithm);

    bool getIsComputing(int algorithm);

    int getCompute_Timer(int algorithm);

    bool getRequestPending();

    bool getTerminating();

    //------------------------------Setters------------------------------
    void setCurrent_Activity(string status);

    void setResourceType(int resource_type);

    void setResourceAmt(int resource_amt);

    void setCurrentAlgorithm(int algorithm);

    void setRequestPending(bool pending);

    void setTerminating(bool terminating);

    void setTotal_Time(int algorithm, int time);

    void setTimeWaiting(int algorithm, int time);

    void setWasAborted(int algorithm, bool aborted);

    void setWasTerminated(int algorithm, bool terminated);

    void setIsComputing(int algorithm, bool comp);

    void setCompute_Timer(int algorithm, int time);

    void resetActivityCounter();

    void nextActivity();

    vector<int> ClaimsVector;                 // Resources Claims
    vector<int> HoldsVector;                  // Resources held
    vector<Activity> ActivitiesVector;        // Activities the task must process

private:

    /*************************** Data Members **********************************/
    string Current_Activity;  // Current activity being process
    int Resource_Type;        // The resource type requested or released
    int Resource_Amt;         // The amount of resources being requested or release
    int CurrentAlgorithm;     // Current algorithm task is being run on
    int ActivityCounter;      // The index of the current activity in the cycle
    int *Total_Time;          // Time taken to complete task
    int *WaitingTime;         // Time task spent waiting on resources
    bool *WasAborted;         // Was the task aborted?
    bool *WasTerminated;      // Did the task run until termination?
    int Compute_Timer;        // Counter to indicate compute time
    bool RequestPending;      // Is there a resource request pending?
    bool Terminating;         // Is the task terminating after current cycle?
    bool IsComputing;         // Is the task still computing?
};

#endif //LAB_3_TEST_TASK_H
