/*----------------------------------------------------------------------------------------------------------------------
 * Class: managers.cpp
 * Author: Ricardo N. Guntur <ricardo.guntur@nyu.edu>
 * Course: Operating Systems -- Professor Allan Gottlieb
 ---------------------------------------------------------------------------------------------------------------------*/
#include "ResourceManager.h"
#include "Task.h"
#include "Verbose.h"
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;
verbose verbose_methods;
/*----------------------------------------------------------------------------------------------------------------------
 * Description - Constructor:
 * Creates a manager that will run input tasks -- Read input, set up tasks
 ---------------------------------------------------------------------------------------------------------------------*/
ResourceManager::ResourceManager(string input_file)
{
    //Local constants/variables
    Task* task;				            // empty task
    Activity activity;		            // activity to add to tasks
    ifstream input_stream;
    const int Total_Algorithms = 2;
    int tempData;			            // stores temporary stuff
    int taskNum;			            // References tasks

    //Initial values of manager
    Time = 1;			                // Time
    ReqResource_Type = 0;	            // Requested Resource Type
    ReqResource_Amount = 0;	                // Requested Resource Amount

    NumAlgorithms = Total_Algorithms;	// Set Number of Algorithms. 2 in our case

    //-------Test if input file can be opened------/
    input_stream.open(input_file.c_str());
    if (!input_stream.is_open()) {
        cout << "Input file could not be opened." << endl;
    }

    else {
        input_stream.close();
        input_stream.open(input_file.c_str());    // Open file

        // Get data for resource manager
        input_stream >> NumTasks;             //# of tasks
        input_stream >> Resources;            //# of resource types


        task = new Task(Total_Algorithms);
        Resources_Held.push_back(0);
        AvailableResources.push_back(0);
                ResourcesBuffer.push_back(0);
                TaskVector.push_back(*task);

                // FOR every task in the system
                for (int i = 0; i < NumTasks; i++) {
                    // Create a new task
                    task = new Task(Total_Algorithms);

                    // Add a task to the resource manager
                    TaskVector.push_back(*task);
                }

                // FOR every resource type
                for (int i = 0; i < Resources; i++) {
                    // Get and store amount of resources per resource type
                    input_stream >> tempData;
                    Resources_Held.push_back(tempData);
                    AvailableResources.push_back(tempData);
                    ResourcesBuffer.push_back(0);            // Create space in buffer for resource type
                }

                // WHILE not end of file
                while (!input_stream.eof()) {
                    // Get activity input data
                    input_stream >> activity.Activity;            //Activity type
                    input_stream >> taskNum;                    //Task being referenced
                    input_stream >> activity.Resource_Type;            //Resource type being referenced
                    input_stream >> activity.Resource_Amount;    //Amount of resources being requested or released

                    // Assign activity to corresponding task
                    TaskVector[taskNum].ActivitiesVector.push_back(activity);
                }

                input_stream.close();


                fifo();
                resetTasks(input_file);
                bankers();
                printResults();
            }
        }


/*----------------------------------------------------------------------------------------------------------------------
 * Description: Optimistic or FIFO Resource Manager. Simulates the FIFO manager. Does not use initial claims.
 * If a deadlock occurs, it aborts the lowest numbered task and releases all of its resources. This is printed.
 ---------------------------------------------------------------------------------------------------------------------*/
        void ResourceManager::fifo()
        {
            verbose_methods.log("Start FIFO-------------------------------");
            // Local constants
            const int FIFO_Reference = 0;
            //initiate is unused in FIFO
            const string Request = "request";
            const string Release = "release";
            const string Compute = "compute";
            const string Terminate = "terminate";

            // Local variables
            Activity activity;
            bool grantableRequest = false;	//Checks if a request can be granted after a task is aborted. (while deadlocked)


            //Loop until all tasks are completed
            while (!completedAllTasks(FIFO_Reference))
            {
                verbose_methods.log("Timer: ", Time);
                while (systemIsDeadlocked(FIFO_Reference) && !grantableRequest)
                {
                    //Loops through every task
                    for (int i = 1; i <= NumTasks; i++)
                    {
                        //If not aborted or terminated
                        if (!TaskVector[i].getWasAborted(FIFO_Reference) &&
                            !TaskVector[i].getWasTerminated(FIFO_Reference))
                        {
                            // Get request type and amount
                            ReqResource_Type = TaskVector[i].getCurrentActivity().Resource_Type;
                            ReqResource_Amount = TaskVector[i].getCurrentActivity().Resource_Amount;

                            // IF system can grant request
                            if (canGrantRequest())
                            {
                                // There is a grantable request
                                grantableRequest = true;
                            }
                        }
                    }

                    // IF there is no grantable request
                    if (!grantableRequest)
                    {
                        abortLowestWorkingTask(FIFO_Reference);
                        returnResources();
                    }
                }
                grantableRequest = false;

                // FOR every task
                for (int i = 1; i <= NumTasks; i++)
                {
                    // IF task is not aborted
                    if (!TaskVector[i].getWasAborted(FIFO_Reference))
                    {
                        // Get current activity
                        activity = TaskVector[i].getCurrentActivity();

                        // Set task's current activity
                        ReqResource_Type = activity.Resource_Type;
                        ReqResource_Amount = activity.Resource_Amount;

                        // Run task
                        verbose_methods.log("        Process Task: ", i);
                        TaskVector[i].process();

                        // IF activity is request and is not already in queue
                        if (activity.Activity == Request && !inQueue(i))
                        {
                            // Add the requests to the requests queue
                            RequestsQueue.push_back(i);
                        }

                            // ELSE IF activity is release
                        else if (activity.Activity == Release)
                        {
                            // Add resources back to system
                            TaskVector[i].HoldsVector[ReqResource_Type] -= ReqResource_Amount;	// Release resources from task
                            ResourcesBuffer[ReqResource_Type] += ReqResource_Amount;			// Add back to system through buffer
                        }
                    }
                }

                // Grant all possible requests for tasks in the queue
                grantRequests(FIFO_Reference);

                // WHILE system is deadlocked and has no new grantable request
                while (systemIsDeadlocked(FIFO_Reference) && !grantableRequest)
                {
                    // FOR every task
                    for (int i = 1; i <= NumTasks; i++)
                    {
                        // IF task is not aborted and not terminated
                        if (!TaskVector[i].getWasAborted(FIFO_Reference) && !TaskVector[i].getWasTerminated(FIFO_Reference))
                        {
                            // Get request type and mount
                            ReqResource_Type = TaskVector[i].getCurrentActivity().Resource_Type;
                            ReqResource_Amount = TaskVector[i].getCurrentActivity().Resource_Amount;

                            // IF system can grant request
                            if (canGrantRequest())
                            {
                                // There is a grantable request
                                grantableRequest = true;
                            }
                        }
                    }

                    // IF there is no grantable request
                    if (!grantableRequest)
                    {
                        abortLowestWorkingTask(FIFO_Reference);
                        returnResources();
                    }
                }

                finishedCycle(FIFO_Reference);
                Time++;
                returnResources();
            }
            verbose_methods.log("FIFO Finished----------------------------\n");
        }

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Bankers uses initial claims. If a claim is more than the current amount of resources
 * then the task gets aborted. Can't get deadlocked!
 * Loops until all tasks are completed.
 ---------------------------------------------------------------------------------------------------------------------*/
        void ResourceManager::bankers()
        {
            verbose_methods.log("Bankers Start----------------------------");
            // Local constants/variables
            const int Banker_Reference = 1;
            const string Initiate  = "initiate";
            const string Request   = "request";
            const string Release   = "release";
            const string Compute = "compute";
            const string Terminate = "terminate";
            Activity activity;	// Current activity being processed
            Time = 1;		// Time / clock cycle


            // WHILE all tasks are not completed
            while (!completedAllTasks(Banker_Reference))
            {
                verbose_methods.log("Timer: ", Time);

                // FOR every task in the system
                for (int i = 1; i <= NumTasks; i++)
                {
                    // IF task is not aborted or terminated
                    if (!TaskVector[i].getWasAborted(Banker_Reference) && !TaskVector[i].getWasTerminated(Banker_Reference))
                    {
                        // Get current activity
                        activity = TaskVector[i].getCurrentActivity();

                        // Set task's current activity
                        ReqResource_Type = activity.Resource_Type;
                        ReqResource_Amount = activity.Resource_Amount;

                        // Run task
                        verbose_methods.log("        Process Task: ", i);
                        TaskVector[i].process();

                        // IF activity is initiate
                if (activity.Activity == Initiate)
                {
                    // IF claims exceed resource available
                    if (ReqResource_Amount > AvailableResources[ReqResource_Type])
                    {
                        // Print abortion message
                        cout << " Banker aborts task " << i << " before run begins: "
                        << "claim for resource " << ReqResource_Type
                        << " (" << ReqResource_Amount << ") exceeds number of units present ("
                        << AvailableResources[ReqResource_Type] << ")\n";

                        // Set aborted
                        TaskVector[i].setWasAborted(Banker_Reference, true);
                        TaskVector[i].setCurrent_Activity("aborted");

                        // Abort the task and release the resources held
                        // FOR every resource type
                        for (int j = 1; j <= Resources; j++)
                        {
                            // Release resource type, available next cycle
                            ResourcesBuffer[j] += TaskVector[i].HoldsVector[j];
                            TaskVector[i].HoldsVector[j] = 0;
                        }
                    }
                }

                    // ELSE IF activity is request and is not already in queue
                else if (activity.Activity == Request && !inQueue(i))
                {
                    // IF request exceeds claims
                    if (ReqResource_Amount + TaskVector[i].HoldsVector[ReqResource_Type]
                        > TaskVector[i].ClaimsVector[ReqResource_Type])
                    {
                        // Abortion message
                        cout << "During cycle " << Time - 1 << " - " << Time
                        << " of Banker's algorithms\n  Task "
                        << i << "'s request exceeds its claim; aborted; ";

                        // Abort task and add resources back to the system for next cycle
                        for (int j = 1; j <= Resources; j++)
                        {
                            cout << TaskVector[i].HoldsVector[j] << " ";
                            ResourcesBuffer[j] += TaskVector[i].HoldsVector[j];
                            TaskVector[i].HoldsVector[j] = 0;
                        }

                        cout << "units available next cycle\n";

                        // Set aborted
                        TaskVector[i].setWasAborted(Banker_Reference, true);
                        TaskVector[i].setCurrent_Activity("aborted");
                        TaskVector[i].setRequestPending(false);
                    }

                        // ELSE request does not exceed claims
                    else
                    {
                        // Add request to the queue
                        RequestsQueue.push_back(i);
                    }
                }

                    // ELSE IF activity is release
                else if (activity.Activity == Release)
                {
                    //Throw resources back to the system
                    TaskVector[i].HoldsVector[ReqResource_Type] -= ReqResource_Amount;	// Release resources from task
                    ResourcesBuffer[ReqResource_Type] += ReqResource_Amount;			// Add back to system through buffer
                }
            }
        }

        grantRequests(Banker_Reference);
        // Finished a cycle
        finishedCycle(Banker_Reference);
        Time++;

        // Return resources back to system
        returnResources();
    }
    verbose_methods.log("Bankers Finished-------------------------\n");
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Checks to see if system is deadlocked.
 * Looks to see if all tasks have pending requests. Compares that number with the total number of tasks
 * in the system. If true then the system is deadlocked.
 ---------------------------------------------------------------------------------------------------------------------*/
bool ResourceManager::systemIsDeadlocked(int algorithm)
{
    int Pending = 0;
    int Active_Tasks = 0;


    //Loop through every task
    for (int i = 1; i <= NumTasks; i++)
    {
        //If task has a pending req
        if (TaskVector[i].getRequestPending())
        {
            // Increment number of tasks with pending requests
            Pending++;
        }
    }

    //Loop through every task
    for (int i = 1; i <= NumTasks; i++)
    {
        // IF task is not terminated and aborted
        if (!TaskVector[i].getWasTerminated(algorithm) && !TaskVector[i].getWasAborted(algorithm))
        {
            //Increment number of active tasks
            Active_Tasks++;
        }

        if (TaskVector[i].getTerminating())
        {
            // Increment number of active tasks
            Active_Tasks++;
        }
    }

    if (Pending == Active_Tasks && Active_Tasks > 0)
    {
        // System is deadlocked
        verbose_methods.log("System is DEADLOCKED");
        return true;
    }
    else
    {
        // System is not deadlocked
        return false;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Checks if the system has enough resources to grant a request
 ---------------------------------------------------------------------------------------------------------------------*/
bool ResourceManager::canGrantRequest()
{
    int resourcesAvailable = 0;

    // Resources being requested
    resourcesAvailable = Resources_Held[ReqResource_Type];

    // If the number of resources not allocated  => requested amount
    if (resourcesAvailable >= ReqResource_Amount)
    {
        verbose_methods.log("Request can be granted");
        return true;
    }

    else
    {
        // The request cannot be granted
        return false;
    }
}


/*----------------------------------------------------------------------------------------------------------------------
 * Description: After a task makes a request, the manager must check if the system is in a safe state.
 * Check if granting a request is safe for the system. If it is safe, it wil return true. Otherwise,
 * the manager will not grant that request.
 ---------------------------------------------------------------------------------------------------------------------*/
bool ResourceManager::isSafe(int task_index)
{
    // Local constants and variables
    const int Banker_Reference = 1;	                        // Only banker's algorithm uses this function
    int requestedResource = 0;						        // Requested resource type
    int requestedAmount = 0;						        // Requested resource amount
    int activeTasksCount = 0;	                            // Number of active tasks
    int resourceClaimsMet = 0;	                            // Number of resource claims that are met
    bool stateIsSafe = true;	                            // boolean to indicate if state is safe
    vector<Task> CopyOf_Tasks(TaskVector);			        // Copy of tasks
    vector<bool> activeTasks(CopyOf_Tasks.size(), false);	// Active tasks
    vector<int> copyOf_Resources(Resources_Held);	            // Clone of resources available

    // Get request
    requestedResource = CopyOf_Tasks[task_index].getCurrentActivity().Resource_Type;    // Resource type
    requestedAmount = CopyOf_Tasks[task_index].getCurrentActivity().Resource_Amount;	// Resource amount

    // Assume request is granted
    copyOf_Resources[requestedResource] -= requestedAmount;							    // Take away from system
    CopyOf_Tasks[task_index].HoldsVector[requestedResource] += requestedAmount;	        // Allocate to task

    // Loop through every task
    for (size_t i = 1; i < activeTasks.size(); i++)
    {
        // If task is active
        if (!CopyOf_Tasks[i].getWasAborted(Banker_Reference) && !CopyOf_Tasks[i].getWasTerminated(Banker_Reference))
        {
            // Update the active tasks list
            activeTasks[i] = true;

            // Increment active tasks counter
            activeTasksCount++;
        }
    }

    // While all active tasks have not been checked and state is safe
    while (activeTasksCount != 0 && stateIsSafe)
    {
        stateIsSafe = false;

        //Loop through every task
        for (size_t i = 1; i < CopyOf_Tasks.size(); i++)
        {
            // Reset counters
            resourceClaimsMet = 0;

            if (activeTasks[i])
            {
                // Loop for every resource type
                for (int j = 1; j <= Resources; j++)
                {
                    // IF claims can be met with resources available
                    if (copyOf_Resources[j] >= (CopyOf_Tasks[i].ClaimsVector[j] - CopyOf_Tasks[i].HoldsVector[j]))
                    {
                        // Increment resource claims met counter
                        resourceClaimsMet++;
                    }
                }
                // IF all resource type claims are met task can finish
                if (resourceClaimsMet == Resources && resourceClaimsMet != 0)
                {
                    activeTasksCount--;
                    activeTasks[i] = false;	// Task has finished, no longer active
                    stateIsSafe = true;

                    // FOR every resource
                    for (int k = 1; k <= Resources; k++)
                    {
                        // Release resource back to system
                        copyOf_Resources[k] += CopyOf_Tasks[i].HoldsVector[k];
                        CopyOf_Tasks[i].HoldsVector[k] = 0;
                    }
                }
            }
        }
    }

    if (stateIsSafe)
    {
        verbose_methods.log("State is currently safe");
        return true;
    }

        // ELSE state is unsafe
    else
    {
        // Return false
        return false;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Goes through every task and finds the lowest numbered task that is not aborted or terminated
 * and gets the number for the system to use and abort.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::abortLowestWorkingTask(int algorithm)
{
    // Local variables
    int lowestWorkingTask = 0;
    int taskIndex = 0;


    // FOR every task until a working task is found
    for (int i = 1; i <= NumTasks && lowestWorkingTask == 0; i++)
    {
        // IF task is not aborted and not terminated
        if (!TaskVector[i].getWasAborted(algorithm) && !TaskVector[i].getWasTerminated(algorithm))
        {
            // Lowest working task is found
            lowestWorkingTask = i;
        }
    }

    // Abort lowest working task
    verbose_methods.log("Aborting the lowest working task: ", lowestWorkingTask);
    TaskVector[lowestWorkingTask].setWasAborted(algorithm, true);	// Aborted
    TaskVector[lowestWorkingTask].setCurrent_Activity("aborted");
    TaskVector[lowestWorkingTask].setRequestPending(false);

    // FOR every resource type
    for (int i = 1; i <= Resources; i++)
    {
        // Return resources to the system through resource buffer
        ResourcesBuffer[i] += TaskVector[lowestWorkingTask].HoldsVector[i];
        TaskVector[lowestWorkingTask].HoldsVector[i] = 0;
    }

    // Remove it from the requests queue
    // FOR every task in the queue
    for (size_t j = 0; j < RequestsQueue.size(); j++)
    {
        // Search the queue for the task
        if (RequestsQueue[j] == lowestWorkingTask)
        {
            // Task index found
            taskIndex = j;
        }
    }

    // Delete the task from the queue
    RequestsQueue.erase(RequestsQueue.begin() + taskIndex);
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: This function will increment every tasks activity counter. Required to know when a cycle
 * is finished.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::finishedCycle(int algorithm)
{
    // Loop through every task
    for (int i = 1; i <= NumTasks; i++)
    {
        // If task is terminating
        if (TaskVector[i].getTerminating())
        {
            // If task is not processing termination yet
            if (TaskVector[i].getCurrentActivity().Activity != "terminate")
            {
                // Move it to termination
                TaskVector[i].nextActivity();
            }
                // Set task as terminated
                verbose_methods.log("Task being terminated: ", i);
                TaskVector[i].setCurrent_Activity("terminate");
                TaskVector[i].setWasTerminated(algorithm, true);	// Task has terminated
                TaskVector[i].setTotal_Time(algorithm, Time);	// Set time it took to complete task
                TaskVector[i].setTerminating(false);			// Now terminated
        }

            // If a task does not have a pending request and is not terminated and not aborted
        else if (!TaskVector[i].getRequestPending()
                 && !TaskVector[i].getWasTerminated(algorithm)
                 && !TaskVector[i].getWasAborted(algorithm))
        {
            // Next activity
            TaskVector[i].nextActivity();
        }
    }
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Travels through the request queue and grants all requests that can be granted.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::grantRequests(int algorithm)
{
    // Local constants and variables
    const int FIFO_Reference = 0;
    const int BANKERSReference = 1;
    int Waiting_Time = 0;				// Time task has spent waiting
    int taskIndex = 0;					// Index of task
    vector<int> Granted_Requests;	    // A list of tasks that have had their requests granted


    // For every task in the request queue
    for (size_t i = 0; i < RequestsQueue.size(); i++)
    {
        // Get requested resource
        ReqResource_Type = TaskVector[RequestsQueue[i]].getCurrentActivity().Resource_Type;
        ReqResource_Amount = TaskVector[RequestsQueue[i]].getCurrentActivity().Resource_Amount;

        // Try to grant requests in order of queue
        if (canGrantRequest())
        {
            switch (algorithm)
            {
                case FIFO_Reference:
                {
                    // Grant request
                    Resources_Held[ReqResource_Type] -= ReqResource_Amount;								    // Resources from system
                    TaskVector[RequestsQueue[i]].HoldsVector[ReqResource_Type] += ReqResource_Amount;	// Grant the request
                    TaskVector[RequestsQueue[i]].setRequestPending(false);					           	// Set task as not request pending
                    Granted_Requests.push_back(RequestsQueue[i]);							            // Add to deletion list
                    break;
                }

                case BANKERSReference:
                {
                    // IF granting the request produces a safe state
                    if (isSafe(RequestsQueue[i]))
                    {
                        // Grant request
                        Resources_Held[ReqResource_Type] -= ReqResource_Amount;							    	// Resources from the system
                        TaskVector[RequestsQueue[i]].HoldsVector[ReqResource_Type] += ReqResource_Amount;	// Grant the request
                        TaskVector[RequestsQueue[i]].setRequestPending(false);					           	// Set task as not request pending
                        Granted_Requests.push_back(RequestsQueue[i]);							            // Add to deletion list
                    }

                        // ELSE state is not safe, postpone request
                    else
                    {
                        // Increment wait time counter
                        Waiting_Time = TaskVector[RequestsQueue[i]].getTimeWaiting(algorithm) + 1;
                        TaskVector[RequestsQueue[i]].setTimeWaiting(algorithm, Waiting_Time);
                    }
                }
            }
        }

            // Else the system cannot grant the request
        else
        {
            // Increment wait time counter
            Waiting_Time = TaskVector[RequestsQueue[i]].getTimeWaiting(algorithm) + 1;
            TaskVector[RequestsQueue[i]].setTimeWaiting(algorithm, Waiting_Time);
        }
    }

    // For every task that had their request granted
    for (size_t i = 0; i < Granted_Requests.size(); i++)
    {
        // Go through queue
        for (size_t j = 0; j < RequestsQueue.size(); j++)
        {
            // Search the queue
            if (RequestsQueue[j] == Granted_Requests[i])
            {
                // Task index found
                taskIndex = j;
            }
        }

        // Delete the task from the queue
        RequestsQueue.erase(RequestsQueue.begin() + taskIndex);
    }
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Returns resources back into the system. Utilizes a buffer. Resources released
 * are not available until the cycle after.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::returnResources()
{
    // FOR every resource type
    for (int i = 1; i <= Resources; i++)
    {
        // Place the resources from the buffer back into the system
        Resources_Held[i] += ResourcesBuffer[i];
        ResourcesBuffer[i] = 0;

    }
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Prints the results in the format required in the lab.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::printResults()
{
    verbose_methods.log("PRINTING RESULTS----------------------------------------|");
    // Local variables
    double timePercentage = 0;
    int totalTimeTaken[2] = { 0, 0 };		// Task completion time
    int totalTimeWaiting[2] = { 0, 0 };		// Waiting Time


    // Set decimal precision
    cout << fixed << setprecision(0);

    // Print algorithm names
    cout << "------------------------------------------------------------------" << endl;
    cout << "              FIFO              |             BANKER'S           |" << endl;

    // FOR each task
    for (int i = 1; i <= NumTasks; i++)
    {
        // FOR each algorithm
        for (int j = 0; j < NumAlgorithms; j++)
        {
            cout << "     Task " << i;

            // IF task was aborted
            if (TaskVector[i].getWasAborted(j))
            {
                // Print task was aborted
                cout << setw(6) << " " << "aborted        |";
            }

                // ELSE task terminated successfully
            else
            {
                timePercentage = (double)TaskVector[i].getTimeWaiting(j)
                                 / (double)TaskVector[i].getTotal_Time(j) * 100 + 0.01;	// +0.01 is for .5's not rounding up

                // Print task's time taken, time waiting, percentage of time waiting
                cout << setw(7) << TaskVector[i].getTotal_Time(j)				    // Time taken
                << setw(4) << TaskVector[i].getTimeWaiting(j) << setw(4)	        // Time waiting
                << timePercentage << "%" << "     |";							    // Time percentage

                // Add time taken and time waiting to sum of algorithm
                totalTimeTaken[j] += TaskVector[i].getTotal_Time(j);
                totalTimeWaiting[j] += TaskVector[i].getTimeWaiting(j);
            }
        }

        cout << endl;	// New line for next task
    }

    // FOR each algorithm
    for (int i = 0; i < NumAlgorithms; i++)
    {
        // Get percentage of time waiting
        timePercentage = (double)totalTimeWaiting[i] / (double)totalTimeTaken[i] * 100;

        // Print total time taken, time waiting, percentage of time waiting
        cout << "     total " << setw(7) << totalTimeTaken[i]		// Time taken
        << setw(4) << totalTimeWaiting[i] << setw(4)			    // Time waiting
        << timePercentage << "%" << "     |";				        // Time percentage
    }
    cout << endl << endl;

}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Resets data structures, data values in order for the next algorithm to run with default values.
 ---------------------------------------------------------------------------------------------------------------------*/
void ResourceManager::resetTasks(string input_file)
{
    // Local variables
    ifstream input_stream;	    // Used to fetch data from the input file
    int tempData;				// Used to temporarily store data that was fetched from input file
    Activity activity;			// Activity that will be added to a task
    int taskNum;				// Task number that activity belongs to


    // Set up input stream
    input_stream.open(input_file.c_str());	// Open file

    // Data should be the same and already retrieved from constructor
    input_stream >> tempData;		// Unused
    input_stream >> tempData;		// Unused

    // FOR every resource type
    for (int i = 0; i < Resources; i++)
    {
        // Skip resource amount
        input_stream >> tempData;
    }

    // FOR every task
    for (int i = 1; i <= NumTasks; i++)
    {

        // Reset arrays
        TaskVector[i].ClaimsVector.clear();
        TaskVector[i].HoldsVector.clear();
        TaskVector[i].ActivitiesVector.clear();

        // Set up 1 based array for simple referencing
        TaskVector[i].ClaimsVector.push_back(0);
        TaskVector[i].HoldsVector.push_back(0);

        // Reset data members
        TaskVector[i].setCurrent_Activity("unused");
        TaskVector[i].setResourceType(0);
        TaskVector[i].setResourceAmt(0);
        TaskVector[i].setCurrentAlgorithm(1); //Set to bankers
        TaskVector[i].setRequestPending(false);
        TaskVector[i].setTerminating(false);
        TaskVector[i].resetActivityCounter();
    }

    // Read whole file again to get data
    while (!input_stream.eof())
    {
        input_stream >> activity.Activity;		    // Get activity type
        input_stream >> taskNum;					// Get task being referenced
        input_stream >> activity.Resource_Type;		    // Get resource type being referenced
        input_stream >> activity.Resource_Amount;	// Get amount of resources being requested or released
        TaskVector[taskNum].ActivitiesVector.push_back(activity);
    }

    input_stream.close();
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Checks if a specific task is in the request queue or not.
 ---------------------------------------------------------------------------------------------------------------------*/
bool ResourceManager::inQueue(int task_num)
{
    bool foundTask = false;

    //Go through queue
    for (size_t i = 0; i < RequestsQueue.size(); i++)
    {
        // Search the queue for target task
        if (RequestsQueue[i] == task_num)
        {
            // Task was found
            foundTask = true;
        }
    }

    // Return found or not
    return foundTask;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Description: Checks if all tasks are completed. Goes through every task to
 * verify it is completed. If it is completed, then increment the counter.
 * Compare the counter with the total number of tasks.
 ---------------------------------------------------------------------------------------------------------------------*/
bool ResourceManager::completedAllTasks(int algorithm)
{

    int Tasks_Completed = 0;	// Number of completed tasks


    // FOR every task
    for (int i = 1; i <= NumTasks; i++)
    {
        // IF task has aborted or terminated
        if (TaskVector[i].getWasTerminated(algorithm) || TaskVector[i].getWasAborted(algorithm))
        {
            Tasks_Completed++;
        }
    }

    if (Tasks_Completed == NumTasks)
    {
        // Return true
        verbose_methods.log("All Tasks Completed!");
        return true;
    }

    else
    {
        // Return false if not all tasks are done
        return false;
    }
}