#include <stdio.h>
#include <string.h>
#include "process.h"

HPFMetrics hpf_preemptive(Process processes[], int num_processes, char (*timeline)[10]) {
    double current_time = 0.0;
    int completed = 0;
    int last_process_index[4] = {-1, -1, -1, -1};
    
    while (completed < num_processes) {
        int next_process = -1;
        int highest_priority = 5;
        
        // Find highest priority process
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && 
                processes[i].arrival_time <= current_time &&
                processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
            }
        }
        
        if (highest_priority < 5) {
            // Use Round Robin within this priority level
            int priority_idx = highest_priority - 1;
            int start_idx = (last_process_index[priority_idx] + 1) % num_processes;
            int checked = 0;
            
            while (checked < num_processes) {
                int i = (start_idx + checked) % num_processes;
                
                if (!processes[i].completed && 
                    processes[i].arrival_time <= current_time &&
                    processes[i].priority == highest_priority) {
                    next_process = i;
                    last_process_index[priority_idx] = i;
                    break;
                }
                checked++;
            }
        }
        
        if (next_process != -1) {
            Process *p = &processes[next_process];
            
            // Don't start new process after quantum 99
            if (p->start_time < 0.0 && current_time > 99.0) {
                last_process_index[p->priority - 1] = next_process;
                
                // Try to find another process that already started
                int found_other = 0;
                for (int i = 0; i < num_processes; i++) {
                    if (!processes[i].completed && 
                        processes[i].arrival_time <= current_time &&
                        processes[i].start_time >= 0.0) {
                        found_other = 1;
                        break;
                    }
                }
                if (!found_other) break;
            }
            
            // Record start time on first run
            if (p->start_time < 0.0) {
                p->start_time = current_time;
                p->response_time = current_time - p->arrival_time;
            }
            
            // Execute for 1 quantum (preemptive RR)
            double exec_time = (p->remaining_time < 1.0) ? p->remaining_time : 1.0;
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
    
    return calculate_hpf_metrics(processes, num_processes);
}
