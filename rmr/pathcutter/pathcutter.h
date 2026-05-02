#ifndef RMR_PATHCUTTER_H
#define RMR_PATHCUTTER_H

#include <stddef.h>

void rmr_pc_die(const char *msg);
int rmr_pc_mkdir_p(const char *path);
void *rmr_pc_xmalloc(size_t n);
void rmr_pc_xfree(void *ptr);

#endif
