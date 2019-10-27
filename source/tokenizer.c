#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "dynamic.h"

void logos_loadDirectivesForTokenizer(TLTokenizer tk);


TLTokenizer logos_createTokenizer(const char * filepath) {
	TLTokenizer tk = NULL;
	CXIndex index = clang_createIndex(
		/* excludeDeclarationsFromPCH */ 1,
		/* displayDiagnostics */ 0);
	clang_CXIndex_setGlobalOptions(index, CXGlobalOpt_None);

	const char * cmdArgv[] = { "-Xclang", "-cc1", "-x", "objective-c++" };
	const int cmdArgc = sizeof(cmdArgv) / sizeof(char *);

	struct CXUnsavedFile * unsavedFiles = NULL;
	const unsigned unsavedFilesc = 0;

	CXTranslationUnit tu = clang_createTranslationUnitFromSourceFile(
		index, filepath,
		cmdArgc, cmdArgv,
		unsavedFilesc, unsavedFiles);
	if (tu != NULL) {
		tk = (TLTokenizer)calloc(1, sizeof(struct TLTokenizerImpl));
		if (tk != NULL) {
			strcpy(tk->filePath, filepath);
			tk->translationUnit = tu;
			CXCursor cursor = clang_getTranslationUnitCursor(tu);
			CXSourceRange range = clang_getCursorExtent(cursor);
			clang_tokenize(tu, range, &(tk->tokens), &(tk->num_tokens));

			logos_loadDirectivesForTokenizer(tk);
			// printf("D: Loaded %d directive%s:\n", tk->num_directives, (tk->num_directives == 1) ? "" : "s");
			// for (size_t i = 0; i < tk->num_directives; i++) {
			// 	printf("%s", tk->directives[i].name);
			// 	if (i < tk->num_directives - 1) {
			// 		printf(", ");
			// 	}
			// }
			// printf("\n");

		} else {
			printf("Failed to allocate memory\n");
			clang_disposeTranslationUnit(tk->translationUnit);
		}
	} else {
		printf("Failed to tokenize: \"%s\"\n", filepath);
	}
	clang_disposeIndex(index);

	return tk;
}

void logos_disposeTokenizer(TLTokenizer tk) {
	if (!tk) {
		return;
	}
	clang_disposeTokens(tk->translationUnit, tk->tokens, tk->num_tokens);
	clang_disposeTranslationUnit(tk->translationUnit);
	free(tk);
}

void logos_loadDirectivesForTokenizer(TLTokenizer tk) {
	// Get directive count
	enumerate_symbols_in_executable(^(const char * symbol, void * addr) {
		if (strstr(symbol, "logos_directive_getInfo_")) {
			tk->num_directives++;
		}
	});
	tk->directives = (TLDirectiveImpl *)calloc(tk->num_directives, sizeof(TLDirectiveImpl));

	// Get directives' metadata
	__block unsigned directive_index = 0;
	enumerate_symbols_in_executable(^(const char * symbol, void * addr) {
		if (strstr(symbol, "logos_directive_getInfo_")) {
			void (*getInfo)(TLDirective) = (void (*)(TLDirective))addr;
			getInfo(&tk->directives[directive_index]);
			// ((void (*)(TLDirective))addr)(&tk->directives[directive_index]);
			// TLDirectiveImpl directive = tk->directives[directive_index];
			// printf("D: Registed \033[36m%s\033[m\n", directive.name);
			directive_index++;
		}
	});
}



static const char * const logos_getTokenKindString(CXTokenKind kind) {
	static const char * const CXTokenKindString[] = {
		"Punctuation  ",	// CXToken_Punctuation
		"Keyword      ",	// CXToken_Keyword
		"Identifier   ",	// CXToken_Identifier
		"Literal      ",	// CXToken_Literal
		"Comment      " 	// CXToken_Comment
	};
	return CXTokenKindString[(int)kind];
}

void logos_dumpToken(CXTranslationUnit tu, unsigned int index, CXToken token, int useColors) {
	if (!tu) {
		return;
	}
	CXTokenKind kind = clang_getTokenKind(token);
	CXString spellingString = clang_getTokenSpelling(tu, token);
	const char * spelling_str = clang_getCString(spellingString);

	CXSourceLocation location = clang_getTokenLocation(tu, token);

	CXSourceRange range = clang_getTokenExtent(tu, token);
	CXSourceLocation rangeStart = clang_getRangeStart(range);
	CXSourceLocation rangeEnd = clang_getRangeEnd(range);

	// location == rangeStart
	#define LOGOS_GET_FLCO(LOCATION, ID) \
		CXFile ID ## _file = NULL; \
		unsigned ID ## _line = 0, ID ## _column = 0, ID ## _offset = 0; \
		clang_getSpellingLocation(LOCATION, &(ID ## _file), \
			&(ID ## _line), &(ID ## _column), \
			&(ID ## _offset));

	LOGOS_GET_FLCO(location, LOC);
	LOGOS_GET_FLCO(rangeStart, STR);
	LOGOS_GET_FLCO(rangeEnd, STP);

	#undef LOGOS_GET_FLCO

	printf(useColors ?
		"%03d > [\033[31m%03d\033[m|\033[32m%03d\033[m|\033[33m%03d\033[m] - [\033[34m%03d\033[m|\033[35m%03d\033[m|\033[36m%03d\033[m]->[\033[34m%03d\033[m|\033[35m%03d\033[m|\033[36m%03d\033[m] T:\033[31;1m(%02d)-%s\033[m \033[32;1m%s\033[m\n" :
		"%03d > [%03d|%03d|%03d] - [%03d|%03d|%03d]->[%03d|%03d|%03d] T:(%02d)-%s %s\n",
		index,
		LOC_line, LOC_column, LOC_offset,
		STR_line, STR_column, STR_offset,
		STP_line, STP_column, STP_offset,
		(int)kind,
		logos_getTokenKindString(kind),
		spelling_str
	);
	clang_disposeString(spellingString);
}

void logos_dumpTokens(TLTokenizer tk, int useColors) {
	if (!tk) {
		return;
	}
	printf(useColors ?
		"### > [\033[31mLLL\033[m|\033[32mCCC\033[m|\033[33mOOO\033[m] - [\033[34mLLL\033[m|\033[35mCCC\033[m|\033[36mOOO\033[m]->[\033[34mLLL\033[m|\033[35mCCC\033[m|\033[36mOOO\033[m] T:\033[31;1m(##)-***********\033[m   \033[32;1mVALUE\033[m\n" :
		"### > [LLL|CCC|OOO] - [LLL|CCC|OOO]->[LLL|CCC|OOO] T:(##)-***********   VALUE\n");
	printf("-----------------------------------------------------------------------------\n");
	CXTranslationUnit tu = tk->translationUnit;
	for (unsigned int i = 0; i < tk->num_tokens; ++i) {
		const CXToken token = tk->tokens[i];
		logos_dumpToken(tu, i, token, useColors);
	}
}



int logos_peekTokenAtIndex(TLTokenizer tk, CXToken * token, unsigned idx) {
	if (!tk || !token) {
		return 0;
	}
	if (idx >= tk->num_tokens) {
		return 0;
	}
	*token = tk->tokens[idx];
	return 1;
}

int logos_peekToken(TLTokenizer tk, CXToken * token) {
	return logos_peekTokenAtIndex(tk, token, tk->offset);
}

int logos_popToken(TLTokenizer tk, CXToken * token) {
	int r = logos_peekToken(tk, token);
	if (r) {
		tk->offset++;
	}
	return r;
}

int logos_lastToken(TLTokenizer tk, CXToken * token) {
	return logos_peekTokenAtIndex(tk, token, tk->offset - 1);
}

void logos_rewindTokens(TLTokenizer tk) {
	if (!tk) {
		return;
	}
	tk->offset = 0;
}



int logos_directiveIndexForToken(TLTokenizer tk, CXToken token) {
	CXTokenKind kind = clang_getTokenKind(token);
	if (kind != CXToken_Identifier && kind != CXToken_Keyword) {
		return -1;
	}

	CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
	const char * spelling_str = clang_getCString(spellingString);

	int ret = -1;
	for (int i = 0; i < tk->num_directives; i++) {
		if (!strcmp(spelling_str, tk->directives[i].name)) {
			ret = i;
			break;
		}
	}

	clang_disposeString(spellingString);
	return ret;
}

TLDirective logos_directiveAtIndex(TLTokenizer tk, int directiveIndex) {
	if (!tk || directiveIndex < 0 || directiveIndex >= tk->num_directives) {
		return NULL;
	}

	return &tk->directives[directiveIndex];
}

void * logos_parseDirective(TLTokenizer tk, int directiveIndex, CXToken percentageToken) {
	TLDirective directive = logos_directiveAtIndex(tk, directiveIndex);
	void * metadata = NULL;
	if (directive) {
		metadata = directive->parse(tk, percentageToken);
	}
	return metadata;
}

void logos_describeDirective(TLTokenizer tk, void * metadata) {
	unsigned int directiveIndex = ((TLMetadata)metadata)->index;
	TLDirective directive = logos_directiveAtIndex(tk, directiveIndex);
	if (directive) {
		directive->describe(tk, metadata);
	}
}

void logos_disposeDirective(TLTokenizer tk, void * metadata) {
	unsigned int directiveIndex = ((TLMetadata)metadata)->index;
	TLDirective directive = logos_directiveAtIndex(tk, directiveIndex);
	if (directive) {
		directive->dispose(tk, metadata);
	}
}



int logos_checkKindAndStringOfToken(TLTokenizer tk, CXToken token, CXTokenKind target_kind, ...) {
	if (clang_getTokenKind(token) != target_kind) {
		return 0;
	}

	int r = 1;

	CXString tokenString = clang_getTokenSpelling(tk->translationUnit, token);
	const char * token_str = clang_getCString(tokenString);

	va_list ap;
	char * target_string;
	for (va_start(ap, target_kind); (target_string = va_arg(ap, char *)) != NULL;) {
		if (strcmp(token_str, target_string)) {
			r = 0;
			break;
		}
	}
	va_end(ap);

	clang_disposeString(tokenString);

	return r;
}



CXToken logos_openParenthesisToken = {{0}};

TLArgumentsParseEnum logos_parseFunctionArguments(TLTokenizer tk, CXToken * * * _argumentList, unsigned int * * _num_argumentList, unsigned int * _num_argumentCount) {
	if (!(_argumentList && _num_argumentList && _num_argumentCount)) {
		return TLArgumentsParseMissingArgumentsError;
	}

	CXToken token;
	if (logos_peekToken(tk, &token)) {
		if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {
			return TLArgumentsParseUnexpectedTokenError;
		}
		logos_openParenthesisToken = token;
	}

	__block CXToken * * argumentList = (CXToken * *)calloc(1, sizeof(CXToken *));
	__block unsigned int * num_argumentList = (unsigned int *)calloc(1, sizeof(unsigned int));
	__block unsigned int num_argumentCount = 0;

	#define BLOCK(R, N, A) R (^N)(A) = ^(A)
	BLOCK(void, addToCurrentList, CXToken token) {
		unsigned int argumentIndex = num_argumentCount;
		unsigned int index = num_argumentList[argumentIndex];

		if (index == 0) {
			argumentList[argumentIndex] = (CXToken *)calloc(1, sizeof(CXToken));
		} else {
			argumentList[argumentIndex] = (CXToken *)realloc(argumentList[argumentIndex], (index + 1) * sizeof(CXToken));
		}
		argumentList[argumentIndex][index] = token;
		num_argumentList[argumentIndex]++;
	};

	BLOCK(void, changeToNextList, void) {
			unsigned int argumentIndex = num_argumentCount + 1;

			argumentList = (CXToken * *)realloc(argumentList, (argumentIndex + 1) * sizeof(CXToken *));
			argumentList[argumentIndex] = NULL;

			num_argumentList = (unsigned int *)realloc(num_argumentList, (argumentIndex + 1) * sizeof(unsigned int));
			num_argumentCount++;
	};

	BLOCK(void, finishList, void) {
		// Ignore empty lists
		// TODO: change to list of stacks (as a class), reusable for arguments lists as well as the tokenizer itself
		if (num_argumentList[num_argumentCount]) {
			num_argumentCount++;
		}
	};
	#undef BLOCK

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
		} else if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, ",", NULL)) {
			if (depth == 1) {
				changeToNextList();
			} else {
				addToCurrentList(token);
			}
		} else {
			addToCurrentList(token);
		}
	}
	if (depth != 0) {
		logos_disposeFunctionArguments(tk, argumentList, num_argumentList, num_argumentCount);
		return TLArgumentsParseOpenParenthesisError;
	}

	*_argumentList = argumentList;
	*_num_argumentList = num_argumentList;
	*_num_argumentCount = num_argumentCount;

	return TLArgumentsParseSuccess;
}

void logos_disposeFunctionArguments(TLTokenizer tk, CXToken * * argumentList, unsigned int * num_argumentList, unsigned int num_argumentCount) {
	if (!(argumentList && num_argumentList && num_argumentCount != 0)) {
		return;
	}

	for (unsigned int argumentIndex = 0; argumentIndex < num_argumentCount; argumentIndex++) {
		free(argumentList[argumentIndex]);
	}
	free(argumentList);
	free(num_argumentList);
}



void logos_analyseTokens(TLTokenizer tk) {
	if (!tk) {
		return;
	}

	int errorCount = 0;

	// CXTranslationUnit tu = tk->translationUnit;
	for (CXToken token; logos_popToken(tk, &token);) {
		if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "%", NULL)) {
			continue;
		}

		CXToken percentageToken = token;

		if (logos_peekToken(tk, &token)) {
			int directiveIndex = logos_directiveIndexForToken(tk, token);
			if (directiveIndex == -1) {
				// CXString tokenString = clang_getTokenSpelling(tk->translationUnit, token);
				// const char * token_str = clang_getCString(tokenString);
				// printf("D: \033[36m%s\033[m is not a registered directive.\n", token_str);
				// clang_disposeString(tokenString);
				continue;
			}
			// Now that we know it is a '%' followed by a registered directive token, pop it
			logos_popToken(tk, &token);

			#pragma mark - Begin processing directive

			// Pass control to appropriate token
			void * metadata = logos_parseDirective(tk, directiveIndex, percentageToken);
			if (metadata) {
				((TLMetadata)metadata)->index = directiveIndex;

				tk->metadata = (void **)realloc(tk->metadata, (tk->num_metadata + 1) * sizeof(void *));
				tk->metadata[tk->num_metadata] = metadata;
				tk->num_metadata++;
			} else {
				errorCount++;
			}

			#pragma mark - End processing directive
		}
	}

	if (errorCount > 0) {
		printf("%d error%s generated.\n", errorCount, (errorCount == 1) ? "" : "s");
	} else {
		for (size_t i = 0; i < tk->num_metadata; i++) {
			void * metadata = tk->metadata[i];
			logos_describeDirective(tk, metadata);
			logos_disposeDirective(tk, metadata);
		}
	}
}
