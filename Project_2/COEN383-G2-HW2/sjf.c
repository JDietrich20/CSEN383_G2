#include <stdio.h>
#include <string.h>
#include "process.h"

Metrics sjf(Process processes[], int num_processes, char (*timeline)[10]) {
    double current_time = 0.0;
    int completed = 0;
    
    while (completed < num_processes) {
        int next_process = -1;
        double shortest_time = 1000.0;
        
        // Find shortest job that has arrived
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && 
                processes[i].arrival_time <= current_time &&
                processes[i].service_time < shortest_time) {
                shortest_time = processes[i].service_time;
                next_process = i;
            }
        }
        
        if (next_process != -1) {
            Process *p = &processes[next_process];
            
            // Don't start new process after quantum 99
            if (current_time > 99.0) {
                break;
            }
            
            // Record start time if first run
            if (p->start_time < 0.0) {
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
        } else {
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
            if (!has_future) break;
        }
    }
    
    return calculate_metrics(processes, num_processes);
}