#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

extern Metrics fcfs(Process processes[], int num_processes, char (*timeline)[10]);
extern Metrics sjf(Process processes[], int num_processes, char (*timeline)[10]);
extern Metrics srt(Process processes[], int num_processes, char (*timeline)[10]);
extern Metrics round_robin(Process processes[], int num_processes, char (*timeline)[10]);
extern HPFMetrics hpf_nonpreemptive(Process processes[], int num_processes, char (*timeline)[10]);
extern HPFMetrics hpf_preemptive(Process processes[], int num_processes, char (*timeline)[10]);

void print_gantt_chart(Process processes[], int num_processes) {
    char chart[1000];
    memset(chart, '_', 999);
    chart[999] = '\0';

    int max_time = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].completed && processes[i].finish_time > 0) {
            int start = (int)processes[i].start_time;
            int end = (int)processes[i].finish_time;
           
            for (int t = start; t < end && t < 999; t++) {
                chart[t] = processes[i].name[0];
            }
            if (end > max_time) max_time = end;
        }
    }
   
    if (max_time > 999) max_time = 999;
    chart[max_time] = '\0';
   
    printf("%s\n\n", chart);
}

void print_input_table(Process p[], int n) {
    printf("process_name | arrival_time | burst_time | process_priority |\n");
    for(int i=0; i<n; i++) {
        printf("%10s | %12.1f | %10.1f | %16d |\n",
               p[i].name, p[i].arrival_time, p[i].service_time, p[i].priority);
    }
    printf("total_number_of_processes: %d\n\n", n);
}

void print_detailed_table(Process p[], int n) {
    printf("process_name | arrival_time | start_time | end_time | burst_time | response_time | wait_time | turn_around_time | priority |\n");
    printf(" -------------------------------------------------------------------------------------------------------------------------------- \n");
   
    for(int i=0; i<n; i++) {
        if(p[i].finish_time > 0) {
             printf("%13s| %13.1f| %12.1f| %10.1f| %10.1f| %14.1f| %10.1f| %17.1f| %9d|\n",
                p[i].name, p[i].arrival_time, p[i].start_time, p[i].finish_time,
                p[i].service_time, p[i].response_time, p[i].wait_time,
                p[i].turnaround_time, p[i].priority);
        }
    }
}

void print_hpf_by_priority(Process p[], int n) {
    for (int prio = 1; prio <= 4; prio++) {
        printf("For Priority Queue %d\n", prio);
        printf("process_name | arrival_time | start_time | end_time | burst_time | response_time | wait_time | turn_around_time | priority |\n");
        printf(" -------------------------------------------------------------------------------------------------------------------------------- \n");
       
        int count = 0;
        double total_tat=0, total_wt=0, total_rt=0;

        for(int i=0; i<n; i++) {
            if(p[i].priority == prio && p[i].finish_time > 0) {
                 printf("%13s| %13.1f| %12.1f| %10.1f| %10.1f| %14.1f| %10.1f| %17.1f| %9d|\n",
                    p[i].name, p[i].arrival_time, p[i].start_time, p[i].finish_time,
                    p[i].service_time, p[i].response_time, p[i].wait_time,
                    p[i].turnaround_time, p[i].priority);
                 
                 total_tat += p[i].turnaround_time;
                 total_wt += p[i].wait_time;
                 total_rt += p[i].response_time;
                 count++;
            }
        }
       
        printf("      Average|           0.0|         0.0|       0.0|       0.0| %14.1f| %10.1f| %17.1f|\n\n",
            (count > 0 ? total_rt/count : 0.0),
            (count > 0 ? total_wt/count : 0.0),
            (count > 0 ? total_tat/count : 0.0));
    }
}

#define NUM_PROCESSES 40
#define NUM_RUNS 5
#define MAX_TIME 500

int main() {
    Process original_workload[NUM_PROCESSES];
    Process temp_processes[NUM_PROCESSES];
    char timeline[MAX_TIME][10];
   
    Metrics fcfs_final = {0}, sjf_final = {0}, srt_final = {0}, rr_final = {0};
    HPFMetrics hpf_np_final = {0}, hpf_p_final = {0};

    printf("Enter number of processes: %d\n", NUM_PROCESSES);

    unsigned int seed = 42;

    for (int run = 0; run < NUM_RUNS; run++) {
        printf("\t\tIteration #%d\n", run + 1);
       
        generate_workload(original_workload, NUM_PROCESSES, seed + run);
       
        print_input_table(original_workload, NUM_PROCESSES);

        printf("FIRST COME FIRST SERVE:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        Metrics m_fcfs = fcfs(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_detailed_table(temp_processes, NUM_PROCESSES);
        printf("      Average|           0.0|         0.0|       0.0|       0.0| %10.1f| %10.1f| %16.1f|\n\n",
               m_fcfs.avg_response, m_fcfs.avg_wait, m_fcfs.avg_turnaround);
       
        fcfs_final.avg_turnaround += m_fcfs.avg_turnaround;
        fcfs_final.avg_wait += m_fcfs.avg_wait;
        fcfs_final.avg_response += m_fcfs.avg_response;
        fcfs_final.throughput += m_fcfs.throughput;

        printf("ROUND ROBIN:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        Metrics m_rr = round_robin(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_detailed_table(temp_processes, NUM_PROCESSES);
        printf("      Average|           0.0|         0.0|       0.0|       0.0| %10.1f| %10.1f| %16.1f|\n\n",
               m_rr.avg_response, m_rr.avg_wait, m_rr.avg_turnaround);
       
        rr_final.avg_turnaround += m_rr.avg_turnaround;
        rr_final.avg_wait += m_rr.avg_wait;
        rr_final.avg_response += m_rr.avg_response;
        rr_final.throughput += m_rr.throughput;

        printf("SHORTEST JOB FIRST:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        Metrics m_sjf = sjf(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_detailed_table(temp_processes, NUM_PROCESSES);
        printf("      Average|           0.0|         0.0|       0.0|       0.0| %10.1f| %10.1f| %16.1f|\n\n",
               m_sjf.avg_response, m_sjf.avg_wait, m_sjf.avg_turnaround);
       
        sjf_final.avg_turnaround += m_sjf.avg_turnaround;
        sjf_final.avg_wait += m_sjf.avg_wait;
        sjf_final.avg_response += m_sjf.avg_response;
        sjf_final.throughput += m_sjf.throughput;

        printf("SHORTEST REMAINING TIME:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        Metrics m_srt = srt(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_detailed_table(temp_processes, NUM_PROCESSES);
        printf("      Average|           0.0|         0.0|       0.0|       0.0| %10.1f| %10.1f| %16.1f|\n\n",
               m_srt.avg_response, m_srt.avg_wait, m_srt.avg_turnaround);

        srt_final.avg_turnaround += m_srt.avg_turnaround;
        srt_final.avg_wait += m_srt.avg_wait;
        srt_final.avg_response += m_srt.avg_response;
        srt_final.throughput += m_srt.throughput;

        printf("HIGHEST PRIORITY FIRST PREEMPTIVE:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        HPFMetrics m_hpf_p = hpf_preemptive(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_hpf_by_priority(temp_processes, NUM_PROCESSES);
       
        printf("The average times of High Priority First Preemptive for all queues:\n");
        printf("Average Turn Around Time(TAT) :%.1f\n", m_hpf_p.overall.avg_turnaround);
        printf("Average Wait Time(WT) : %.1f\n", m_hpf_p.overall.avg_wait);
        printf("Average Response Time(RT) : %.1f\n\n", m_hpf_p.overall.avg_response);

        hpf_p_final.overall.avg_turnaround += m_hpf_p.overall.avg_turnaround;
        hpf_p_final.overall.avg_wait += m_hpf_p.overall.avg_wait;
        hpf_p_final.overall.avg_response += m_hpf_p.overall.avg_response;
        hpf_p_final.overall.throughput += m_hpf_p.overall.throughput;

        printf("HIGHEST PRIORITY FIRST NON PREEMPTIVE:\n");
        reset_processes(original_workload, temp_processes, NUM_PROCESSES);
        HPFMetrics m_hpf_np = hpf_nonpreemptive(temp_processes, NUM_PROCESSES, timeline);
        print_gantt_chart(temp_processes, NUM_PROCESSES);
        print_hpf_by_priority(temp_processes, NUM_PROCESSES);

        printf("The Calculated statistics of High Priority First Non Preemptive for all priority queues is:\n");
        printf("Average Turn Around Time(TAT) :%.1f\n", m_hpf_np.overall.avg_turnaround);
        printf("Average Wait Time(WT) : %.1f\n", m_hpf_np.overall.avg_wait);
        printf("Average Response Time(RT) : %.1f\n", m_hpf_np.overall.avg_response);

        hpf_np_final.overall.avg_turnaround += m_hpf_np.overall.avg_turnaround;
        hpf_np_final.overall.avg_wait += m_hpf_np.overall.avg_wait;
        hpf_np_final.overall.avg_response += m_hpf_np.overall.avg_response;
        hpf_np_final.overall.throughput += m_hpf_np.overall.throughput;
    }

    printf("\nAll STATISTICS FOR %d ITERATIONS\n\n", NUM_RUNS);
   
    printf("FIRST COME FIRST SERVE (FCFS):\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", fcfs_final.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", fcfs_final.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", fcfs_final.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n\n", fcfs_final.throughput / NUM_RUNS);

    printf("SHORTEST JOB FIRST (SJF):\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", sjf_final.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", sjf_final.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", sjf_final.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n\n", sjf_final.throughput / NUM_RUNS);

    printf("SHORTEST REMAINING TIME (SRT):\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", srt_final.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", srt_final.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", srt_final.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n\n", srt_final.throughput / NUM_RUNS);

    printf("ROUND ROBIN (RR):\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", rr_final.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", rr_final.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", rr_final.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n\n", rr_final.throughput / NUM_RUNS);

    printf("HIGHEST PRIORITY FIRST (HPF) [NON-PREEMPTIVE]:\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", hpf_np_final.overall.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", hpf_np_final.overall.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", hpf_np_final.overall.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n\n", hpf_np_final.overall.throughput / NUM_RUNS);

    printf("HIGHEST PRIORITY FIRST (HPF) [PREEMPTIVE]:\n");
    printf("Average Turn Around Time(TAT) :%.1f\n", hpf_p_final.overall.avg_turnaround / NUM_RUNS);
    printf("Average Wait Time(WT) : %.1f\n", hpf_p_final.overall.avg_wait / NUM_RUNS);
    printf("Average Response Time(RT) : %.1f\n", hpf_p_final.overall.avg_response / NUM_RUNS);
    printf("Average Throughput :%.1f\n", hpf_p_final.overall.throughput / NUM_RUNS);

    return 0;
}
