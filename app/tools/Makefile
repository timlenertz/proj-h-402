CFLAGS := -std=c11

ifeq ($(DEPLOY), 1)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g
endif

CFLAGS += -I../dypc/include
LDLIBS += -L../dypc/lib -ldypc -Wl,-rpath=.

SRC := $(shell find src/. -name '*.c')
EXE := $(patsubst src/%.c,%,$(SRC))


all : $(EXE)
	

% : src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

clean :
	
	
mrproper : clean
	$(RM) -f $(EXE)
