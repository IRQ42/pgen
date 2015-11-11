# Makefile ################################################################### 

CC= clang 
STD= -std=c99
OPT= -O3
DEBUG=
WARN= -Wall -Werror -Wextra -pedantic
CFLAGS= $(STD) $(OPT) $(DEBUG) $(WARN)
LDFLAGS=
LIBS=
INCLUDES=

SRCS= main.c alloc.c stack.c charset.c info.c
OBJS= $(SRCS:.c=.o)
TARGET= pgen
INSTALL_DIR= /usr/local/bin

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES)	\
		-o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

.c.o: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


.PHONY: all debug clean install

all: $(TARGET)

debug: DEBUG= -ggdb3
debug: OPT= $(DEBUG)
debug: $(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET) 

install: $(TARGET)
	strip --strip-unneeded $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)/$(TARGET)
