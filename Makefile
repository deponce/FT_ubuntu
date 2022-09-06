SUBDIRS = \
	Simple 

all: subdirs

subdirs:
	for n in $(SUBDIRS); do "$(MAKE)" -C $$n || exit 1; done

clean:
	rm -f *.o *~
	for n in $(SUBDIRS); do "$(MAKE)" -C $$n clean; done
	
