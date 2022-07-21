TARGETS := wp77xx wp76xx 
EXECUTABLES = xmlstarlet xmllint unzip zip
# $(MAKECMDGOALS)

CURDIR = $(shell pwd)

K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $(exec)),some string,$(error "No $(exec) in PATH")))

# AV_DATA := $(shell xmlstarlet sel -t -c '//asset' $(CURDIR)/av-datapoints.xml)
# MAIFEST_DATA := $(shell xmlstarlet sel -t -c '//asset' ${TMPDIR}/manifest.app)
#

.PHONY: all $(TARGETS) pack_%
all: $(TARGETS)

$(TARGETS):
	mkapp -v -t $@ \
		  -i $(LEGATO_ROOT)/interfaces/dataConnectionService \
		  -i $(LEGATO_ROOT)/interfaces/airVantage \
		  -i $(CURDIR) \
		  basicAirVantageDataExchangeComponent.adef

pack_%:
	$(eval TMPDIR := $(shell mktemp -d))
	av-pack -u basicAirVantageDataExchangeComponent.$$(echo $@ | sed s/pack_//gi).update -b _build_basicAirVantageDataExchangeComponent/$$(echo $@ | sed s/pack_//gi)/
	unzip basicAirVantageDataExchangeComponent.zip -d "$(TMPDIR)"
	mv basicAirVantageDataExchangeComponent.zip basicAirVantageDataExchangeComponent.ORIG.zip
	$(eval AV_DATA = "`xmlstarlet sel -t -c '//asset' ${CURDIR}/av-datapoints.xml`")
	$(eval MAN = "`xmlstarlet sel -t -c '//asset' ${TMPDIR}/manifest.app`")
	xmlstarlet ed -u "//app:application/capabilities/data/encoding" -v ${MAN}${AV_DATA} $(TMPDIR)/manifest.app | xmlstarlet unesc | xmllint --format - >$(TMPDIR)/manifest.new.app
	mv $(TMPDIR)/manifest.new.app $(TMPDIR)/manifest.app
	cp $(TMPDIR)/manifest.app $(CURDIR)
	zip -j basicAirVantageDataExchangeComponent.zip $(TMPDIR)/manifest.app $(TMPDIR)/basicAirVantageDataExchangeComponent.wp76xx.update
	rm -Rf "$(TMPDIR)"


clean:
	rm -rf _build_* *.update

