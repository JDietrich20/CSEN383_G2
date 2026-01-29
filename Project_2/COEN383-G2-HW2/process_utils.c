#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "process.h"

void generate_workload(Process processes[], int num_processes, unsigned int seed) {
    srand(seed);
    
    for (int i = 0; i < num_processes; i++) {
        processes[i].id = i + 1;
        // Generate process name (A, B, C, ..., Z, a, a, ...)
        if (i < 26) {
            processes[i].name[0] = 'A' + i;
            processes[i].name[1] = '\0';
        } else {
            processes[i].name[0] = 'a';
            processes[i].name[1] = '\0';
        }
        
        // Generate float values
        processes[i].arrival_time = (double)(rand() % 100);  // 0.0 to 99.0
        processes[i].service_time = ((rand() % 100) / 10.0) + 0.1;  // 0.1 to 10.0
        processes[i].remaining_time = processes[i].service_time;
        processes[i].priority = (rand() % 4) + 1;  // 1 to 4
        processes[i].start_time = -1.0;
        processes[i].finish_time = 0.0;
        processes[i].turnaround_time = 0.0;
        processes[i].wait_time = 0.0;
        processes[i].response_time = 0.0;
        processes[i].completed = 0;
        processes[i].first_run = 0;
    }
    
    // Sort by arrival time
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = 0; j < num_processes - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void print_processes(Process processes[], int num_processes) {
    printf("\nProcess Details:\n");
    printf("Name\tArrival\tBurst\tPriority\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%s\t%.1f\t%.1f\t%d\n", 
               processes[i].name,
               processes[i].arrival_time, 
               processes[i].service_time, 
               processes[i].priority);
    }
}

void print_timeline(int timeline[], int length) {
    printf("\nTimeline (0-%d):\n", length - 1);
    for (int i = 0; i < length; i++) {
        if (timeline[i] == -1) {
            printf(".");  // Idle
        } else {
            printf("%d", timeline[i]);
        }
        if ((i + 1) % 50 == 0) printf("\n");
    }
    printf("\n");
}

Metrics calculate_metrics(Process processes[], int num_processes) {
    Metrics m = {0, 0, 0, 0};
    int completed = 0;
    
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].completed && processes[i].start_time <= 99.0) {
            m.avg_turnaround += processes[i].turnaround_time;
            m.avg_wait += processes[i].wait_time;
            m.avg_response += processes[i].response_time;
            completed++;
        }
    }
    
    if (completed > 0) {
        m.avg_turnaround /= completed;
        m.avg_wait /= completed;
        m.avg_response /= completed;
    }
    
    m.throughput = (double)completed;
    
    return m;
}

HPFMetrics calculate_hpf_metrics(Process processes[], int num_processes) {
    HPFMetrics hm;
    memset(&hm, 0, sizeof(HPFMetrics));
    
    int total_completed = 0;
    int priority_counts[4] = {0};
    
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].completed && processes[i].start_time <= 99.0) {
            int p = processes[i].priority - 1;
            
            hm.priority[p].avg_turnaround += processes[i].turnaround_time;
            hm.priority[p].avg_wait += processes[i].wait_time;
            hm.priority[p].avg_response += processes[i].response_time;
            priority_counts[p]++;
            
            hm.overall.avg_turnaround += processes[i].turnaround_time;
            hm.overall.avg_wait += processes[i].wait_time;
            hm.overall.avg_response += processes[i].response_time;
            total_completed++;
        }
    }
    
    for (int p = 0; p < 4; p++) {
        if (priority_counts[p] > 0) {
            hm.priority[p].avg_turnaround /= priority_counts[p];
            hm.priority[p].avg_wait /= priority_counts[p];
            hm.priority[p].avg_response /= priority_counts[p];
        }
        hm.priority[p].throughput = (double)priority_counts[p];
    }
    
    if (total_completed > 0) {
        hm.overall.avg_turnaround /= total_completed;
        hm.overall.avg_wait /= total_completed;
        hm.overall.avg_response /= total_completed;
    }
    hm.overall.throughput = (double)total_completed;
    
    return hm;
}

void reset_processes(Process original[], Process copy[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        copy[i] = original[i];
        copy[i].remaining_time = copy[i].service_time;
        copy[i].start_time = -1.0;
        copy[i].finish_time = 0.0;
        copy[i].turnaround_time = 0.0;
        copy[i].wait_time = 0.0;
        copy[i].response_time = 0.0;
        copy[i].completed = 0;
        copy[i].first_run = 0;
    }
}