TOP=..

include $(TOP)/configure/CONFIG


powerSupplySRC = $(TOP)/src

INC += pv/powerSupplyRecord.h

DBD += powerSupplyRegister.dbd

LIBRARY = powerSupply
LIBSRCS += powerSupplyRecord.cpp
LIBSRCS += powerSupplyRegister.cpp
powerSupply_LIBS += pvDatabase pvAccess nt pvData Com

# shared library ABI version.
SHRLIB_VERSION ?= 4.3.0

PROD_HOST += powerSupplyMain
powerSupplyMain_SRCS += powerSupplyMain.cpp
powerSupplyMain_LIBS += powerSupply
powerSupplyMain_LIBS += pvDatabase
powerSupplyMain_LIBS += pvAccess pvAccessCA pvData Com

PROD_HOST += powerSupplyClient
powerSupplyClient_SRCS += powerSupplyClient.cpp
powerSupplyClient_LIBS += pvaClient pvAccess pvAccessCA pvData ca Com


PROD_HOST += powerSupplyMonitor
powerSupplyMonitor_SRCS += powerSupplyMonitor.cpp
powerSupplyMonitor_LIBS += pvaClient pvAccess pvAccessCA pvData ca Com


include $(TOP)/configure/RULES

