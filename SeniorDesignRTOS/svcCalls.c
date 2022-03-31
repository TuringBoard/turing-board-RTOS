#include <stdint.h>
#include <stdbool.h>
#include "svcCalls.h"

void yield()
{
    __asm(" SVC #2");
}
void sleep(uint32_t tick)
{
    //step 8
    //createThreads: idle, lengthy, flash4hz
    /*
     * when sleep is called, it triggers svcIsr
     * __asm(" SVC #N"); sleepSvcCall()
     * tick will be pushed into R0 and R0 to xPSR are pushed on the stack by hardware before calling svc
     * in svcISR, N will hit the SLEEP case and
     *      tick will be extracted from R0 and saved in tcb[taskCurrent].tick
     *      the state will be changed to delayed tcb[taskCurrent].state = DELAY
     */
//    putsUart0("ticks: ");
//    printHex(tick);
//    putsUart0("\n");
//    sleepSvcCall();
    __asm(" SVC #3");
}
void wait(int8_t semaphore)
{
    //step 9
    //createthreads: idle, lengthy, flash4hz, oneShot
    /*
     * trigger svcISR with a different N
     * in svcISR
     *  create a new case for WAIT (if N == WAIT)
     *      get the index of the semaphore (passed in semaphore is the index of the semaphore from the semaphores array) from R0
     *      if the semaphores[index].count > 0
     *          decrement count. :count--
     *      else
     *          add the process (pid number of taskCurrent) to semaphores[index].queue and increment queue count
     *          record the address of semaphore[index] or *semaphore[index] in tcb
     *          set the state of the taskCurrent to BLOCKED
     *              tcb[taskCurrent].state = BLOCKED
     *          set pendsv
     *
     */
    __asm(" SVC #4");
}
void post(int8_t semaphore)
{
    /*
     * to test, call: readPb, debounce, idle, lengthy, flash4hz, oneshot
     * call svc with a new number N (5)
     * in svc isr
     *      if(N == 5)
     *          increment semaphores[index].count
     *          if queueSize > 0
     *              make next task in queue ready
     *              delete the task from the queue
     *              decrement queueSize
     *              decrement count
     */
    __asm(" SVC #5");
}
