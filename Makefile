prefix=/usr/local

all: piHomeEasy
	
piHomeEasy: piHomeEasy.o
	$(CXX) $+ -o $@ -lwiringPi

install: piHomeEasy
	install -m 755 piHomeEasy $(prefix)/bin

uninstall:
	$(RM) $(prefix)/bin/piHomeEasy

clean:
	$(RM) *.o piHomeEasy