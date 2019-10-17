#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	CXToken * * argumentList;
	unsigned int * num_argumentList;
	unsigned int num_argumentCount;
} Metadata;

static const char logos_directive_name[] = "init";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;

		CXToken token;
		if (logos_peekToken(tk, &token)) {
			if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {

				TLArgumentsParseEnum r = logos_parseFunctionArguments(tk,
					&metadata->argumentList, &metadata->num_argumentList, &metadata->num_argumentCount);
				switch (r) {
					case TLArgumentsParseSuccess:
						break;
					case TLArgumentsParseMissingArgumentsError:
						// Covered by metadata
						break;
					case TLArgumentsParseUnexpectedTokenError:
						// Covered at beginning of this function
						break;
					case TLArgumentsParseOpenParenthesisError:
						logos_diagnoseToken(tk, token, CXDiagnostic_Error, "missing closing '(' to match this ')'");
						break;
				}
				if (r != TLArgumentsParseSuccess) {
					logos_directive_dispose(tk, metadata);
					return NULL;
				}
			}
		} else {
			logos_diagnoseToken(tk, percentageToken, CXDiagnostic_Error, "expected '(' for this '%%%s'", logos_directive_name);
			logos_directive_dispose(tk, metadata);
			return NULL;
		}
	}
	return metadata;
}

static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_diagnoseToken(tk, metadata->percentageToken, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);

		unsigned int startingIndex = 0;
		if (metadata->num_argumentCount > 0) {
			if (metadata->num_argumentList[0] == 1) {
				printf("Group name:\n");
				CXToken token = metadata->argumentList[0][0];
				CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
				const char * spelling_str = clang_getCString(spellingString);
				printf("- \033[33m%s\033[m\n", spelling_str);
				clang_disposeString(spellingString);
				startingIndex = 1;
			}
		}
		for (unsigned int argumentIndex = startingIndex; argumentIndex < metadata->num_argumentCount; argumentIndex++) {
			printf("Arguments list:\n");
			printf("- \033[33m");
			for (unsigned int index = 0; index < metadata->num_argumentList[argumentIndex]; index++) {
				CXToken token = metadata->argumentList[argumentIndex][index];
				CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
				const char * spelling_str = clang_getCString(spellingString);
				printf("%s", spelling_str);
				if (index < metadata->num_argumentList[argumentIndex] - 1) {
					printf(" ");
				}
				clang_disposeString(spellingString);
			}
			printf("\033[m\n");
		}
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_disposeFunctionArguments(tk, metadata->argumentList, metadata->num_argumentList, metadata->num_argumentCount);
		free(metadata);
	}
}

void logos_directive_getInfo_init(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
