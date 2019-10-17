#ifndef LOGOS_COMMON_H
#define LOGOS_COMMON_H

#define LOGOS_MAJOR_VERSION 2
#define LOGOS_MINOR_VERSION 0
#define LOGOS_MINOR_PATCH 0

#if !defined(LOGOS_EXTERN)
# if defined(__cplusplus)
#  define LOGOS_EXTERN extern "C"
#  define LOGOS_EXTERN_BEGIN extern "C" {
#  define LOGOS_EXTERN_END }
# else
#  define LOGOS_EXTERN extern
#  define LOGOS_EXTERN_BEGIN
#  define LOGOS_EXTERN_END
# endif
#endif

#define LOGOS_Enum(NAME)    typedef enum NAME NAME; enum NAME
#define LOGOS_Struct(NAME)  typedef struct NAME NAME; struct NAME
#define LOGOS_StructAndPointer(NAME, POINTER_NAME)  typedef struct NAME NAME; typedef NAME * POINTER_NAME; struct NAME

LOGOS_Enum(TLDirectiveType) {
	TLDirectiveTypeOptional = 1 << 7,
	/*
	 * Function Arguments
	 *
	 * Syntax:
	 *   %XXX(...)
	 *
	 * Examples:
	 *   %config
	 *   %c
	 *   %class (deprecated)
	 *
	 */
	TLDirectiveTypeFunctionArguments = 1 << 0,
	/*
	 * Function Body
	 *
	 * Syntax:
	 *   %XXX { ... }
	 *
	 * Examples:
	 *   %ctor
	 *   %dtor
	 */
	TLDirectiveTypeFunctionBody = 1 << 1,
	/*
	 * Function Arguments Optional
	 *
	 * Syntax:
	 *   %XXX(...)
	 *   %XXX
	 *
	 * Examples:
	 *   %init
	 *   %orig
	 *   %new
	 *   %log
	 *
	 */
	TLDirectiveTypeFunctionArgumentsOptional = TLDirectiveTypeFunctionArguments | TLDirectiveTypeOptional,
	/*
	 * Function Arguments and Body
	 *
	 * Syntax:
	 *   %XXX(...) { ... }
	 *
	 * Examples:
	 *   %hookf
	 *
	 */
	TLDirectiveTypeFunctionArgumentsAndBody = TLDirectiveTypeFunctionArguments | TLDirectiveTypeFunctionBody,
	/*
	 * Function Identifier
	 * Syntax:
	 *
	 *   %XXX ...
	 *
	 * Examples:
	 *   %hook
	 *   %group
	 *
	 */
	TLDirectiveTypeIdentifier = 1 << 2,
	/*
	 * Function Subclass
	 *
	 * Syntax:
	 *   %XXX ... : ... <...>
	 *
	 * Examples:
	 *   %subclass
	 *
	 */
	TLDirectiveTypeSubclass = 1 << 3,
	/*
	 * Function Subclass
	 *
	 * Syntax:
	 *   %XXX (...) ...;
	 *
	 * Examples:
	 *   %property
	 *
	 */
	TLDirectiveTypeProperty = 1 << 4,
};

#endif /* LOGOS_COMMON_H */
