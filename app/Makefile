TARGET := dypc
DEPLOY := 0

CXX := clang++
CXXFLAGS := -std=c++11
MKDIR := mkdir
RM := rm

ifeq ($(DEPLOY), 1)
	CXXFLAGS += -O3 -DNDEBUG
else
	CXXFLAGS += -g
endif

PACKAGES := gl glew sqlite3

CXXFLAGS += -I"$(realpath ./external)"
LDFLAGS += -L"$(realpath ./external)"

LDFLAGS += $(shell pkg-config --libs-only-l --libs-only-L $(PACKAGES))
LDLIBS += $(shell pkg-config --libs-only-other $(PACKAGES))
CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))

CXXFLAGS += $(shell wx-config --cxxflags)
LDLIBS += $(shell wx-config --libs --gl-libs)

LDLIBS += -lm -lz -lhdf5 -lhdf5_cpp

CPP := $(shell find ./src -name '*.cc')
OBJ := $(patsubst %.cc,build/%.o,$(CPP))
DEP := $(patsubst %.cc,build/%.d,$(CPP))

FBP := $(wildcard res/*.fbp)
FBP_CPP := $(patsubst %.fbp,%.cpp,$(FBP))
FBP_HPP := $(patsubst %.fbp,%.h,$(FBP))
OBJ += $(patsubst %.fbp,build/%.o,$(FBP))

ifeq ($(GEANY), 1)
	CXXFLAGS += -fno-caret-diagnostics -fno-diagnostics-fixit-info
endif


all : $(TARGET)
	

externals :
	$(MAKE) -C external/

$(TARGET) : externals $(FBP_CPP) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

build/%.o : %.cc
	$(MKDIR) -p $(dir $@) && $(CXX) $(CXXFLAGS) -c -o $@ $< -MMD
	
build/res/%.o : res/%.cpp res/%.h
	$(MKDIR) -p $(dir $@) && $(CXX) $(CXXFLAGS) -c -o $@ $<

res/%.cpp res/%.h : res/%.fbp
	wxformbuilder -g $<

clean :
	$(RM) -rf build/ && $(MAKE) -C external clean && $(RM) -f $(FBP_CPP) $(FBP_HPP)
	
clean_autosave :
	$(RM) -f $(GEDIT_AUTOSAVE)

mrproper : clean
	$(RM) -f $(TARGET)


-include $(DEP)
