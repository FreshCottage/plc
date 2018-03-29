projectname = plc

src = $(shell ls *.c)
objects = $(patsubst %.c,%.o,$(src))

PTHREADLIB = -lpthread
# CC = gcc
# CC = mips-openwrt-linux-gcc

$(projectname) : $(objects)
		$(CC) -o $(projectname) $(objects) ${PTHREADLIB}
#-ldl

.PHONY : clean install
clean:
	-test -z "$(projectname)" || rm -f $(projectname)
	-rm -f *.o

install:
	test -z "/usr/local/sbin" || /bin/mkdir -p "/usr/local/sbin"
	/usr/bin/install -c $(projectname) '/usr/local/sbin'