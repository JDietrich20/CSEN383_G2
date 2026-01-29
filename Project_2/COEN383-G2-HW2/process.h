#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    int id;
    char name[3];
    double arrival_time;
    double service_time;
    double remaining_time;
    int priority;
    double start_time;
    double finish_time;
    double turnaround_time;
    double wait_time;
    double response_time;
    int completed;
    int first_run;
} Process;

typedef struct {
    double avg_turnaround;
    double avg_wait;
    double avg_response;
    double throughput;
} Metrics;

typedef struct {
    Metrics overall;
    Metrics priority[4];  // Metrics for each priority level (0-3 for priorities 1-4)
} HPFMetrics;

// Function prototypes
void generate_workload(Process processes[], int num_processes, unsigned int seed);
void print_processes(Process processes[], int num_processes);
void print_timeline(int timeline[], int length);
Metrics calculate_metrics(Process processes[], int num_processes);
HPFMetrics calculate_hpf_metrics(Process processes[], int num_processes);
void reset_processes(Process original[], Process copy[], int num_processes);

#endif