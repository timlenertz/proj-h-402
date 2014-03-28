DEPLOY := 1

CXX := clang++
CC := clang
MKDIR := mkdir
RM := rm
CP := cp

export


all : make_dypc make_viewer make_tools
	

dist_dir :
	$(MKDIR) -p dist

make_dypc : dist_dir
	$(MAKE) -C dypc && \
	$(CP) dypc/lib/libdypc.so dist/

make_viewer : dist_dir
	$(MAKE) -C viewer && \
	$(CP) viewer/viewer dist/ && \
	$(CP) -r viewer/res dist/

make_tools : dist_dir
	$(MAKE) -C tools && \
	$(CP) tools/ply2hdf tools/uniform_stat dist/

clean :
	$(MAKE) -C dypc clean && \
	$(MAKE) -C viewer clean && \
	$(MAKE) -C tools clean

mrproper : clean
	$(MAKE) -C dypc mrproper && \
	$(MAKE) -C viewer mrproper && \
	$(MAKE) -C tools mrproper && \
	$(RM) -rf dist/

