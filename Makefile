CC = gcc
CFLAGS = -g -Wall -Wextra
HEADERS = main.c functii.c
EXEC = atm

.PHONY = build run clean
build: $(EXEC)

$(EXEC): $(HEADERS)
	$(CC) $(CFLAGS) $(HEADERS) -o $@

run: build
	./$(EXEC)

clean:
	rm -f $(EXEC)