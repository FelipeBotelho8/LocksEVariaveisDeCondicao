# the compiler
CC = gcc

# compiler flags:
CFLAGS  = -Wall -Werror -D_POSIX_THREAD_SEMANTICS
LDFLAGS = -pthread

# the build target executable:
TARGET = sec-5-1

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)
