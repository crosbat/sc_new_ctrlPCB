TARGET = simplelink

ifneq ($(TARGET),cooja)
BOARD = launchpad/cc26x2r1
endif

# Read version file
ifndef VERSION
versionFile='version'
VERSION=$(shell cat ${versionFile})
$(info 'Get firmware version... file: ${versionFile}, version: ${VERSION}')
VERSION_major := $(shell echo $(VERSION) | cut -f1 -d.)
VERSION_minor := $(shell echo $(VERSION) | cut -f2 -d.)
VERSION_revision := $(shell echo $(VERSION) | cut -f3 -d.)
$(info "Version: ${VERSION}, Major: ${VERSION_major} Minor: ${VERSION_minor} Revision: ${VERSION_revision}")
endif

CFLAGS += -DSF_SC_VERSION_MAJOR=${VERSION_major}
CFLAGS += -DSF_SC_VERSION_MINOR=${VERSION_minor}
CFLAGS += -DSF_SC_VERSION_PATCH=${VERSION_revision}
CFLAGS += -DUSE_GPRAM=0
CFLAGS += -DUSE_AUXRAM=1
CFLAGS += -g

CONTIKI_PROJECT = app-sc
all: $(CONTIKI_PROJECT)

CONTIKI = ../../modules/thirdparty/sf-contiki-ng

CORE_SDK = $(CONTIKI_CPU)/lib/simplelink_cc13x2_26x2_sdk_4_40_04_04

# Add app specific source files
APP_SOURCEFILES += sf-tsch-timeslot.c
APP_SOURCEFILES += sf-tsch-schedule.c
APP_SOURCEFILES += sf_join.c
APP_SOURCEFILES += sf_joinFramer.c
APP_SOURCEFILES += sf_frameType.c
APP_SOURCEFILES += sf_stateManager.c
APP_SOURCEFILES += sf_joinRequester.c
APP_SOURCEFILES += sf_measSender.c
APP_SOURCEFILES += measHandler.c
APP_SOURCEFILES += sf_callbackHandler.c
APP_SOURCEFILES += sf_configMgmt.c
APP_SOURCEFILES += sf_persistentDataStorage.c
ifneq ($(TARGET), cooja)
APP_SOURCEFILES += flash_interface_internal.c
endif
APP_SOURCEFILES += sf_beaconScan.c
APP_SOURCEFILES += sf_absoluteTime.c
APP_SOURCEFILES += sf_tsch.c
APP_SOURCEFILES += sf_led.c

RF_REGIONS  = ../../modules/sf-rf-regions
CONFIG_MGMT = ../../modules/sf-configMgmt
COMMON      = ../../modules/common
TI_FLASH    = ../../modules/ti/common/cc26xx/flash_interface
JOIN        = ../../modules/sf-join
BEACON_SCAN = ../../modules/sf-beaconScan
ABS_TIM     = ../../modules/sf-absoluteTime
SF_TSCH     = ../../modules/sf-tsch
MEAS        = ../../modules/sf-meas


# RF regions
APP_SOURCEFILES += sf_rf.c

ROOT_PATH  := ../../
SOURCEDIRS += $(ROOT_PATH)

SOURCEDIRS += $(MEAS) $(SF_TSCH) $(ABS_TIM) $(BEACON_SCAN) $(JOIN) $(TI_FLASH) $(COMMON) $(CONFIG_MGMT) $(RF_REGIONS)

PLATFORMS_EXCLUDE = nrf52dk

MAKE_MAC = MAKE_MAC_TSCH
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
