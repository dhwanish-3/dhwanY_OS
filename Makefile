.PHONY: all

all:
	$(MAKE) -C expl
	$(MAKE) -C spl
	$(MAKE) -C xfs-interface
	$(MAKE) -C xsm

clean:
	$(MAKE) -C expl clean
	$(MAKE) -C spl clean
	$(MAKE) -C xfs-interface clean
	$(MAKE) -C xsm clean
