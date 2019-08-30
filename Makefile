EXEC=OGSSim
#CM_FLAGS=-DUSE_STATIC_LIBRARIES=ON -DUSE_MPI_BOOST=OFF -DUSE_PYTHON_BINDING=OFF -DUSE_STATIC_GLOG=OFF -DUSE_TINYXML=ON -DUSE_TINYXML_STATIC=ON
CM_FLAGS=-DUSE_STATIC_LIBRARIES=OFF -DUSE_MPI_BOOST=ON -DUSE_PYTHON_BINDING=ON -DUSE_STATIC_GLOG=OFF -DUSE_TINYXML=ON -DUSE_TINYXML_STATIC=OFF

.PHONY: all mrproper clean docs debug release utest mpi unlog

all: debug

utest:
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Utest ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

debug:
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Debug ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

release:
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Release ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

mpidbg:
	OMPI_CXX=clang++
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Mpidbg -DCMAKE_CXX_COMPILER=mpicxx ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

mpi:
	OMPI_CXX=clang++
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Mpi -DCMAKE_CXX_COMPILER=mpicxx ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

unlog:
	rm -f log/*

mrproper:
	$(MAKE) -C src clean
	rm -f $(EXEC)
