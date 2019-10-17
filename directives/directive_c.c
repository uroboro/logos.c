#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	CXToken * signatureList;
	unsigned int num_signatureList;
} Metadata;

static const char logos_directive_name[] = "c";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;
#if 0
		CXToken token;
		if (logos_popToken(tk, &token)) {
			if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected '('");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected '(' for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}

		if (logos_popToken(tk, &token)) {
			if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "+", "-", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected '+' or '-'");
				logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Note, "to match this '%%c'");
				return NULL;
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected ')' for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}


		if (logos_popToken(tk, &token)) {
			if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, ")", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected ')'");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		}

#endif
	}

	return metadata;
}

static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_diagnoseToken(tk, metadata->percentageToken, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);

		printf("Class name:\n");
		printf("- \033[33m");
		for (unsigned int index = 0; index < metadata->num_signatureList; index++) {
			CXToken token = metadata->signatureList[index];
			CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
			const char * spelling_str = clang_getCString(spellingString);
			printf("%s", spelling_str);
			if (index < metadata->num_signatureList - 1) {
				printf(" ");
			}
			clang_disposeString(spellingString);
		}
		printf("\033[m\n");
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		if (metadata->signatureList) {
			free(metadata->signatureList);
		}
		free(metadata);
	}
}

void logos_directive_getInfo_c(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
