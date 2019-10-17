# Logos v2

## Module information

Each file module consists of:

- Configurators `%config`
- Groups `%` (`_ungrouped` or named)
	- Hooks `%hook`
		- Properties `%property`
		- Methods (`%new` or not)
	- Subclasses `%subclass`
		- Properties `%property`
		- Methods (`%new` or not)
	- Hookfs `%hookf`
	- (`%end`)
- Constructors `%ctor`
- Destructors `%dtor`
- Initializers `%init`

```c
typedef struct TLGroupImpl * TLGroup;
typedef struct TLConstructorImpl * TLConstructor;
typedef struct TLDestructorImpl * TLDestructor;
typedef struct TLInitializerImpl * TLInitializer;

typedef struct TLModule {
	int groupc;
	TLGroup groupv[];
	int tokenc;
	CXToken * tokenv;
} TLModule;

```

## Directive syntax "plugins"

```c
typedef struct Metadata {
	int argc;
	CXToken * argv[];
	int tokenc;
	CXToken * tokenv;
} Metadata;

static const char logos_directiveName[] = "hookf";

void logos_hookf_directiveName(char * name) {
	strcpy(name, logos_directiveName); // maximum 10 characters
}

void * logos_hookf_createMetadata(TLTokenizer tk, CXSourceLocation percentageRangeStart) {
	// if there are warnings, errors:
	logos_diagnoseToken(tk, percentageRangeStart, CXDiagnostic_Note,
		"found directive '%s'", logos_directiveName);

	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	/*
	 * use these functions to get data about this directive:
	 *   - logos_peekToken(tk, &token);
	 *   - logos_popToken(tk, &token);
	 */
	return metadata;
}

void logos_hookf_deleteMetadata(void * metadata) {
	// free internal data
	free(metadata);
}

```


## Output

![](file:///Users/uroboro/Desktop/img1.png)
![](file:///Users/uroboro/Desktop/img2.png)
