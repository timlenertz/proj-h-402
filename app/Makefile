DEPLOY := 0

CXX := clang++
MKDIR := mkdir
RM := rm
CP := cp

export


all : make_dypc make_viewer
	$(MKDIR) -p dist && $(CP) dypc/lib/libdypc.so dist/ && $(CP) viewer/viewer dist/ && $(CP) -r viewer/res dist/
	
make_dypc : 
	$(MAKE) -C dypc

make_viewer : dypc
	$(MAKE) -C viewer

clean :
	$(MAKE) -C dypc clean && $(MAKE) -C viewer clean
