ifeq ($(origin CC),default)
CC = gcc
endif

# C source files
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

CFLAGS := \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wdouble-promotion \
	-Wconversion \
	-Wno-unused-function \
	-Werror
CFLAGS += $(shell $(CC) -Werror $(CFLAGS_EXTRA) -c tests/dummy.c 2> /dev/null && echo $(CFLAGS_EXTRA))

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
