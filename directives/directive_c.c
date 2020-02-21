#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	char sigil;
	char * name;
} Metadata;

static const char logos_directive_name[] = "c";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;
		metadata->sigil = '-';

		CXToken token;
		if (logos_popToken(tk, &token)) {
			if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, "(", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected '('");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected '(' for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}


		if (logos_peekToken(tk, &token)) {
			if (logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, ")", NULL)) {
				logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected identifier for this '%%%s'", logos_directive_name);
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		}

		if (logos_popToken(tk, &token)) {
			if (logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, "+", "-", NULL)) {
				CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
				const char * spelling_str = clang_getCString(spellingString);
				metadata->sigil = spelling_str[0];
				clang_disposeString(spellingString);

				if (!logos_popToken(tk, &token)) {
					logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected identifier");
					logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Note, "to match this '%%c'");
					return NULL;
				}
			}

			CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
			const char * spelling_str = clang_getCString(spellingString);
			metadata->name = strdup(spelling_str);
			clang_disposeString(spellingString);
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected identifier for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}

		if (logos_popToken(tk, &token)) {
			if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, ")", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected ')'");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		}

	}

	return metadata;
}

static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_diagnoseToken(tk, metadata->percentageToken, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);

		printf("Class name:\n");
		printf("- \033[33m%c%s\033[m\n", metadata->sigil, metadata->name);
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		free(metadata->name);
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
