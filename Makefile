all:
	$(MAKE) -C src
	if [ ! -d bin ]; then mkdir bin; fi
	mv src/sit bin/

install:
	cp bin/sit /usr/local/bin/

.PHONY: clean

clean:
	$(MAKE) -C src clean
	rm -f bin/sit
