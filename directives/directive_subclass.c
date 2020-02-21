#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	CXToken name;
	CXToken superclass;

	CXToken * protocolList;
	unsigned int num_protocolList;
} Metadata;

static const char logos_directive_name[] = "subclass";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;

		CXToken token;
		if (logos_peekToken(tk, &token)) {
			if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Identifier, NULL)) {
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

		if (logos_peekToken(tk, &token)) {
			if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, ":", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected ':'");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected ':' for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}
		logos_popToken(tk, &token);

		if (logos_peekToken(tk, &token)) {
			if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Identifier, NULL)) {
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
			metadata->superclass = token;
		}

		if (logos_peekToken(tk, &token)) {
			if (logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, "<", NULL)) {
				logos_popToken(tk, &token);

				for (;;) {
					if (logos_peekToken(tk, &token)) {
						if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Identifier, NULL)) {
							logos_lastToken(tk, &token);
							logos_diagnoseExpectedAfterToken(tk, token, CXDiagnostic_Error, "identifier");
							// logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected identifier");
							logos_directive_dispose(tk, metadata);
							return NULL;
						}
					} else {
						logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected identifier for this '%%%s'", logos_directive_name);
						logos_directive_dispose(tk, metadata);
						return NULL;
					}
					logos_popToken(tk, &token);
					if (metadata->num_protocolList == 0) {
						metadata->protocolList = (CXToken *)calloc(1, sizeof(CXToken));
					} else {
						metadata->protocolList = (CXToken *)realloc(metadata->protocolList, (metadata->num_protocolList + 1) * sizeof(CXToken));
					}
					metadata->protocolList[metadata->num_protocolList] = token;
					metadata->num_protocolList++;

					if (logos_peekToken(tk, &token)) {
						if (logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, ">", NULL)) {
							logos_popToken(tk, &token);
							break;
						}
						if (!logos_tokenMatchesKindAndString(tk, token, CXToken_Punctuation, ",", NULL)) {
							logos_lastToken(tk, &token);
							logos_diagnoseExpectedAfterToken(tk, token, CXDiagnostic_Error, "',' or '>'");
							logos_directive_dispose(tk, metadata);
							return NULL;
						}
						logos_popToken(tk, &token);
					} else {
						logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected identifier or ',' for this '%%%s'", logos_directive_name);
						logos_directive_dispose(tk, metadata);
						return NULL;
					}
				}
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

		{
			printf("Subclass Name:\n");
			CXToken token = metadata->name;
			CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
			const char * spelling_str = clang_getCString(spellingString);
			printf("- \033[33m%s\033[m\n", spelling_str);
			clang_disposeString(spellingString);
		}

		{
			printf("Superclass Name:\n");
			CXToken token = metadata->superclass;
			CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
			const char * spelling_str = clang_getCString(spellingString);
			printf("- \033[33m%s\033[m\n", spelling_str);
			clang_disposeString(spellingString);
		}

		if (metadata->num_protocolList > 0) {
			printf("Protocols:\n");
			for (unsigned int index = 0; index < metadata->num_protocolList; index++) {
				CXToken token = metadata->protocolList[index];
				CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
				const char * spelling_str = clang_getCString(spellingString);
				printf("- \033[33m%s\033[m\n", spelling_str);
				clang_disposeString(spellingString);
			}
		}
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		if (metadata->protocolList) {
			free(metadata->protocolList);
		}
		free(metadata);
	}
}

void logos_directive_getInfo_subclass(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
