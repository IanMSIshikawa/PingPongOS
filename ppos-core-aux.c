#include "ppos.h"
#include "ppos-core-globals.h"
#include "ppos-disk-manager.h"


// ****************************************************************************
// Coloque as suas modificações aqui, 
// p.ex. includes, defines variáveis, // estruturas e funções



void before_ppos_init () {
    // put your customization here
#ifdef DEBUG
    printf("\ninit - BEFORE");
#endif
}

void after_ppos_init () {
    // put your customization here
#ifdef DEBUG
    printf("\ninit - AFTER");
#endif
    
}

void before_task_create (task_t *task ) {
#ifdef DEBUG
    printf("\ntask_create - BEFORE - [%d]", task->id);
#endif
}

void after_task_create (task_t *task ) {
#ifdef DEBUG
    printf("\ntask_create - AFTER - [%d]", task->id);
#endif
    
}

void before_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - AFTER- [%d]", taskExec->id);
#endif
    
}

void before_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - BEFORE - [%d -> %d]", taskExec->id, task->id);
#endif
    
}

void after_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - AFTER - [%d -> %d]", taskExec->id, task->id);
#endif
}

void before_task_yield () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - BEFORE - [%d]", taskExec->id);
#endif
}
void after_task_yield () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - AFTER - [%d]", taskExec->id);
#endif
}


void before_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - BEFORE - [%d]", task->id);
#endif
}

void after_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - AFTER - [%d]", task->id);
#endif
}

void before_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - BEFORE - [%d]", task->id);
#endif
    // printf("before taskResume");
    // print_tcb(taskExec);
}

void after_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - AFTER - [%d]", task->id);
#endif
    // printf("after taskResume");
    // print_tcb(taskExec);

}

void before_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - AFTER - [%d]", taskExec->id);
#endif
}

int before_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}


int before_sem_create (semaphore_t *s, int value) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_create (semaphore_t *s, int value) {
    
#ifdef DEBUG
    printf("\nsem_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}


int mutex_create (mutex_t *m)
{
    if (!m)
        return -1; //Failed
    
    m->lock = 0;
    m->owner = NULL;
    return 0;

}


int mutex_lock(mutex_t *m) {
    printf("enter mutex lock\n");
    if (!m)
        return -1; // Error: NULL mutex

    // Print the current state of the lock before attempting to acquire it
    printf("Task %d attempting to acquire mutex. Lock state: %d\n", taskExec->id, m->lock);

    // Attempt to acquire the lock
    while (__atomic_test_and_set(&(m->lock), __ATOMIC_SEQ_CST)) {
        printf("Mutex lock, Task yield. Lock state: %d\n", m->lock);
        task_yield(); // Yield CPU while waiting for the mutex
    }

    // After acquiring the lock
    printf("Task %d acquired mutex.\n", taskExec->id);
    m->owner = taskExec;  // Set the current task as the owner
    printf("exit mutex lock\n");
    return 0; // Lock successfully acquired
}


int mutex_unlock(mutex_t *m) {
    printf("enter mutex unlock\n");
    if (!m || m->owner != taskExec) 
        return -1; // Only the owner can unlock

    m->owner = NULL;

    
    __atomic_clear(&(m->lock), __ATOMIC_SEQ_CST); // Unlock atomically
    printf("Mutex unlock, Lock state: %d\n", m->lock);
    printf("exit mutex unlock\n");
    return 0;
}

int mutex_destroy(mutex_t *m)
{
    if (!m)
        return -1;
    

    m->lock = 0;
    m->owner = NULL; 
    m->lock =-1;

    return 0;
}


int sem_destroy (semaphore_t *s) {
    printf("In destroy\n");

    if (!s || s->active == 0) {
        return -1; // null or already inactive
    }
    
    s->active = 0; //inactive
    
    //mutex_destroy(&s->mutex);


    while (s->queue) //Checks if queue is Null
    { 
        task_t *wakeup = (task_t *)s->queue;
     
        queue_remove((queue_t **)&(s->queue), (queue_t *)wakeup); //removing tasks

        queue_append((queue_t **)&readyQueue, (queue_t *)wakeup);
    }

    printf("Out destroy\n");

    return 0;
}

int sem_create(semaphore_t *s, int value)
{
    s->active = 1;
    s->count = value;
    s->queue =  NULL;

    if (mutex_create(&s->mutex))
        return -1;

    return 0;
}

int sem_down (semaphore_t *s) {
    PPOS_PREEMPT_DISABLE
    if (!s || s->active == 0 ){
        return -1;

    } 

    mutex_lock(&s->mutex);

    s->count -=1;
    if(s->count < 0 ){
        //Suspend running task
        mutex_unlock(&s->mutex);
        task_suspend( taskExec, &s->queue );
        

        printf("Task suspended\n");
        
        
        
        //Execute new task
        PPOS_PREEMPT_ENABLE
        task_yield();
        
        
    } else 
    {
        mutex_unlock(&s->mutex);
    }
    
    PPOS_PREEMPT_ENABLE

    

    return 0;
}

int sem_up (semaphore_t *s) {
    PPOS_PREEMPT_DISABLE
    if (!s || s->active == 0 ){
        return -1;
    } 

    mutex_lock(&s->mutex);

    s->count +=1;

    if (s->count <= 0){
        task_resume(s->queue);
    }


    PPOS_PREEMPT_ENABLE
    
    mutex_unlock(&s->mutex);
    
    return 0;
}



int before_sem_down (semaphore_t *s) {
#ifdef DEBUG
    printf("\nsem_down - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_down (semaphore_t *s) {
#ifdef DEBUG
    printf("\nsem_down - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_up (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_up - BEFORE - [%d]", taskExec->id);
#endif 
    return 0;
}

int after_sem_up (semaphore_t *s) {
    
#ifdef DEBUG
    printf("\nsem_up - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}


int before_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

// Essa função implemeneta o escalonador de requisicoes de 
// leitura/scrita do disco usado pelo gerenciador do disco
// A função implementa a política FCFS.
diskrequest_t* disk_scheduler(diskrequest_t* queue) {
     // FCFS scheduler
    if ( queue != NULL ) {
        PPOS_PREEMPT_DISABLE
        diskrequest_t* request = queue;
        PPOS_PREEMPT_ENABLE
        return request;
    }
    return NULL;
}