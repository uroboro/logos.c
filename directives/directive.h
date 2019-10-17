#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include <clang-c/Index.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../source/tokenizer.h"

// Directive name, without percentage character
__used
static const char logos_directive_name[10];

// Validate and retrieve directive metadata
__used
static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken);

// Describe directive metadata
__used
static void logos_directive_describe(TLTokenizer tk, void * _metadata);

// Dispose directive metadata
__used
static void logos_directive_dispose(TLTokenizer tk, void * _metadata);

#endif /* DIRECTIVE_H */
