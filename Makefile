CC ?= gcc

all: reg_rw

reg_rw: reg_rw.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -rf reg_rw *.o
