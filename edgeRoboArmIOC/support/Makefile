#FILENAME:      Makefile
#USAGE:         Top synApps Makefile
#Version:       $Revision$
#Modified By:   $Author$
#Last Modified: $Date$
#HeadURL:       $URL$

#NOTES
#     - The "MODULE_LIST" order is based on compile time dependencies.
#     - The user must modify SUPPORT and EPICS_BASE in the
#       <synApps>/support/configure directory for the local configuration.
#     - To support multiple configurations, use multiple configure* directories
#     - Support modules can be shared between configurations only if
#       dependencies are not violated.  Only the "DIRS" are the target of
#       gnumake.  If this configuration is using a support module built by
#       another configuration, then the SUPPORT_DIRS line for that support
#       module must be commented out (i.e, must begin with a '#').
#     - To remove modules from the build, delete or comment out the module
#       in the <synApps>/configure/RELEASE file; not here.

# Note the only dependencies that matter in $(<module>)_DEPEND_DIRS are
# compile-time dependencies.

TOP = .

MASTER_FILE = $(TOP)/configure/RELEASE

include $(TOP)/configure/CONFIG

DIRS := $(DIRS) $(filter-out $(DIRS), configure)

define  MODULE_defined
  ifdef $(1)
  SUPPORT_DIRS  += $($(1))
  RELEASE_FILES += $($(1))/configure/RELEASE
  endif  
endef


###### 1st Tier Support Modules - Only Depend on EPICS BASE ######

MODULE_LIST = SNCSEQ IPAC SSCAN AUTOSAVE
$(foreach mod, $(MODULE_LIST), $(eval $(call MODULE_defined,$(mod)) ))

###### 2nd Tier Support Modules - Only Depend on 1st Tier ########

MODULE_LIST  = ASYN CALC
$(foreach mod, $(MODULE_LIST), $(eval $(call MODULE_defined,$(mod)) ))

$(ASYN)_DEPEND_DIRS = $(SNCSEQ) $(IPAC)
$(CALC)_DEPEND_DIRS = $(SSCAN)

################### 3rd Tier Support Modules #####################

MODULE_LIST  = IP USB
$(foreach mod, $(MODULE_LIST), $(eval $(call MODULE_defined,$(mod)) ))

$(IP)_DEPEND_DIRS   = $(ASYN) $(IPAC) $(SNCSEQ)
$(USB)_DEPEND_DIRS  = $(ASYN) $(IPAC) $(SNCSEQ)

################### End of Support-Modules #####################

DIRS = $(SUPPORT_DIRS)

################### User Modules #####################

ifdef XXX
DIRS += $(XXX)
RELEASE_FILES += $(XXX)/configure/RELEASE
$(XXX)_DEPEND_DIRS = $(SUPPORT_DIRS)
endif

ACTIONS += uninstall realuninstall distclean cvsclean

include $(TOP)/configure/RULES_TOP

release:
	echo SUPPORT=$(SUPPORT)
	echo ' '
	echo EPICS_BASE=$(EPICS_BASE)
	echo ' '
	echo MASTER_FILE=$(MASTER_FILE)
	echo ' '
	echo DIRS=$(DIRS)
	echo ' '
	echo RELEASE_FILES=$(RELEASE_FILES)
	echo ' '
	$(PERL) $(TOP)/configure/makeReleaseConsistent.pl $(SUPPORT) $(EPICS_BASE) $(MASTER_FILE) $(RELEASE_FILES)

