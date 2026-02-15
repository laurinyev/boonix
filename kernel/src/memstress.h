#ifndef __MEMSTRESS_H__
#define __MEMSTRESS_H__

#define PAGE_SIZE 4096
#define MAX_PAGES 4096   // adjust to stress your system
#define PAT1 0xAA
#define PAT2 0x55

void memstress_test(void);

#endif //__MEMSTRESS_H__
