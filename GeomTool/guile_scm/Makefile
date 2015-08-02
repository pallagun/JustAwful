# just here to move the scm files to the user install location
SOURCES=$(wildcard *.scm)

LOAD_PATH=~/.guile-load/gt-interface/

all: user-install

user-install: user-clean
	mkdir -p $(LOAD_PATH)
	cp $(SOURCES) $(LOAD_PATH)

clean:
user-clean:
	rm -rf $(LOAD_PATH)

