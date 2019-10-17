#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	CXToken * signatureList;
	unsigned int num_signatureList;
} Metadata;

static const char logos_directive_name[] = "new";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;

		CXToken token;
		if (logos_peekToken(tk, &token)) {
			if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {
				// logos_popToken(tk, &token);

				void (^addToCurrentList)(CXToken token) = ^(CXToken token) {
					unsigned int index = metadata->num_signatureList;

					if (index == 0) {
						metadata->signatureList = (CXToken *)calloc(1, sizeof(CXToken));
					} else {
						metadata->signatureList = (CXToken *)realloc(metadata->signatureList, (index + 1) * sizeof(CXToken));
					}
					metadata->signatureList[index] = token;
					metadata->num_signatureList++;
				};

				void (^finishList)(void) = ^(void) {
					// metadata->num_signatureList++;
				};

				unsigned int depth = 0;
				for (CXToken token; logos_popToken(tk, &token);) {
					if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {
						depth++;
						if (depth != 1) {
							addToCurrentList(token);
						}
					} else if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, ")", NULL)) {
						depth--;
						if (depth == 0) {
							finishList();
							break;
						} else {
							addToCurrentList(token);
						}
					} else {
						addToCurrentList(token);
					}
				}
				logos_peekToken(tk, &token);
			}

			if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "+", NULL)
				&& !logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "-", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected '+' or '-'");
				logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Note, "to match this '%%%s'", logos_directive_name);
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

		printf("Signature:\n");
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

void logos_directive_getInfo_new(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
