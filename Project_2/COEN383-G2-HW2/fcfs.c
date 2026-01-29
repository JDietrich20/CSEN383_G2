#include <stdio.h>
#include <string.h>
#include "process.h"

Metrics fcfs(Process processes[], int num_processes, char (*timeline)[10])
{
    double current_time = 0.0;
    int completed = 0;

    // While there are uncompleted processes
    while (completed < num_processes)
    {
        int next_process = -1; // Index of next process to execute (-1 if none)

        // Find next process that has arrived and not completed
        for (int i = 0; i < num_processes; i++)
        {
            if (!processes[i].completed &&
                processes[i].arrival_time <= current_time) // FCFS: pick the first arrived
            {
                next_process = i;
                break;
            }
        }

        if (next_process != -1)
        {
            Process *p = &processes[next_process]; // Pointer to the selected process because we will modify it

            // Don't start new process after quantum 99
            if (current_time > 99.0)
            {
                break;
            }

            // Record start time if first run
            if (p->start_time < 0.0)
            {
                p->start_time = current_time;
                p->response_time = current_time - p->arrival_time;
            }

            // Execute the process completely
            current_time += p->service_time;
            p->finish_time = current_time;
            p->turnaround_time = p->finish_time - p->arrival_time;
            p->wait_time = p->turnaround_time - p->service_time;
            p->completed = 1;
            completed++;
        }
        else
        {
            // No process ready, advance time
            current_time += 1.0;

            // Check if we should stop because no future processes will arrive
            int has_future = 0;
            for (int i = 0; i < num_processes; i++)
            {
                if (!processes[i].completed && processes[i].arrival_time > current_time)
                {
                    has_future = 1;
                    break;
                }
            }
            if (!has_future)
                break;
        }
    }

    return calculate_metrics(processes, num_processes);
}