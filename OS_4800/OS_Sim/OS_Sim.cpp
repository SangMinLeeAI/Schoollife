// OS_Sim.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable>

#define TIME_QUANTUM    5

#define PROCESS_READY   0
#define PROCESS_RUN     1
#define PROCESS_BLOCK   2

#define SCHED_IO_REQ            0
#define SCHED_QUANTUM_EXPIRED   1

# define NUMBER_OF_PRIORITY 3 //우선 순위 변수, 낮을수록 우선순위가 높음

volatile int cur_proc;
volatile int memory;

std::condition_variable cv;
std::mutex cv_m;

struct proc_tbl_t {
    int id;
    int priority;
    int state;
    int count; // aging algorithm을 위해 추가한 변수, 프로세스가 실행될때 마다 증가.
    int time_quantum;
    std::thread th;
    std::mutex mu_lock;
    struct proc_tbl_t* prev;
    struct proc_tbl_t* next;
} proc_tbl[10];

struct proc_tbl_t run_q[NUMBER_OF_PRIORITY]; //multiple level scheduler를 구현하기 위해 run_q를 구조체 배열로 선언.
struct proc_tbl_t block_q;

void Put_Tail_Q(proc_tbl_t*, proc_tbl_t*);
proc_tbl_t* Get_Head_Q(proc_tbl_t* head);
void Print_Q(proc_tbl_t*);
void syscall_reqio();
void sys_timer_int();
void sys_scheduler(int why);

void IO_Completion_Interrupt(int id);
void proc_1(int id);
void proc_2(int id);
void proc_3(int id);
void proc_4(int id);
void proc_5(int id);



void proc_1(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 10) syscall_reqio();  // call IO with 10% probability
        else Sleep(10);
    }
}

void proc_2(int id)  // CPU-bound process, Do computation only, No IO Request
{
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        Sleep(10);
    }
}

void proc_3(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=]{return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 15) syscall_reqio();  // call IO with 15% probability
        else Sleep(10);
    }
}

void proc_4(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 20) syscall_reqio();  // call IO with 20% probability
        else Sleep(10);
    }
}

void proc_5(int id)
{
    int r;
    std::unique_lock<std::mutex> lk(cv_m);

    for (;;) {
        cv.wait(lk, [=] {return cur_proc == id; });
        std::cout << id << ' ';
        r = rand() % 100;
        if (r < 5) syscall_reqio();  // call IO with 5% probability
        else Sleep(10);
    }
}

int main()
{
    proc_tbl_t* p;

    //run_q가 구조체 배열이 됨에 때라 for문을 이용해 배열 속 모든 구조체를 초기화함
    for (int i = 0; i < NUMBER_OF_PRIORITY ; i++)
    {
        run_q[i].next = run_q[i].prev = &(run_q[i]);
    }



    block_q.next = block_q.prev = &(block_q);
    cur_proc = -1;
    p = &(proc_tbl[0]);
    p->id = 0;
    p->priority = 0;
    p->count = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(IO_Completion_Interrupt, 0);

//    Print_Q(&run_q);
    //프로세스에 count 변수 추가

    p = &(proc_tbl[1]);
    p->id = 1;
    p->priority = 0;
    p->count = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_1, 1);

    Put_Tail_Q(&run_q[0], p);
//    Print_Q(&run_q);

    p = &(proc_tbl[2]);
    p->id = 2;
    p->priority = 0;
    p->count = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_2, 2);

    Put_Tail_Q(&run_q[0], p);
//    Print_Q(&run_q);

    p = &(proc_tbl[3]);
    p->id = 3;
    p->priority = 0;
    p->count = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_3, 3);

    Put_Tail_Q(&run_q[0], p);
//    Print_Q(&run_q);

    p = &(proc_tbl[4]);
    p->id = 4;
    p->priority = 0;
    p->count =0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_4, 4);

    Put_Tail_Q(&run_q[0], p);

    p = &(proc_tbl[5]);
    p->id = 5;
    p->priority = 0;
    p->count = 0;
    p->state = PROCESS_READY;
    p->th = std::thread(proc_5, 5);

    Put_Tail_Q(&run_q[0], p);
//    Print_Q(&run_q);
    // The member function will be executed in a separate thread

    // Wait for the thread to finish, this is a blocking operation

    // Now Begin Schedule

    sys_timer_int();


    return 0;
}

void syscall_reqio()
{
    // Invoke IO Operation, 
    // Then schedule to other process
    sys_scheduler(SCHED_IO_REQ);
}

void sys_timer_int()
{
    proc_tbl_t* p;

 

    for (;;) {
        /* do Time Service */
        if (cur_proc == -1) {
            sys_scheduler(SCHED_QUANTUM_EXPIRED);
        }
        else {
            p = &(proc_tbl[cur_proc]);
            p->time_quantum--;
            if (p->time_quantum <= 0) 
                sys_scheduler(SCHED_QUANTUM_EXPIRED);
        }
        Sleep(100);
    }
}

void sys_scheduler(int why)
{
    proc_tbl_t* p;

    /* 학생들은 이 함수내에서 코드를 작성해야 한다 */



    //IO request가 들어오면 현재 실행중인 프로세스를 멈추고 block_q에 넣음
    if (why == SCHED_IO_REQ) {
        p = &(proc_tbl[cur_proc]);
        p->state = PROCESS_BLOCK;
        Put_Tail_Q(&(block_q), p);
     
    }
    // IO request가 아니고 현재 실행중인 프로세스가 있을 경우 timer interrupt에 의해 다시 ready_q로 돌아감
    else if (cur_proc != -1) {
        p = &(proc_tbl[cur_proc]);
        p->state = PROCESS_READY;
        //실행이 되었기 때문에 해당 프로세스의 우선순위를 낮춤. 하지만 가장 낮은 우선순위를 가질 경우를 예외 처리함
        if (p->priority<NUMBER_OF_PRIORITY-1) {
            p->priority++;
        }
        Put_Tail_Q(&(run_q[p->priority]), p);
    }

     /*SCHED_IO_REQ*/

    //    Print_Q(&run_q);

    // multiple level scheduler에서 실행하는 큐는 비어있지 않은 큐에서 가장 우선순위가 높은 큐로 지정된다.
    // run_q가 배열로 선언되었기 때문에 반복실행하면서 우선순위가 높은 큐부터 비어있는지 확인하면 된다.
    int num_i =0;
    p = Get_Head_Q(&run_q[num_i]);
    while (p == NULL) {
        num_i++;
        p = Get_Head_Q(&run_q[num_i]);
    }

    cur_proc = p->id;
    p->count = 0; // 실행되었으므로 starvation에서 벗어나 aging algorithm의 도움을 받을 필요가 없어서 count = 0으로 초기화 해준다.
    p->state = PROCESS_RUN;
    p->time_quantum = TIME_QUANTUM;

    cv.notify_all();   // switch to process p->id and run */
}

/* IO Interrupt Generator */
void IO_Completion_Interrupt(int id)
{
    proc_tbl_t* p;
    
   
    for (;;) { 
        /* 학생들은 이곳에 코드를 작성해야 한다 */
        /*aging algorithm*/
        /* 프로세스의 상태가 대기인 경우 IO completion이 일어날때, 즉 주지걱으로 확인해 count를 1씩 증가시킨다*/
        for(int b = 1; b < 6 ; b++){
            if (proc_tbl[b].state == PROCESS_READY) {
                proc_tbl[b].count++;
            }
        }
        /* count값이 일정 이상이고 우선순위가 최상위가 아니면 우선순위를 한단계씩 증가 시켜준다. 실행되면 count =0으로 초기화된다.(line277)*/
        int sum_flag = 0;
        for(int c = 1;  c < 6; c++){
            sum_flag += proc_tbl[c].count;
            if (sum_flag >= 10) {
                for(int c = 1;  c < 6; c++){

                    if (proc_tbl[c].priority == NUMBER_OF_PRIORITY - 1) { proc_tbl[c].priority--; }
                    Put_Tail_Q(&run_q[NUMBER_OF_PRIORITY-2],Get_Head_Q(&run_q[NUMBER_OF_PRIORITY-1]));
                }
        }

        }

        //block_q에서 최상단의 프로세스를 가져온다.
        p = Get_Head_Q(&(block_q));
        //만약 block_q가 비어있지 않으면 ready queue에 넣어준다.
        if (p != NULL) {
            p->state = PROCESS_READY;
            //ready큐에 넣어줄때 우선순위를 한단계 증가 시켜준다.
            if(p->priority >= 1) { p->priority--; }
            Put_Tail_Q(&(run_q[p->priority]), p);
            //학번 이름을 출력한다.
            printf("2021480011 LeeSangMin \n");
        }
        Sleep(500); 
 


      /* Wakeup in every 3 sec */
        
 
    }
}

void Put_Tail_Q(proc_tbl_t *head, proc_tbl_t *item)
{
    (head->mu_lock).lock();
    item->prev = head->prev;
    head->prev->next = item;
    item->next = head;
    head->prev = item;
    (head->mu_lock).unlock();
}

proc_tbl_t* Get_Head_Q(proc_tbl_t* head)
{
    proc_tbl_t *item;

    (head->mu_lock).lock();
    if (head->next == head) {
        (head->mu_lock).unlock();
        return NULL;
    }

    item = head->next;
    item->next->prev = head;
    head->next = item->next;

    (head->mu_lock).unlock();
    return item;
}

void Print_Q(proc_tbl_t* head)
{
    proc_tbl_t* item;

    item = head->next;
    while (item != head) {
        std::cout << item->id << ' ';
        item = item->next;
    }
    std::cout << '\n';
}


// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
