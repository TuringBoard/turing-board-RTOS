#ifndef SVCCALLS_H_
#define SVCCALLS_H_

void yield();
void sleep(uint32_t tick);
void wait(int8_t semaphore);
void post(int8_t semaphore);

#endif
