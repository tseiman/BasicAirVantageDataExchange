TARGETS := $(MAKECMDGOALS)

CURDIR = $(shell pwd)

.PHONY: all $(TARGETS)
all: $(TARGETS)

$(TARGETS):
	mkapp -v -t $@ \
		  -i $(LEGATO_ROOT)/interfaces/dataConnectionService \
		  -i $(LEGATO_ROOT)/interfaces/airVantage \
		  -i $(CURDIR) \
		  basicAirVantageDataExchangeComponent.adef


clean:
	rm -rf _build_* *.update

