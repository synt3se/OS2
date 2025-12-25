CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2
LDFLAGS = -pthread

SRCDIR = src
OBJDIR = obj

TARGET = cp_r
SRCS = main.c common.c utils.c copier.c sync.c
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
HEADERS = $(wildcard $(SRCDIR)/*.h)

.PHONY: all clean test gen_testdir test_error

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)
	rm -rf test_src test_dst