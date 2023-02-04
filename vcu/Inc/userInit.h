#ifndef USERINIT_H
#define USERINIT_H
void userInit(void);
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName );
#endif /* end of include guard: USERINIT_H */