#if DEBUGFLOWSIZE
extern uint8_t debugflowsize,debugflow[DEBUGFLOWSIZE];
#define DEBUGFLOW(c) if (debugflowsize<(DEBUGFLOWSIZE-1))
debugflow[debugflowsize++]=c
#else
#define DEBUGFLOW(c)
#endif
