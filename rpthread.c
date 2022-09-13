// File:	rpthread.c

// List all group member's name: Connor Aleksandrowicz cja142 and Ryan Berardi rtb100
// username of iLab: ilab3
// iLab Server: ilab3.cs.rutgers.edu

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
int i = 0;
rpthread_mutex_t mut;
rpthread_t id = 0;
tcb* m; 
ucontext_t sched;
ucontext_t term;
int finished[200];
tcb* curr;
tcb* front;
tcb* rear;
tcb* front1;
tcb* rear1;
tcb* front2;
tcb* rear2;
tcb* front3;
tcb* rear3;
int qsize = 0;
int qsize1 = 0;
int qsize2 = 0;
int qsize3 = 0;
int init = 0;
int schedType;
struct itimerval timer;
struct sigaction sa;
static void sched_rr();
static void sched_mlfq();
// YOUR CODE HERE
int TestAndSet(int* old_ptr, int new){
	int old = *old_ptr;
	*old_ptr = new;
	return old;
}
void enqueue(tcb* thread, int priority){
	//Queue is empty
	if(priority == 0){
		if(rear == NULL){
			front = rear = thread;
			qsize++;
		}
		//Queue has multiple threads
		else{
			rear->next = thread;
			rear = thread;
			qsize++;
		}
	}else if(priority == 1){
		if(rear1 == NULL){
			front1 = rear1 = thread;
			qsize1++;
		}
		//Queue has multiple threads
		else{
			rear1->next = thread;
			rear1 = thread;
			qsize1++;
		}
	}else if(priority == 2){
		if(rear2 == NULL){
			front2 = rear2 = thread;
			qsize2++;
		}
		//Queue has multiple threads
		else{
			rear2->next = thread;
			rear2 = thread;
			qsize2++;
		}
	}else if(priority == 3){
		if(rear3 == NULL){
			front3 = rear3 = thread;
			qsize3++;
		}
		//Queue has multiple threads
		else{
			rear3->next = thread;
			rear3 = thread;
			qsize3++;
		}
	}
	
}
void handler(){
	curr->state = SCHEDULED;
	if(schedType == 1){
		if(curr->priority < 3){
			curr->priority++;
		}else{
			curr->priority = 0;
		}
	}
	enqueue(curr, curr->priority);
	swapcontext(&curr->context, &sched);
}
tcb* tcb_create(){
	//normal block stuff
	tcb* block = (tcb*) malloc(sizeof(tcb));
	block->tid = id++;
	block->priority = 0;
	block->stack = malloc(STACK_SIZE);
	block->next = NULL;
	//join stuff
	block->joined = NULL;
	block->j = 0;

	block->mutnext = NULL;
	return block;
}

void initMain(){
	m = tcb_create();
	getcontext(&m->context);
	m->context.uc_link = NULL;
	m->context.uc_stack.ss_sp = m->stack;
	m->context.uc_stack.ss_size = STACK_SIZE;
	m->context.uc_stack.ss_flags = 0;
}
void startTimer(){
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	sigaction(SIGPROF, &sa, NULL);
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = TIMESLICE*1000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	setitimer(ITIMER_PROF, &timer, NULL);
}
void disableTimer(){
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	setitimer(ITIMER_PROF, &timer, NULL);
}
void pop(int priority){
	if(priority == 0){
		if(front == NULL){
			printf("Attempted to pop an empty queue");
			exit(1);
		}
		curr = front;
		if(qsize == 1){
			front->next = NULL;
			front = NULL;
			rear->next = NULL;
			rear = NULL;
		}else{
			front = front->next;
		}
		if(front == NULL) rear = NULL;
		//have to clean up popped thread eventually
		qsize--;
	}else if(priority == 1){
		if(front1 == NULL){
			printf("Attempted to pop an empty queue");
			exit(1);
		}
		curr = front1;
		if(qsize1 == 1){
			front1->next = NULL;
			front1 = NULL;
			rear1->next = NULL;
			rear1 = NULL;
		}else{
			front1 = front1->next;
		}
		if(front1 == NULL) rear1 = NULL;
		//have to clean up popped thread eventually
		qsize1--;
	}else if(priority == 2){
		if(front2 == NULL){
			printf("Attempted to pop an empty queue");
			exit(1);
		}
		curr = front2;
		if(qsize2 == 1){
			front2->next = NULL;
			front2 = NULL;
			rear2->next = NULL;
			rear2 = NULL;
		}else{
			front2 = front2->next;
		}
		if(front2 == NULL) rear2 = NULL;
		//have to clean up popped thread eventually
		qsize2--;
	}else if(priority == 3){
		if(front3 == NULL){
			printf("Attempted to pop an empty queue");
			exit(1);
		}
		curr = front3;
		if(qsize3 == 1){
			front3->next = NULL;
			front3 = NULL;
			rear3->next = NULL;
			rear3 = NULL;
		}else{
			front3 = front3->next;
		}
		if(front3 == NULL) rear3 = NULL;
		//have to clean up popped thread eventually
		qsize3--;
	}
}
static void schedule() {
// schedule policy
	if(schedType == 0){
		sched_rr();
	}else if(schedType == 1){
		sched_mlfq();
	}
	free(sched.uc_stack.ss_sp);
	free(term.uc_stack.ss_sp);
	if(m != NULL){
		free(m->stack);
		free(m);
	}
}

/* Round Robin (RR) scheduling algorithm */
static void sched_rr() {
	if(qsize > 0){
		pop(0);
		curr->state = RUNNING;
		startTimer();
		setcontext(&curr->context);
	}
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	if(qsize > 0){
		pop(0);
		curr->state = RUNNING;
		startTimer();
		setcontext(&curr->context);
	}else if(qsize1 > 0){
		pop(1);
		curr->state = RUNNING;
		startTimer();
		setcontext(&curr->context);
	}else if(qsize2 > 0){
		pop(2);
		curr->state = RUNNING;
		startTimer();
		setcontext(&curr->context);
	}else if(qsize3 > 0){
		pop(3);
		curr->state = RUNNING;
		startTimer();
		setcontext(&curr->context);
	}
}

void initSched(){
	//Initialize global sched and term
	//makes sched context
	schedule();
	getcontext(&sched);
	sched.uc_link = NULL;
	sched.uc_stack.ss_sp = malloc(STACK_SIZE);
	sched.uc_stack.ss_size = STACK_SIZE;
	sched.uc_stack.ss_flags = 0;
	makecontext(&sched, (void*)schedule, 0);
	//makes term context
	getcontext(&term);
	term.uc_link = NULL;
	term.uc_stack.ss_sp = malloc(STACK_SIZE);
	term.uc_stack.ss_size = STACK_SIZE;
	term.uc_stack.ss_flags = 0;
	makecontext(&term, (void*)rpthread_exit, 1, NULL);
	//Initialize for safety
	front = NULL;
	rear = NULL;
	curr = NULL;
}
/* create a new thread */
int rpthread_create(rpthread_t * thread, pthread_attr_t * attr, 
                      void *(*function)(void*), void * arg) {
		#ifndef MLFQ
	// Choose RR
     schedType = 0;
#else 
	// Choose MLFQ
     schedType = 1;
#endif
		if(init == 0){
		   initSched();
		   init = 1;
		   initMain();
		   //initTimer();
	   }
       // create Thread Control Block
       	   *thread = id;
	   tcb* t = tcb_create(thread);
       // create and initialize the context of this thread
	   getcontext(&t->context);
	   t->context.uc_link = (&term);
	   t->context.uc_stack.ss_sp = t->stack;
	   t->context.uc_stack.ss_size = STACK_SIZE;
	   t->context.uc_stack.ss_flags = 0;
	   makecontext(&t->context, (void*)function, 1, arg);
       // allocate space of stack for this thread to run
       // after everything is all set, push this thread int
       // YOUR CODE HERE
	   if(curr == NULL){
		   enqueue(m, m->priority);
		   enqueue(t, t->priority);
		   if(schedType == 1 && m->priority < 3) m->priority++;
		   else if(schedType == 1 && m->priority >= 3) m->priority = 0;
		   swapcontext(&(m->context), &sched);
	   }
	   else if(curr->tid == 0){
		   enqueue(m, m->priority);
		   enqueue(t, t->priority);
		   if(schedType == 1 && m->priority < 3) m->priority++;
		   else if(schedType == 1 && m->priority >= 3) m->priority = 0;
		   swapcontext(&(m->context), &sched);
	   }
	   else{
		   enqueue(t, t->priority);
		   setcontext(&sched);
	   }
    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {
	
	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context

	//Makes sure the timer does not go off in yield
	//yields thread
	disableTimer();
	curr->state = SCHEDULED;
	enqueue(curr, curr->priority);
	swapcontext(&curr->context, &sched);

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	disableTimer();
	finished[curr->tid] = 1;
	//Makes sure the timer does not go off in rpthread_exit
	//exits thread
	int leaving = curr->priority;
	if(curr == NULL){
		setcontext(&sched);
	}
	free(curr->stack);
	if(curr->joined != NULL){
		curr->joined->j = 0;
	}
	free(curr);
	curr = NULL;
	if(qsize == 0 && leaving == 0){
		front = NULL;
		rear = NULL;
	}else if(qsize1 == 0 && leaving == 1){
		front1 = NULL;
		rear1 = NULL;
	}else if(qsize2 == 0 && leaving == 2){
		front2 = NULL;
		rear2 = NULL;
	}else if(qsize3 == 0 && leaving == 3){
		front3 = NULL;
		rear3 = NULL;
	}
	setcontext(&sched);
	// YOUR CODE HERE
};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {
	if(finished[thread] == 1){
		return 0;
	}
	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread
	
	tcb* ptr = front;
	for(int i = 0; i < qsize; i++){
		if(ptr->tid == thread){
			break;
		}
		ptr = ptr->next;
	}
	if(schedType == 0 &&ptr->tid != thread){
		printf("Did not find thread\n");
		exit(1);
	}else if(schedType == 1 && ptr == NULL){
		ptr = front1;
		for(int i = 0; i < qsize1; i++){
			if(ptr->tid == thread){
				break;
			}
			ptr = ptr->next;
		}
		if(ptr == NULL){
			ptr = front2;
			for(int i = 0; i < qsize2; i++){
				if(ptr->tid == thread){
					break;
				}
				ptr = ptr->next;
			}
			if(ptr == NULL){
				ptr = front3;
				for(int i = 0; i < qsize3; i++){
					if(ptr->tid == thread){
						break;
					}
					ptr = ptr->next;
				}
				if(ptr == NULL){
					printf("Thread to be joined doesn't exist\n");
					exit(1);
				}
			}
		}
	}
	ptr->joined = curr;
	curr->j = 1;
	while(curr->j != 0){
		enqueue(curr, curr->priority);
		if(schedType == 1 && curr->priority < 3) curr->priority++;
		swapcontext(&curr->context, &sched);
	}
	// YOUR CODE HERE
	//program ends if main returns
	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, 
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex
	disableTimer();
	mutex->owner = NULL;
	mutex->flag = 0;
	if(init != 0) startTimer();
	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //  
        // context switch to the scheduler thread
		getcontext(&curr->context);
		disableTimer();
		while((TestAndSet(&(mutex->flag), 1) == 1)){
			setcontext(&sched);
			//do nothing
		}
		mutex->flag = 1;
		mutex->owner = curr;
		startTimer();
        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again. 
	// Put threads in block list to run queue 
	// so that they could compete for mutex later.
	disableTimer();
	mutex->owner = NULL;
	mutex->flag = 0;
	startTimer();
	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

	return 0;
};

