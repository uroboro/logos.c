#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;
	CXToken name;
} Metadata;

static const char logos_directive_name[] = "hook";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;

		CXToken token;
		if (logos_peekToken(tk, &token)) {
			if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Identifier, NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected identifier");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected identifier for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}

		if (logos_popToken(tk, &token)) {
			metadata->name = token;
		}
	}
	return metadata;
}

static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_diagnoseToken(tk, metadata->percentageToken, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);

		printf("Hook Name:\n");
		printf("- \033[33m");

		CXToken token = metadata->name;
		CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
		const char * spelling_str = clang_getCString(spellingString);
		printf("%s", spelling_str);
		clang_disposeString(spellingString);

		printf("\033[m\n");
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		free(metadata);
	}
}

void logos_directive_getInfo_hook(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
