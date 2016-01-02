/*-----------------------------------------------------------------------------
 * Class: managers.h
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 -----------------------------------------------------------------------------*/
#ifndef LAB_3_TEST_RESOURCEMANAGER_H
#define LAB_3_TEST_RESOURCEMANAGER_H
#include <string>
#include <vector>
#include "Task.h"

using namespace std;

class ResourceManager
{
public:

    // Constructor
    ResourceManager(string input_file);


private:

    //Manager Functions
    void grantRequests(int algorithm);			// Grants all possible requests for tasks in the queue

    void returnResources();					    // Return the resources in the buffer back into the system

    void abortLowestWorkingTask(int algorithm);	// Aborts the lowest numbered task that is running and releases its resources

    void finishedCycle(int algorithm);			// Any task that has finished their activity this cycle will move on to their next activity

    void resetTasks(string input_file);	        //Resets everything for the next manager to run

    void printResults();						// Print result of running all algorithms on tasks

    bool canGrantRequest();					    // Can the resource manager grant the request?

    bool inQueue(int task_num);				    // Checks if a task is in request queue or not

    bool systemIsDeadlocked(int algorithm);	    // Is the system deadlocked?

    bool isSafe(int task_index);			    // Is the current request safe to grant?

    bool completedAllTasks(int algorithm);	    // Have all the tasks been completed or aborted?

    //Resource Manager Algorithms
    void fifo();
    void bankers();

    /***************************Data Structures & Variables**********************************/

    vector<Task> TaskVector;			// Tasks in the system
    vector<int> Resources_Held;		    // Resources currently held by the system
    vector<int> ResourcesBuffer;   	    // Resources returning to the system next cycle
    vector<int> RequestsQueue; 	        // Requests queue for granting requests
    vector<int> AvailableResources;	    // Total resource amount available to the system
    int NumAlgorithms;	                // # of managers to run. In our case, it is 2.
    int NumTasks;		                // # tasks in the system
    int Resources;		                // # resources in the system
    int Time;			                // program time
    int ReqResource_Type;	            // Requested Resource Type
    int ReqResource_Amount;	            // Requested Resource Amount
};


#endif //LAB_3_TEST_RESOURCEMANAGER_H
