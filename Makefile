CC = gcc
CFLAGS = -I./include -L ./lib -Wall -Wextra -Werror
PRJ = my_secmalloc
OBJS = src/my_secmalloc.o
SLIB = lib${PRJ}.a
LIB = lib${PRJ}.so

all: ${LIB}

${LIB} : CFLAGS += -fpic -shared
${LIB} : ${OBJS}

${SLIB}: ${OBJS}

dynamic: CFLAGS += -DDYNAMIC
dynamic: ${LIB}

static: ${SLIB}

clean:
	${RM} .*.swp *~ src/*.o test/*.o

distclean: clean
	${RM} ${SLIB} ${LIB}

build_test: CFLAGS += -DTEST
build_test: ${OBJS} test/test.o libmy_secmalloc.a
	$(CC) -o test/test $^ -Llib -lcriterion -L ./ -lmy_secmalloc

test: build_test
	LD_LIBRARY_PATH=./lib test/test


.PHONY: all clean build_test dynamic test static distclean

%.so:
	$(LINK.c) -shared $^ $(LDLIBS) -o $@

%.a:
	${AR} ${ARFLAGS} $@ $^
