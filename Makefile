CC = gcc
CFLAGS = -Wall -std=gnu99


SRC_DIR = src/
BIN_DIR = bin/

ipfixed:
	$(CC) $(CFLAGS) $(SRC_DIR)ipfixed.c $(SRC_DIR)netflow5.c -o $(BIN_DIR)ipfixed

clean:
	rm -f $(BIN_DIR)ipfixed

