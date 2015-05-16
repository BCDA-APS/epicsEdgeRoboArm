TOP = ../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

SNC = $(INSTALL_HOST_BIN)/snc$(HOSTEXE)

#  Generate snc main programs (not needed under VxWorks)
SNCFLAGS_DEFAULT += +m -i
SNCFLAGS_vxWorks += -nil-

#  Set to path of valgrind executable if tests should run under valgrind
USE_VALGRIND =

#  Sequence programs and/or object files to create
TESTPROD_HOST += decl
TESTPROD_HOST += delay
TESTPROD_HOST += entry
TESTPROD_HOST += entryOpte
TESTPROD_HOST += entryVar
TESTPROD_HOST += exitOptx
TESTPROD_HOST += optt
TESTPROD_HOST += stop
TESTPROD_HOST += subscript
TESTPROD_HOST += syncq

REGRESSION_TESTS_WITH_DB += bittypes
REGRESSION_TESTS_WITH_DB += evflag
REGRESSION_TESTS_WITH_DB += monitorEvflag
REGRESSION_TESTS_WITH_DB += pvGet
REGRESSION_TESTS_WITH_DB += pvGetAsync
REGRESSION_TESTS_WITH_DB += pvPutAsync
REGRESSION_TESTS_WITH_DB += reassign

REGRESSION_TESTS_WITH_DB += norace

# uncomment this test to see race
# fail (safe mode off)
#REGRESSION_TESTS_WITH_DB += race

REGRESSION_TESTS_WITHOUT_DB += array
REGRESSION_TESTS_WITHOUT_DB += assign
REGRESSION_TESTS_WITHOUT_DB += change
REGRESSION_TESTS_WITHOUT_DB += commaOperator
REGRESSION_TESTS_WITHOUT_DB += local
REGRESSION_TESTS_WITHOUT_DB += pvSync
REGRESSION_TESTS_WITHOUT_DB += safeMonitor
REGRESSION_TESTS_WITHOUT_DB += opttVar
REGRESSION_TESTS_WITHOUT_DB += userfunc
REGRESSION_TESTS_WITHOUT_DB += safeModeNotAssigned

REGRESSION_TESTS += $(REGRESSION_TESTS_WITHOUT_DB)
REGRESSION_TESTS += $(REGRESSION_TESTS_WITH_DB)

ifeq '$(EPICS_HAS_UNIT_TEST)' '1'
TESTPROD_HOST += $(REGRESSION_TESTS)
TESTSCRIPTS_HOST += $(REGRESSION_TESTS:%=%.t)
TESTSCRIPTS_HOST += $(REGRESSION_TESTS_WITH_DB:%=%Ioc.t)
endif

#TESTPROD_HOST += ctest

#  Libraries
PROD_LIBS += seqSoftIoc
PROD_LIBS += seq pv
PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

LIBRARY += seqSoftIoc

DBD += seqSoftIoc.dbd
seqSoftIoc_DBD += base.dbd

seqSoftIoc_SRCS += seqSoftIoc_registerRecordDeviceDriver.cpp

seqSoftIoc_LIBS += seq pv
seqSoftIoc_LIBS += $(EPICS_BASE_IOC_LIBS)

ifeq '$(EPICS_HAS_UNIT_TEST)' '1'
seqSoftIoc_SRCS += testSupport.c

REGRESSION_TESTS_vxWorks = $(filter-out pvGetAsync,$(REGRESSION_TESTS))

PROD_vxWorks = vxTestHarness
vxTestHarness_SRCS += $(REGRESSION_TESTS_vxWorks:%=%.st)
vxTestHarness_SRCS += testSupport.c
vxTestHarness_SRCS += vxTestHarness_registerRecordDeviceDriver.cpp
vxTestHarness_OBJS += $(EPICS_BASE_BIN)/vxComLibrary

DBD += vxTestHarness.dbd
vxTestHarness_DBD += base.dbd
vxTestHarness_DBD += $(COMMON_DIR)/vxTestHarnessRegistrars.dbd

USR_DBDFLAGS += -I $(COMMON_DIR)

DB += vxTestHarness.db
endif

# PROD_RTEMS = rtemsTestHarness
# rtemsTestHarness_SRCS += $(TESTPROD_HOST::%=%.st)

#PROD_RTEMS += $(TESTPROD_HOST)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

SOFTIOC=$(EPICS_BASE_BIN)/softIoc$(HOSTEXE)
SOFTDBD=$(EPICS_BASE)/dbd/softIoc.dbd

$(REGRESSION_TESTS_WITH_DB:%=%Ioc.t): %Ioc.t: %$(EXE) ../makeTestfile.pl
	$(RM) $@.tmp
	$(PERL) ../makeTestfile.pl $@.tmp $* $< ioc $(SOFTIOC) $(SOFTDBD) $(USE_VALGRIND)
	$(MV) $@.tmp $@

$(REGRESSION_TESTS:%=%.t): %.t: %$(EXE) ../makeTestfile.pl
	$(RM) $@.tmp
	$(PERL) ../makeTestfile.pl $@.tmp $* $< noioc $(SOFTIOC) $(SOFTDBD) $(USE_VALGRIND)
	$(MV) $@.tmp $@

norace.i race.i: ../raceCommon.st

$(COMMON_DIR)/vxTestHarnessRegistrars.dbd: ../makeTestDbd.pl
	$(RM) $@.tmp
	$(PERL) ../makeTestDbd.pl $(REGRESSION_TESTS_vxWorks) > $@.tmp
	$(MV) $@.tmp $@

$(COMMON_DIR)/vxTestHarness.dbd: $(COMMON_DIR)/vxTestHarnessRegistrars.dbd

$(COMMON_DIR)/vxTestHarness.db: $(REGRESSION_TESTS_WITH_DB:%=../%.db)
	$(RM) $@.tmp
	$(PERL) -ne print $(REGRESSION_TESTS_WITH_DB:%=../%.db) > $@.tmp
	$(MV) $@.tmp $@

st.cmd: ../st.cmd.vxWorks ../makeStCmd.pl
	$(RM) $@.tmp
	$(CP) ../st.cmd.vxWorks $@.tmp
	$(PERL) -MExtUtils::Command -e chmod 644 $@.tmp
	$(PERL) ../makeStCmd.pl $(REGRESSION_TESTS_vxWorks) >> $@.tmp
	$(MV) $@.tmp $@

ifeq "$(OS_CLASS)" "vxWorks"
build: st.cmd
endif
