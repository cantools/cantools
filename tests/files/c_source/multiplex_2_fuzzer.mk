#
# The MIT License (MIT)
#
# Copyright (c) 2018-2019 Erik Moqvist
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#

#

CC = clang
EXE = fuzzer
C_SOURCES = \
	multiplex_2.c \
	multiplex_2_fuzzer.c
CFLAGS = \
	-fprofile-instr-generate \
	-fcoverage-mapping \
	-I. \
	-g -fsanitize=address,fuzzer \
	-fsanitize=signed-integer-overflow \
	-fno-sanitize-recover=all
EXECUTION_TIME ?= 5

all:
	$(CC) $(CFLAGS) $(C_SOURCES) -o $(EXE)
	rm -f $(EXE).profraw
	LLVM_PROFILE_FILE="$(EXE).profraw" \
	    ./$(EXE) \
	    -max_total_time=$(EXECUTION_TIME)
	llvm-profdata merge -sparse $(EXE).profraw -o $(EXE).profdata
	llvm-cov show ./$(EXE) -instr-profile=$(EXE).profdata
	llvm-cov report ./$(EXE) -instr-profile=$(EXE).profdata

