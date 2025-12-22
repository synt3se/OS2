CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
LDFLAGS = -pthread

SRCDIR = src
OBJDIR = obj

TARGET = cp_r
SRCS = main.c common.c utils.c copier.c
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
HEADERS = $(SRCDIR)/common.h $(SRCDIR)/utils.h $(SRCDIR)/copier.h

.PHONY: all clean test gen_testdir benchmark

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

gen_testdir:
	@echo "=== Generating test directory ==="
	@rm -rf test_src
	@mkdir -p test_src
	@for i in 1 2 3; do \
		mkdir -p test_src/level1_$$i/level2_a/level3; \
		mkdir -p test_src/level1_$$i/level2_b; \
	done
	@echo "Small file" > test_src/small.txt
	@dd if=/dev/urandom of=test_src/medium.bin bs=1K count=100 2>/dev/null
	@dd if=/dev/urandom of=test_src/large.bin bs=1M count=10 2>/dev/null
	@for i in 1 2 3; do \
		echo "File $$i" > test_src/level1_$$i/file.txt; \
		dd if=/dev/urandom of=test_src/level1_$$i/data.bin bs=1K count=50 2>/dev/null; \
		echo "Deep a" > test_src/level1_$$i/level2_a/deep.txt; \
		echo "Deep b" > test_src/level1_$$i/level2_b/deep.txt; \
		echo "Deepest" > test_src/level1_$$i/level2_a/level3/deepest.txt; \
	done
	@mkdir -p test_src/many_files
	@for i in $$(seq 1 100); do \
		echo "Content $$i" > test_src/many_files/file_$$i.txt; \
	done
	@ln -sf small.txt test_src/symlink_file
	@ln -sf level1_1 test_src/symlink_dir
	@mkfifo test_src/fifo 2>/dev/null || true
	@mkdir -p test_src/empty
	@echo "spaces" > "test_src/file with spaces.txt"
	@mkdir -p "test_src/dir with spaces"
	@echo "nested" > "test_src/dir with spaces/file.txt"
	@echo "hidden" > test_src/.hidden
	@mkdir -p test_src/.hidden_dir
	@echo "in hidden" > test_src/.hidden_dir/file.txt
	@echo "=== Done ==="
	@echo "Files: $$(find test_src -type f | wc -l)"
	@echo "Dirs:  $$(find test_src -type d | wc -l)"
	@echo "Size:  $$(du -sh test_src | cut -f1)"

test: $(TARGET) gen_testdir
	@mkdir -p test_dst
	@echo ""
	@echo "=== Running cp_r test_src test_dst ==="
	./$(TARGET) test_src test_dst
	@echo ""
	@echo "=== Verifying ==="
	@diff -r test_src test_dst/test_src \
		--exclude='symlink_*' \
		--exclude='fifo' \
		&& echo "OK: All files copied"

benchmark: $(TARGET) gen_testdir
	@mkdir -p test_dst
	@rm -rf test_dst/*
	@echo "=== cp_r ==="
	@bash -c 'time ./$(TARGET) test_src test_dst'
	@rm -rf test_dst/*
	@echo ""
	@echo "cp -R"
	@bash -c 'time cp -R test_src test_dst/'