DEPLOY := 0

CXX := clang++
CC := clang
MKDIR := mkdir
RM := rm
CP := cp

export


all : make_dypc make_tools
	$(MKDIR) -p dist && \
	$(CP) dypc/lib/libdypc.so dist/ && \
	$(CP) viewer/viewer dist/ && \
	$(CP) -r viewer/res dist/ && \
	$(CP) tools/ply2hdf tools/uniform_stat dist/
	
make_dypc : 
	$(MAKE) -C dypc

make_viewer : dypc
	$(MAKE) -C viewer

make_tools :
	$(MAKE) -C tools

clean :
	$(MAKE) -C dypc clean && \
	$(MAKE) -C viewer clean && \
	$(MAKE) -C tools clean

mrproper : clean
	$(MAKE) -C dypc mrproper && \
	$(MAKE) -C viewer mrproper && \
	$(MAKE) -C tools mrproper && \
	$(RM) -rf dist/

