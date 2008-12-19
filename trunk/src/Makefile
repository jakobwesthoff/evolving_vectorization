MYCFLAGS=`pkg-config --cflags cairo libpng12`
MYLDFLAGS=`pkg-config --libs cairo libpng12`

all: evolver

evolver: polygon.o random.o

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) ${MYCFLAGS} $< -o $@


%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) ${MYCFLAGS} ${MYLDFLAGS} $^ -o $@


clean:
	rm -f evolver *.o *.png
