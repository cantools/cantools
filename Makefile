ifeq ($(origin CC), default)
CC = gcc
endif

C_SOURCES := \
	tests/main.c \
	tests/files/c_source/motohawk.c \
	tests/files/c_source/padding_bit_order.c \
	tests/files/c_source/vehicle.c \
	tests/files/c_source/multiplex.c \
	tests/files/c_source/multiplex_2.c \
	tests/files/c_source/floating_point.c \
	tests/files/c_source/no_signals.c \
	tests/files/c_source/choices.c \
	tests/files/c_source/signed.c

CFLAGS_EXTRA := \
	-Wduplicated-branches \
	-Wduplicated-cond \
	-Wjump-misses-init \
	-Wlogical-op \
	-Wnull-dereference \
	-Wrestrict

CFLAGS_EXTRA_CLANG := \
	-fsanitize=address \
	-fsanitize=undefined,nullability \
	-Warray-bounds-pointer-arithmetic \
	-Wassign-enum \
	-Wbad-function-cast \
	-Wcast-align \
	-Wcast-qual \
	-Wchar-subscripts \
	-Wcomma \
	-Wcomment \
	-Wconditional-uninitialized \
	-Wcovered-switch-default \
	-Wdate-time \
	-Wduplicate-decl-specifier \
	-Wduplicate-enum \
	-Wduplicate-method-arg \
	-Wduplicate-method-match \
	-Wembedded-directive \
	-Wempty-translation-unit \
	-Wexpansion-to-defined \
	-Wflexible-array-extensions \
	-Wfloat-conversion \
	-Wfloat-equal \
	-Wfloat-overflow-conversion \
	-Wfloat-zero-conversion \
	-Wformat-non-iso \
	-Wformat-nonliteral \
	-Wformat-pedantic \
	-Wfour-char-constants \
	-Wgnu-anonymous-struct \
	-Wgnu-array-member-paren-init \
	-Wgnu-auto-type \
	-Wgnu-binary-literal \
	-Wgnu-case-range \
	-Wgnu-complex-integer \
	-Wgnu-compound-literal-initializer \
	-Wgnu-conditional-omitted-operand \
	-Wgnu-designator \
	-Wgnu-empty-initializer \
	-Wgnu-empty-struct \
	-Wgnu-flexible-array-initializer \
	-Wgnu-flexible-array-union-member \
	-Wgnu-folding-constant \
	-Wgnu-imaginary-constant \
	-Wgnu-include-next \
	-Wgnu-label-as-value \
	-Wgnu-redeclared-enum \
	-Wgnu-statement-expression \
	-Wgnu-union-cast \
	-Wgnu-zero-line-directive \
	-Wgnu-zero-variadic-macro-arguments \
	-Wheader-hygiene \
	-Widiomatic-parentheses \
	-Wimplicit \
	-Wimplicit-fallthrough \
	-Wloop-analysis \
	-Wmethod-signatures \
	-Wmissing-braces \
	-Wmissing-field-initializers \
	-Wnested-anon-types \
	-Wnewline-eof \
	-Wnull-pointer-arithmetic \
	-Woverlength-strings \
	-Wpointer-arith \
	-Wsign-compare \
	-Wtautological-compare \
	-Wundef \
	-Wuninitialized \
	-Wunreachable-code \
	-Wunreachable-code-aggressive \
	-Wunused-comparison \
	-Wunused-const-variable \
	-Wunused-parameter \
	-Wunused-variable \
	-Wvariadic-macros \
	-Wzero-as-null-pointer-constant \
	-Wzero-length-array

CFLAGS := \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wdouble-promotion \
	-Wfloat-equal \
	-Wformat=2 \
	-Wshadow \
	-Werror
CFLAGS += $(shell $(CC) -Werror $(CFLAGS_EXTRA) -c tests/dummy.c 2> /dev/null && echo $(CFLAGS_EXTRA))
CFLAGS += $(shell $(CC) -Werror $(CFLAGS_EXTRA_CLANG) -c tests/dummy.c 2> /dev/null && echo $(CFLAGS_EXTRA_CLANG))

.PHONY: test
test:
	python2 setup.py test
	python3 setup.py test
	$(MAKE) test-sdist
	env PYTHONPATH=. python2 examples/hello_world.py
	env PYTHONPATH=. python3 examples/hello_world.py
	env PYTHONPATH=. python3 examples/dbc_io/main.py
	env PYTHONPATH=. python3 examples/diagnostics/did.py
	codespell -d $$(git ls-files | grep -v \.kcd | grep -v \.[hc])
	$(MAKE) test-c

.PHONY: test-c
test-c:
	$(CC) \
	    $(CFLAGS) \
	    -std=c99 \
	    -O3 \
	    $(C_SOURCES)
	./a.out

.PHONY: test-sdist
test-sdist:
	rm -rf dist
	python setup.py sdist
	cd dist && \
	mkdir test && \
	cd test && \
	tar xf ../*.tar.gz && \
	cd cantools-* && \
	python setup.py test

.PHONY: release-to-pypi
release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
