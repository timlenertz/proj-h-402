TARGET := lib/libdypc.so

CXXFLAGS := -std=c++11 -fpic -fvisibility=hidden 
LDFLAGS := -shared
ifeq ($(DEPLOY), 1)
	CXXFLAGS += -O3 -DNDEBUG
else
	CXXFLAGS += -g
endif

PACKAGES := sqlite3

CXXFLAGS += -I"$(realpath ./external)"
LDFLAGS += -L"$(realpath ./external)"

LDLIBS += $(shell pkg-config --libs $(PACKAGES))
CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))

LDLIBS += -lm -lz -lhdf5 -lhdf5_cpp

CPP := $(shell find ./src -name '*.cc')
OBJ := $(patsubst %.cc,build/%.o,$(CPP))
DEP := $(patsubst %.cc,build/%.d,$(CPP))

INTERFACE_H := $(shell find ./src/interface -name '*.h')


all : $(TARGET) includes
	

externals :
	$(MAKE) -C external/

$(TARGET) : externals $(OBJ)
	$(MKDIR) -p lib/ && \
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

includes : $(INTERFACE_H)
	$(MKDIR) -p include/dypc/ && \
	$(CP) $^ include/dypc

build/%.o : %.cc
	$(MKDIR) -p $(dir $@) && \
	$(CXX) $(CXXFLAGS) -c -o $@ $< -MMD

clean :
	$(RM) -rf build/

mrproper : clean
	$(RM) -rf include/ lib/

doc : $(CPP) $(OBJ)
	doxygen Doxyfile

-include $(DEP)
