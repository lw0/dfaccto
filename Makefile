SYN_CONFIG = eventtest.py
SIM_CONFIG = tb_eventtest.py
SIM_SCRIPT = $(realpath ./tb/eventtest.lua)

# Directories
BUILD_DIR = $(abspath ./build)
SYN_GEN_DIR = $(BUILD_DIR)/synhw
SYN_GEN_LIST = $(SYN_GEN_DIR)/sources.list
SIM_GEN_DIR = $(BUILD_DIR)/simhw
SIM_GEN_LIST = $(SIM_GEN_DIR)/sources.list

# Submodules
DFACCTO_SIM = $(realpath ./sub/dfaccto_sim)
DFACCTO_TPL = $(realpath ./sub/dfaccto_tpl)
DFACCTO_LIB = $(realpath ./sub/dfaccto_lib)

# DFACCTO-TPL Module structure:
export DFTPL_CFGDIRS      = $(realpath ./cfg)
export DFTPL_TPLDIRS      = $(realpath ./tpl)
export DFTPL_MODULES      = lib sim
export DFTPL_CFGDIRS_lib  = $(DFACCTO_LIB)/cfg
export DFTPL_TPLDIRS_lib  = $(DFACCTO_LIB)/tpl
export DFTPL_CFGDIRS_sim  = $(DFACCTO_SIM)/cfg
export DFTPL_TPLDIRS_sim  = $(DFACCTO_SIM)/tpl

# Export DFACCTO-TPL Variables for Synthesis:
DFTPL_ARGS_SYN  = -E "DFTPL_GEN_DIR = $(SYN_GEN_DIR)"
DFTPL_ARGS_SYN += -E "DFTPL_GEN_LIST = $(SYN_GEN_LIST)"
DFTPL_ARGS_SYN += -E "DFTPL_CONFIG = $(SYN_CONFIG)"

# Export DFACCTO-TPL Variables for Simulation:
DFTPL_ARGS_SIM  = -E "DFTPL_GEN_DIR = $(SIM_GEN_DIR)"
DFTPL_ARGS_SIM += -E "DFTPL_GEN_LIST = $(SIM_GEN_LIST)"
DFTPL_ARGS_SIM += -E "DFTPL_CONFIG = $(SIM_CONFIG)"

# Export DFACCTO-SIM Variables:
DFSIM_ARGS  = -E "DFSIM_TOP       = Testbench"
DFSIM_ARGS += -E "DFSIM_SCRIPT    = $(SIM_SCRIPT)"
DFSIM_ARGS += -E "DFSIM_LIST      = $(SIM_GEN_LIST)"
DFSIM_ARGS += -E "DFSIM_MODEL_DIR = $(BUILD_DIR)/sim"
DFSIM_ARGS += -E "DFSIM_WAVE_DIR  = $(abspath ./wave)"

#------------------------------------------------------------------------------
# User Information
#------------------------------------------------------------------------------

.PHONY: info
info:
	@echo "Usage:"
	@echo " make usage       - Display this information panel"
	@echo
	@echo " make synhwinfo   - Display DFACCTO-TPL build variables for synthesis"
	@echo " make synhw       - Generate hardware sources from template and config files for synthesis"
	@echo " make synhwclean  - Delete hardware sources for synthesis"
	@echo " make simhwinfo   - Display DFACCTO-TPL build variables for simulation"
	@echo " make simhw       - Generate hardware sources from template and config files for simulation"
	@echo " make simhwclean  - Delete hardware sources for simulation"
	@echo " make tplupdate   - Update DFACCTO-TPL environment"
	@echo
	@echo " make siminfo     - Display DFACCTO-SIM build variables"
	@echo " make simmodel    - Build simulation model from hardware sources"
	@echo " make sim         - Run the simulation binary and generate trace files"
	@echo " make simclean    - Delete simulation model"
	@echo
	@echo " make clean       - Delete hardware sources and simulation model"


#------------------------------------------------------------------------------
# DFACCTO-TPL Integration
#------------------------------------------------------------------------------

.PHONY: synhwinfo
synhwinfo:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SYN) info

.PHONY: synhw
synhw:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SYN) gen

.PHONY: synhwdebug
synhwdebug:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SYN) debug

.PHONY: synhwclean
synhwclean:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SYN) clean


.PHONY: simhwinfo
simhwinfo:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SIM) info

.PHONY: simhw
simhw:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SIM) gen

.PHONY: simhwdebug
simhwdebug:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SIM) debug

.PHONY: simhwclean
simhwclean:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SIM) clean


.PHONY: tplupdate
tplupdate:
	@make -C $(DFACCTO_TPL) $(DFTPL_ARGS_SYN) update


#------------------------------------------------------------------------------
# DFACCTO-SIM Integration
#------------------------------------------------------------------------------

.PHONY: siminfo
siminfo:
	@make -C $(DFACCTO_SIM) $(DFSIM_ARGS) info

.PHONY: simmodel
simmodel: simhw
	@make -C $(DFACCTO_SIM) $(DFSIM_ARGS) model

.PHONY: sim
sim: simhw
	@make -C $(DFACCTO_SIM) $(DFSIM_ARGS) sim

.PHONY: simclean
simclean:
	@make -C $(DFACCTO_SIM) $(DFSIM_ARGS) clean


#------------------------------------------------------------------------------
# Cleanup
#------------------------------------------------------------------------------

.PHONY: clean
clean: simhwclean synhwclean simclean

