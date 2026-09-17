#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex])
;

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}

// Implement your schedulers here!
void first_come_first_served(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int taskIndex = 0; 

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);
 
        while (tasks[taskIndex]->state != finished && globalTime < timeout)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }
    

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}

void shortest_process_next(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    int taskIndex = 0;

    do
    {
        int shortestTask = -1;

          for (int i = 0; i<taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                if (shortestTask == -1 || tasks[i]->totalRuntime < tasks[shortestTask]->totalRuntime)
                {
                    shortestTask = i;
                }
            }
        }

        if (shortestTask == -1)
        { 
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }
        taskIndex = shortestTask;


        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;

        set_task_state(tasks[taskIndex], running);

        while (tasks[taskIndex]->state != finished && globalTime < timeout)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }
    

       
    } while (globalTime < timeout);
}


void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout)
{
    // Implement your solution here
    // Implement your solution here
    int taskIndex = 0;

    do
    {
        int highest_task = -1;
        double highestRatio = -1;

          for (int i = 0; i<taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                int waiting_time = globalTime - tasks[i]->arrivalTime;

                double responeRatio  = (double)(waiting_time+tasks[i]->totalRuntime) / tasks[i]->totalRuntime;

                if (highest_task == -1 || responeRatio > highestRatio)
                {
                    highest_task = i;
                    highestRatio = responeRatio;
                }
            }
        }

        if (highest_task == -1)
        { 
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }
        taskIndex = highest_task;


        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;

        set_task_state(tasks[taskIndex], running);

        while (tasks[taskIndex]->state != finished && globalTime < timeout)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
        }
    


       
    } while (globalTime < timeout);
}

void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    // Implement your solution here
    int taskIndex = 0;

    do
    {
        int shortestTask = -1;

          for (int i = 0; i<taskCount; i++)
        {
            if (tasks[i]->state != finished &&
                tasks[i]->arrivalTime <= globalTime)
            {
                int remTime = 
                tasks[i]->totalRuntime - tasks[i]->currentRuntime;

                if (shortestTask == -1 ||
                     remTime < 
                     (tasks[shortestTask]->totalRuntime - 
                      tasks[shortestTask]->currentRuntime))
                {
                    shortestTask = i;
                }
            }
        }

        if (shortestTask == -1)
        { 
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }
        taskIndex = shortestTask;

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
        {
            tasks[taskIndex]->startTime = globalTime;
        }

        set_task_state(tasks[taskIndex], running);


                // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        if (tasks[taskIndex]->state != finished)
        {
            set_task_state(tasks[taskIndex], preempted);
        }

}while (globalTime -timeout);
}


void feedback(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    // Implement your solution here

}
