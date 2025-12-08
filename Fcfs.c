#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATED 4

#define MAX_PROCS 100
#define MAX_KILLS 100

typedef struct PCB {
    char name[32];
    int pid;
    int total_cpu_burst;
    int io_start_time;
    int io_duration;
    int executed_cpu;
    int remaining_io;
    int arrival_time;
    int completion_time;
    int state;
    int killed;
} PCB;

typedef struct Node {
    PCB *proc;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

typedef struct KillEvent {
    int pid;
    int kill_time;
} KillEvent;

PCB processes[MAX_PROCS];
KillEvent kill_events[MAX_KILLS];
Queue ready_q, waiting_q, terminated_q;

int proc_count = 0;
int kill_count = 0;

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

void setup() {
    init_queue(&ready_q);
    init_queue(&waiting_q);
    init_queue(&terminated_q);
}

void enqueue(Queue *q, PCB *p) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->proc = p;
    n->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = n;
    } else {
        q->rear->next = n;
        q->rear = n;
    }
}

PCB* dequeue(Queue *q) {
    if (q->front == NULL) return NULL;

    Node *temp = q->front;
    PCB *p = temp->proc;

    q->front = temp->next;
    if (q->front == NULL) q->rear = NULL;

    free(temp);
    return p;
}

PCB* find_process(int pid) {
    for (int i = 0; i < proc_count; i++) {
        if (processes[i].pid == pid)
            return &processes[i];
    }
    return NULL;
}

void load_input() {
    FILE *fp = fopen("input.txt", "r");

    while (!feof(fp)) {
        char line[100];
        if (!fgets(line, 100, fp)) break;

        if (strncmp(line, "KILL", 4) == 0) {
            sscanf(line, "KILL %d %d",
                   &kill_events[kill_count].pid,
                   &kill_events[kill_count].kill_time);
            kill_count++;
        } 
        else {
            PCB *p = &processes[proc_count];

            sscanf(line, "%s %d %d %d %d",
                   p->name, &p->pid,
                   &p->total_cpu_burst,
                   &p->io_start_time,
                   &p->io_duration);

            p->executed_cpu = 0;
            p->remaining_io = 0;
            p->arrival_time = 0;
            p->completion_time = 0;
            p->state = READY;
            p->killed = 0;

            enqueue(&ready_q, p);
            proc_count++;
        }
    }

    fclose(fp);
}

void process_kills(int time, PCB **running) {
    for (int i = 0; i < kill_count; i++) {
        if (kill_events[i].kill_time == time) {
            PCB *p = find_process(kill_events[i].pid);

            if (p == NULL || p->state == TERMINATED) continue;

            p->killed = 1;
            p->state = TERMINATED;
            p->completion_time = time;

            if (*running == p) *running = NULL;

            enqueue(&terminated_q, p);
        }
    }
}

void update_waiting() {
    Node *prev = NULL;
    Node *curr = waiting_q.front;

    while (curr != NULL) {
        PCB *p = curr->proc;
        p->remaining_io--;

        if (p->remaining_io <= 0) {
            p->state = READY;
            enqueue(&ready_q, p);

            Node *del = curr;
            if (prev == NULL) waiting_q.front = curr->next;
            else prev->next = curr->next;

            if (del == waiting_q.rear)
                waiting_q.rear = prev;

            curr = curr->next;
            free(del);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

int is_done(PCB *running) {
    return ready_q.front == NULL &&
           waiting_q.front == NULL &&
           running == NULL;
}

void run_fcfs() {
    int time = 0;
    PCB *running = NULL;

    while (!is_done(running)) {

        process_kills(time, &running);

        if (running == NULL && ready_q.front != NULL) {
            running = dequeue(&ready_q);
            running->state = RUNNING;
        }

        if (running != NULL && !running->killed) {
            running->executed_cpu++;

            if (running->executed_cpu == running->total_cpu_burst) {
                running->state = TERMINATED;
                running->completion_time = time + 1;
                enqueue(&terminated_q, running);
                running = NULL;
            } 
            else if (running->executed_cpu == running->io_start_time) {
                running->state = WAITING;
                running->remaining_io = running->io_duration;
                enqueue(&waiting_q, running);
                running = NULL;
            }
        }

        update_waiting();
        time++;
        sleep(1);
    }
}

void display_result() {
    printf("\nPID\tNAME\tSTATUS\t\tTAT\tWT\n");

    for (int i = 0; i < proc_count; i++) {
        PCB *p = &processes[i];

        if (p->killed) {
            printf("%d\t%s\tKILLED at %d\t-\t-\n",
                   p->pid, p->name, p->completion_time);
        } else {
            int tat = p->completion_time - p->arrival_time;
            int wt = tat - (p->total_cpu_burst + p->io_duration);

            printf("%d\t%s\tDONE\t\t%d\t%d\n",
                   p->pid, p->name, tat, wt);
        }
    }
}

int main() {
    setup();
    load_input();
    run_fcfs();
    display_result();
    return 0;
}
