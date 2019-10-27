#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "tokenizer.h"
#include "fileLine.h"


void logos_diagnosePrintFileLocation(TLTokenizer tk, CXToken token) {
	CXSourceLocation location = clang_getRangeStart(clang_getTokenExtent(tk->translationUnit, token));
	unsigned STR_line = 0, STR_column = 0;
	clang_getSpellingLocation(location, NULL, &STR_line, &STR_column, NULL);
	printf("\033[1m%s:%d:%d:\033[m", tk->filePath, STR_line, STR_column);
}

void logos_diagnosePrintSeverity(TLTokenizer tk, enum CXDiagnosticSeverity severity) {
	char * severities[5][2] = {
		{ "30;0", "ignored" }, // CXDiagnostic_Ignored
		{ "30;1", "note"    }, // CXDiagnostic_Note
		{ "35;1", "warning" }, // CXDiagnostic_Warning
		{ "31;1", "error"   }, // CXDiagnostic_Error
		{ "31;1", "fatal"   }  // CXDiagnostic_Fatal
	};
	printf(" \033[%sm%s\033[m:", severities[severity][0], severities[severity][1]);
}

void logos_diagnosePrintWithSeverity_v(TLTokenizer tk, enum CXDiagnosticSeverity severity, const char * format, va_list ap) {
	int severities[5] = {
		0, // CXDiagnostic_Ignored
		0, // CXDiagnostic_Note
		0, // CXDiagnostic_Warning
		1, // CXDiagnostic_Error
		1  // CXDiagnostic_Fatal
	};
	printf("\033[%dm ", severities[severity]);
    vprintf(format, ap);
	printf("\033[m\n");
}

void logos_diagnosePrintWithSeverity(TLTokenizer tk, enum CXDiagnosticSeverity severity, const char * format, ...) {
	va_list ap;
    va_start(ap, format);
	logos_diagnosePrintWithSeverity_v(tk, severity, format, ap);
    va_end(ap);
}

void logos_diagnosePrintFileLine(TLTokenizer tk, unsigned line) {
	char buffer[256] = { 0 };
	read_file_line(tk->filePath, line, buffer);
	printf("%s\n", buffer);
}

void logos_diagnosePrintAtLocation(TLTokenizer tk, unsigned line, unsigned column, const char * format, ...) {
	char buffer[256] = { 0 };
	read_file_line(tk->filePath, line, buffer);

	memset(buffer + column - 1, 0, 256 - (column - 1));
	for (size_t i = 0; i < column - 1; i++) {
		if (isprint(buffer[i])) {
			buffer[i] = ' ';
		}
	}
	sprintf(buffer + column - 1, "\033[32;1m");

	va_list ap;
    va_start(ap, format);
	int n = vsprintf(buffer + column - 1 + 7, format, ap);
    va_end(ap);
	sprintf(buffer + column - 1 + 7 + n, "\033[m");
	printf("%s\n", buffer);
}

void logos_diagnoseExpectedTokenAtLocation(TLTokenizer tk, CXToken token, CXSourceLocation location, enum CXDiagnosticSeverity severity, const char * expectation) {
	logos_diagnosePrintFileLocation(tk, token);
	logos_diagnosePrintSeverity(tk, severity);

	unsigned STR_line = 0, STR_column = 0;
	clang_getSpellingLocation(location, NULL, &STR_line, &STR_column, NULL);

	if (expectation) {
		logos_diagnosePrintWithSeverity(tk, severity, "expected '%s'", expectation);
	} else {
		logos_diagnosePrintWithSeverity(tk, severity, "expected expression");
	}
	logos_diagnosePrintFileLine(tk, STR_line);
	logos_diagnosePrintAtLocation(tk, STR_line, STR_column, "^");
	if (expectation) {
		logos_diagnosePrintAtLocation(tk, STR_line, STR_column, expectation);
	}
}

void logos_diagnoseExpectedToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation) {
	CXSourceRange range = clang_getTokenExtent(tk->translationUnit, token);
	logos_diagnoseExpectedTokenAtLocation(tk, token, clang_getRangeStart(range), severity, expectation);
}

void logos_diagnoseExpectedAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation) {
	CXSourceRange range = clang_getTokenExtent(tk->translationUnit, token);
	logos_diagnoseExpectedTokenAtLocation(tk, token, clang_getRangeEnd(range), severity, expectation);
}

void logos_diagnoseToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...) {
	logos_diagnosePrintFileLocation(tk, token);
	logos_diagnosePrintSeverity(tk, severity);

	unsigned STR_line = 0, STR_column = 0;
	clang_getSpellingLocation(
		clang_getRangeStart(
			clang_getTokenExtent(tk->translationUnit, token)
		),
		NULL, &STR_line, &STR_column, NULL);

	va_list ap;
    va_start(ap, format);
	logos_diagnosePrintWithSeverity_v(tk, severity, format, ap);
    va_end(ap);

	logos_diagnosePrintFileLine(tk, STR_line);
	logos_diagnosePrintAtLocation(tk, STR_line, STR_column, "^");
}

void logos_diagnoseAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...) {
	logos_diagnosePrintFileLocation(tk, token);
	logos_diagnosePrintSeverity(tk, severity);

	unsigned STR_line = 0, STR_column = 0;
	clang_getSpellingLocation(
		clang_getRangeEnd(
			clang_getTokenExtent(tk->translationUnit, token)
		),
		NULL, &STR_line, &STR_column, NULL);

	va_list ap;
    va_start(ap, format);
	logos_diagnosePrintWithSeverity_v(tk, severity, format, ap);
    va_end(ap);

	logos_diagnosePrintFileLine(tk, STR_line);
	logos_diagnosePrintAtLocation(tk, STR_line, STR_column, "^");
}
