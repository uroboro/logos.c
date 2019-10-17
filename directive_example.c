#include <clang-c/Index.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../source/tokenizer.h"


// Per-directive ocurrence metadata
typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	// Other information
} Metadata;

// Directive name without percentage character
static const char logos_directive_name[] = "example";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken);
static void logos_directive_describe(TLTokenizer tk, void * _metadata);
static void logos_directive_dispose(TLTokenizer tk, void * _metadata);


// Validate and retrieve directive metadata
static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	// Peek next token
	CXToken token;
	if (logos_peekToken(tk, &token)) {
		// Check if it matches expectations (":" which is a punctuation token)
		if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, ":", NULL)) {
			CXSourceLocation tokenStart = clang_getRangeStart(
				clang_getTokenExtent(tk->translationUnit, token)
			);
			// Display a concise error message
			logos_diagnoseToken(tk, tokenStart, CXDiagnostic_Error,
				"expected ':'");
			// Return NULL to indicate an error
			return NULL;
		}
	}

	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageLocation = percentageLocation;
		/*
		* use these functions to get data about this directive:
		*   - logos_peekToken(tk, &token);
		*   - logos_popToken(tk, &token);
		*/
	}
	return metadata;
}

// Describe directive metadata
static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		// Describe internal data
		logos_diagnoseToken(tk, metadata->percentageLocation, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);
	}
}

// Dispose directive metadata
static void logos_directive_dispose(TLTokenizer tk, void * metadata) {
	if (metadata) {
		// Free internal data
		free(metadata);
	}
}

// Configure directive initializer
void logos_directive_getInfo_example(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
