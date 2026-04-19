CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDFLAGS = -lX11
TARGET  = kbtoggle

$(TARGET): kbtoggle.c
	$(CC) $(CFLAGS) -o $(TARGET) kbtoggle.c $(LDFLAGS)

clean:
	rm -f $(TARGET)

install:
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)
