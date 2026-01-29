#include <stdio.h>
#include <string.h>
#include "process.h"

#define TIME_QUANTUM 1.0

Metrics round_robin(Process processes[], int num_processes, char (*timeline)[10]) {
    double current_time = 0.0;
    int completed = 0;
    int current_index = 0;
   
    while (completed < num_processes) {
        int found = 0;
        int checked = 0;
       
        // Round-robin: check processes starting from current_index
        while (checked < num_processes) {
            int i = (current_index + checked) % num_processes;
           
            if (!processes[i].completed &&
                processes[i].arrival_time <= current_time) {
                Process *p = &processes[i];
               
                // Don't start new process after quantum 99
                if (p->start_time < 0.0 && current_time > 99.0) {
                    checked++;
                    continue;
                }
               
                found = 1;
               
                // Record start time on first run
                if (p->start_time < 0.0) {
                    p->start_time = current_time;
                    p->response_time = current_time - p->arrival_time;
                }
               
                // Execute for time quantum or remaining time
                double exec_time = (p->remaining_time < TIME_QUANTUM) ?
                                  p->remaining_time : TIME_QUANTUM;
               
                current_time += exec_time;
                p->remaining_time -= exec_time;
               
                // Check if completed
                if (p->remaining_time < 0.01) {
                    p->finish_time = current_time;
                    p->turnaround_time = p->finish_time - p->arrival_time;
                    p->wait_time = p->turnaround_time - p->service_time;
                    p->completed = 1;
                    completed++;
                }
               
                current_index = (i + 1) % num_processes;
                break;
            }
            checked++;
        }
       
        if (!found) {
            // No process ready, advance time
            current_time += 1.0;
           
            // Check if we should stop
            int has_future = 0;
            for (int i = 0; i < num_processes; i++) {
                if (!processes[i].completed && processes[i].arrival_time > current_time) {
                    has_future = 1;
                    break;
                }
            }
           
            // Also check if there are any started processes still running
            int has_started = 0;
            for (int i = 0; i < num_processes; i++) {
                if (!processes[i].completed && processes[i].start_time >= 0.0) {
                    has_started = 1;
                    break;
                }
            }
           
            if (!has_future && !has_started) break;
        }
    }
   
    return calculate_metrics(processes, num_processes);
}
