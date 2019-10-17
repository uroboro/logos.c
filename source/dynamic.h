#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "tlcommon.h"

LOGOS_EXTERN
void enumerate_symbols_in_executable(void (^callback)(const char * symbol, void * addr));

#endif /* DYNAMIC_H */
