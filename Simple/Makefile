#TOPDIR  := $(shell cd ..; pwd)
#include $(TOPDIR)/Rules.make

DEPENDENCIES := -lpthread
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux
ifeq ($(UNAME), Darwin)
	DEPENDENCIES += -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES += -lrt
endif

CFLAGS = -Wall -Wextra

DYNAMIC_LINK_OPTIONS := -Wl,-rpath /usr/local/lib

APP = simple
STATIC_APP = $(APP)-static

all: $(APP)-static 

$(STATIC_APP): main.c
	$(CC) main.c -o $(STATIC_APP) ../build/libftd2xx.a $(CFLAGS) $(DEPENDENCIES)

clean:
	-rm -f *.o ; rm $(STATIC_APP);