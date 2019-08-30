EXEC=OGMDSim

.PHONY: all mrproper clean debug release

all: debug

debug:
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Debug ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

release:
	cd src; cmake ./ -DCMAKE_BUILD_TYPE=Release ${CM_FLAGS}; cd ..;
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

mrproper:
	$(MAKE) -C src clean
	rm -f $(EXEC)
