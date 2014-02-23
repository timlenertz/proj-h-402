TARGET := viewer

CXXFLAGS := -std=c++11

ifeq ($(DEPLOY), 1)
	CXXFLAGS += -O3 -DNDEBUG
else
	CXXFLAGS += -g
endif

CXXFLAGS += -I../dypc/include
LDLIBS += -L../dypc/lib -ldypc -Wl,-rpath=.

PACKAGES := gl glew
LDFLAGS += $(shell pkg-config --libs-only-l --libs-only-L $(PACKAGES))
LDLIBS += $(shell pkg-config --libs-only-other $(PACKAGES))
CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))

CXXFLAGS += $(shell wx-config --cxxflags)
LDLIBS += $(shell wx-config --libs --gl-libs)

CPP := $(shell find ./src -name '*.cc')
OBJ := $(patsubst %.cc,build/%.o,$(CPP))
DEP := $(patsubst %.cc,build/%.d,$(CPP))

FBP := $(wildcard forms/*.fbp)
FBP_CPP := $(patsubst %.fbp,%.cpp,$(FBP))
FBP_HPP := $(patsubst %.fbp,%.h,$(FBP))
OBJ += $(patsubst %.fbp,build/%.o,$(FBP))



all : $(TARGET)
	

$(TARGET) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

build/%.o : %.cc
	$(MKDIR) -p $(dir $@) && \
	$(CXX) $(CXXFLAGS) -c -o $@ $< -MMD
	
build/forms/%.o : forms/%.cpp forms/%.h
	$(MKDIR) -p $(dir $@) && \
	$(CXX) $(CXXFLAGS) -c -o $@ $<

forms/%.cpp forms/%.h : forms/%.fbp
	wxformbuilder -g $<

clean :
	$(RM) -rf build/ && \
	$(RM) -f $(FBP_CPP) $(FBP_HPP)
	
mrproper : clean
	$(RM) -f $(TARGET)


-include $(DEP)
