/*-----------------------------------------------------------------------------
 * Class: tasks.cpp
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#include "Task.h"
#include "Verbose.h"

using namespace std;

verbose verbose_method;
Task::Task(int Total_Algorithms) {

    // Initialize values
    ClaimsVector.push_back(0);
    HoldsVector.push_back(0);

    Current_Activity = "unused";
    Resource_Type = 0;
    Resource_Amt = 0;
    RequestPending = false;
    Terminating = false;
    ActivityCounter = 0;
    CurrentAlgorithm = 0;
    Compute_Timer = 0;
    Total_Time = new int[Total_Algorithms];
    WaitingTime = new int[Total_Algorithms];
    WasAborted = new bool[Total_Algorithms];
    WasTerminated = new bool[Total_Algorithms];
    IsComputing = new bool [Total_Algorithms];

    // Initialize all arrays to 0
    for (int i = 0; i < Total_Algorithms; i++) {
        Total_Time[i] = 0;
        WaitingTime[i] = 0;
        WasAborted[i] = false;
        WasTerminated[i] = false;
    }

}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Process 1 cycle and handle the current activity. If the task is not terminated or aborted
 * then get the activity. The activity is then handled with conditionals. If initiate: add claim to a list.
 *If request: set that the task is requesting resources. If release: Release resource
 *At the end it checks if the next activity is terminate and sets the activity to terminate at END of current activity.
 ---------------------------------------------------------------------------------------------------------------------*/
void Task::process() {

    // Local constants and variables
    const string Initiate  = "initiate";
    const string Request   = "request";
    const string Release   = "release";
    const string Compute   = "compute";
    const string Terminate = "terminate";
    Activity currentActivity;
    Activity *NextActivity;         //Looks at next activity


    // IF task is not terminated and not aborted
    if (!(WasTerminated[CurrentAlgorithm] || WasAborted[CurrentAlgorithm])) {
        // Get Activity
        currentActivity = ActivitiesVector[ActivityCounter];
        Current_Activity = currentActivity.Activity;                          // Set activity of task for this cycle
        Resource_Type = currentActivity.Resource_Type;               // Set resource type being used
        Resource_Amt = currentActivity.Resource_Amount;              // Set resource amount being used

        if (Current_Activity == Initiate) {
            //Add the claim to the task's claims list
            ClaimsVector.push_back(Resource_Amt);
            HoldsVector.push_back(0);
            verbose_method.log("        Task activity is Initiate");
        }

        else if (Current_Activity == Request) {
            // Set the task as requesting resources
            verbose_method.log("        Task activity is Request");
            RequestPending = true;
        }

        else if (Current_Activity == Compute) {
            verbose_method.log("        Task activity is Compute");
            IsComputing = true;
        }

        // If task is not terminating
        if (!Terminating) {
            NextActivity = &ActivitiesVector[ActivityCounter + 1];

            // Check if next activity is to terminate
            if (NextActivity->Activity == Terminate) {
                // Set the task to terminate after it completes its current activity
                Terminating = true;
            }
        }
    }
}

//****************************Setters****************************

void Task::setTotal_Time(int algorithm, int time) {
    // Set time taken for specified algorithm
    Total_Time[algorithm] = time;
}

void Task::setTimeWaiting(int algorithm, int time) {
    // Set time waiting for specified algorithm
    WaitingTime[algorithm] = time;
}

void Task::setWasAborted(int algorithm, bool aborted) {
    // Set Current_Activity of task's abortion for specified algorithm
    WasAborted[algorithm] = aborted;
}

void Task::setWasTerminated(int algorithm, bool terminated) {
    // Set Current_Activity of task's termination for specified algorithm
    WasTerminated[algorithm] = terminated;
}

void Task::setCurrent_Activity(string Current_Activity) {
    // Set Current_Activity of task's current activity
    Current_Activity = Current_Activity;
}

void  Task::setResourceType(int resource_type) {
    // Set resource type
    Resource_Type = resource_type;
}

void  Task::setResourceAmt(int resource_amt) {
    // Set resource amount
    Resource_Amt = resource_amt;
}

void  Task::setCurrentAlgorithm(int algorithm) {
    // Set current algorithm
    CurrentAlgorithm = algorithm;
}

void Task::setRequestPending(bool pending) {
    // Set request pending Current_Activity
    RequestPending = pending;
}

void Task::setTerminating(bool terminating) {
    // Set task's terminating Current_Activity
    Terminating = terminating;
}

void Task::setIsComputing(int algorithm, bool comp) {
    //Set task's compute Current_Activity
    IsComputing = comp;
}

void Task::setCompute_Timer(int algorithm, int time) {
    Compute_Timer = time;
}

void Task::resetActivityCounter() {
    // Reset activity counter
    ActivityCounter = 0;
}

void Task::nextActivity() {
    // Point to next activity
    ActivityCounter++;
}

//****************************Getters****************************
Activity Task::getCurrentActivity() {
    return ActivitiesVector[ActivityCounter];
}

//Get Total Time a task has taken to FINISH using a manager
int Task::getTotal_Time(int algorithm) {
    return Total_Time[algorithm];
}

//Get task waiting time of a specific manager
int  Task::getTimeWaiting(int algorithm) {
    return WaitingTime[algorithm];
}

//Get aborted boolean
bool  Task::getWasAborted(int algorithm) {
    return WasAborted[algorithm];
}

//Get terminated boolean
bool  Task::getWasTerminated(int algorithm) {
    return WasTerminated[algorithm];
}

//Get is request pending boolean
bool Task::getRequestPending() {
    return RequestPending;
}

//Get is terminating or not boolean
bool Task::getTerminating() {
    return Terminating;
}

bool Task::getIsComputing(int algorithm) {
    return IsComputing;
}

int Task::getCompute_Timer(int algorithm) {
    return Compute_Timer;
}

