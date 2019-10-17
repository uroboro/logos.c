#ifndef LOGOS_STACK_H
#define LOGOS_STACK_H

#include <clang-c/Index.h>
#include "tlcommon.h"

LOGOS_EXTERN_BEGIN

LOGOS_StructAndPointer(TLStackImpl, TLStack) {
	CXToken * tokens;
	unsigned int num_tokens;
	unsigned int offset;
	unsigned char copy;
};

TLStack logos_stack_create(CXToken * tokens, unsigned count);

TLStack logos_stack_createCopying(CXToken * tokens, unsigned count);

void logos_stack_dispose(TLStack stack);

int logos_stack_pushToken(TLStack stack, CXToken token);

int logos_stack_peekToken(TLStack stack, CXToken * token);

int logos_stack_popToken(TLStack stack, CXToken * token);

int logos_stack_lastToken(TLStack stack, CXToken * token);

void logos_stack_rewindTokens(TLStack stack);

LOGOS_EXTERN_END

#endif /* LOGOS_STACK_H */
