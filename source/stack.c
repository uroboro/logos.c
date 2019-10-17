#include "stack.h"
#include <stdlib.h>
#include <string.h>

#pragma mark - Lifecycle

static TLStack logos_stack_createPrivate(CXToken * tokens, unsigned count, int copy) {
	TLStack stack = (TLStack)calloc(1, sizeof(TLStackImpl));
	if (stack) {
		if (copy) {
			stack->tokens = (CXToken *)calloc(count, sizeof(CXToken));
			memcpy(stack->tokens, tokens, count);
		} else {
			stack->tokens = tokens;
		}
		stack->num_tokens = count;
		stack->copy = copy;
	}
	return stack;
}

TLStack logos_stack_create(CXToken * tokens, unsigned count) {
	return logos_stack_createPrivate(tokens, count, 0);
}

TLStack logos_stack_createCopying(CXToken * tokens, unsigned count) {
	return logos_stack_createPrivate(tokens, count, 1);
}

void logos_stack_dispose(TLStack stack) {
	if (stack) {
		if (stack->copy) {
			free(stack->tokens);
			free(stack);
		}
	}
}

#pragma mark - Utility

int logos_stack_pushToken(TLStack stack, CXToken token) {
	CXToken * tmp = (CXToken *)realloc(stack->tokens, (stack->num_tokens + 1) * sizeof(CXToken));
	if (!tmp) {
		return -1;
	}
	stack->tokens = tmp;
	stack->tokens[stack->num_tokens] = token;
	stack->num_tokens++;
	return 0;
}

static int logos_stack_peekTokenAtIndex(TLStack stack, CXToken * token, unsigned idx) {
	if (!stack || !token) {
		return 0;
	}
	if (idx >= stack->num_tokens) {
		return 0;
	}
	*token = stack->tokens[idx];
	return 1;
}

int logos_stack_peekToken(TLStack stack, CXToken * token) {
	return logos_stack_peekTokenAtIndex(stack, token, stack->offset);
}

int logos_stack_popToken(TLStack stack, CXToken * token) {
	int r = logos_stack_peekToken(stack, token);
	if (r) {
		stack->offset++;
	}
	return r;
}

int logos_stack_lastToken(TLStack stack, CXToken * token) {
	if (stack->offset == 0) {
		return 0;
	}
	return logos_stack_peekTokenAtIndex(stack, token, stack->offset - 1);
}

void logos_stack_rewindTokens(TLStack stack) {
	if (!stack) {
		return;
	}
	stack->offset = 0;
}
