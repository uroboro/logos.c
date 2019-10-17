# Logos V2 Todo List

Standardize syntax wiki, "Classname"/"Class"/"class" using same notation, make %group/%hook/%subclass close with %end in their own syntax example.

Split tokenizer.c/h into:

- stack object
- diagnoser (output)
- plugin API
- other

## Syntax parser

1. %config ✓

		%config(Key=Value)
	| Key | Possible Values |
	| --- | --- |
	| generator | MobileSubstrate / internal |
	| warnings | none / default / error |
	| dump | yaml |

- %hookf ✓

	```c
	%hookf(rtype, symbolName, args...) {
		/* body */
	}
	```
	```c
	%hookf(rtype, "symbolName", args...) {
		/* body */
	}
	```

- %ctor ✓

	```c
	%ctor {
		/* body */
	}
	```

- %dtor ✓

	```c
	%dtor {
		/* body */
	}
	```

- %group

	```c
	%group Groupname
	/* %hooks */
	%end
	```

- %hook

	```c
	%hook Classname
	/* objc methods */
	%end
	```

- %new ✓

	```c
	%new
	/* objc method */
	```
	```c
	%new(signature)
	/* objc method */
	```

- %subclass

	```c
	%subclass Classname: Superclass <Protocol list>
	/* %properties and methods */
	%end
	```c

- %property ✓

	```c
	%property (nonatomic|assign|retain|copy|weak|strong|getter=...|setter=...) Type name;
	```

- %end

	```c
	```

- %init ✓

	```c
	%init;
	```
	```c
	%init([<class>=<expr>, …]);
	```
	```c
	%init(Group[, [+|-]<class>=<expr>, …]);
	```

- %c

	```c
	%c([+|-]Class)
	```

- %orig ✓

	```c
	%orig
	```
	```c
	%orig(args, …)
	```

- &%orig

	```c
	&%orig
	```

- %log ✓

	```c
	%log;
	```
	```c
	%log([(<type>)<expr>, …]);
	```

- method

	```c
	- (returnType)method {
		/* body */
	}
	```
	```c
	- (returnType)method:(type)argument {
		/* body */
	}
	```
	```c
	- (returnType)method:(type)arg0 name:(type)arg1 {
		/* body */
	}
	```

- function

	```c
	returnType identifier([type name, ...])
	```


## Hierarchy

1. %config (global)
- %hookf (global | group)
- %ctor (global)
- %dtor (global)
- %group (global)
- %hook (global | group)
- %new (hook | subclass)
- %subclass (global | group)
- %property (hook | subclass)
- %end (group | hook | subclass)
- %init (hookf | method | function)
- %c (hookf | method | function)
- %orig (hookf | method)
- &%orig (hookf | method)
- %log (hookf | method | function)
- method (hook | subclass)
- function (global)
