# Setup:
VENV_DIR = $(shell realpath ./venv)
INSTALL_DIR = $(shell realpath ./sub/dfaccto_tpl/)

# Hardware sources organization
HW_DIR = $(shell realpath ./hdl)
HW_CFG_DIR = $(HW_DIR)/cfg
HW_TPL_DIR = $(HW_DIR)/tpl
HW_GEN_DIR = $(HW_DIR)/gen
HW_STATIC_DIR = $(HW_DIR)/static
HW_SRCS_LIST = $(HW_DIR)/sources.lst

# Hardware config file
HW_CONFIG = $(HW_CFG_DIR)/simple.py

# Simulation:
SIM_TOP = PipelineTest

BUILD_DIR = $(shell realpath ./build)

SIM_LIB_DIR = $(shell realpath ./sim)
export SIM_LIB_BUILD_DIR = $(BUILD_DIR)/sw
export SIM_LIB_TARGET = $(SIM_LIB_BUILD_DIR)/aux.a

SIM_BUILD_DIR = $(BUILD_DIR)/hw
SIM_SRCS_LIST = $(SIM_BUILD_DIR)/sources.lst
SIM_TARGET = $(SIM_BUILD_DIR)/$(SIM_TOP)

TRACE_DIR = $(shell realpath ./trace)
TRACE_FILE = $(SIM_TOP)_$(shell date +%Y_%m_%d_%H%M%S).ghw
TRACE_LINK = $(SIM_TOP).ghw


.PHONY: info
info:
	@echo "Usage:"
	@echo " make info        - Display this information panel"
	@echo " make hardware    - Generate hardware sources from templates and config script"
	@echo " make sim_model   - Build and link a simulation model"
	@echo " make sim         - Run the simulation model and generate trace files"
	@echo " make clean       - Remove all build artifacts, but keep simulation traces and Python environment"
	@echo " make clean-all   - Remove all build artifacts, simulation traces and Python environment"
	@echo
	@echo "Variables:"
	@echo " VENV_DIR          = $(VENV_DIR)"
	@echo " INSTALL_DIR       = $(INSTALL_DIR)"
	@echo
	@echo " HW_DIR            = $(HW_DIR)"
	@echo " HW_CFG_DIR        = $(HW_CFG_DIR)"
	@echo " HW_TPL_DIR        = $(HW_TPL_DIR)"
	@echo " HW_GEN_DIR        = $(HW_GEN_DIR)"
	@echo " HW_STATIC_DIR     = $(HW_STATIC_DIR)"
	@echo " HW_SRCS_LIST      = $(HW_SRCS_LIST)"
	@echo
	@echo " HW_CONFIG         = $(HW_CONFIG)"
	@echo
	@echo
	@echo " SIM_TOP           = $(SIM_TOP)"
	@echo
	@echo " BUILD_DIR         = $(BUILD_DIR)"
	@echo
	@echo " SIM_LIB_DIR       = $(SIM_LIB_DIR)"
	@echo " SIM_LIB_BUILD_DIR = $(SIM_LIB_BUILD_DIR)"
	@echo " SIM_LIB_TARGET    = $(SIM_LIB_TARGET)"
	@echo
	@echo " SIM_BUILD_DIR     = $(SIM_BUILD_DIR)"
	@echo " SIM_SRCS_LIST     = $(SIM_SRCS_LIST)"
	@echo " SIM_TARGET        = $(SIM_TARGET)"
	@echo
	@echo " TRACE_DIR         = $(TRACE_DIR)"
	@echo " TRACE_FILE        = $(TRACE_FILE)"
	@echo " TRACE_LINK        = $(TRACE_LINK)"


#------------------------------------------------------------------------------
# Hardware Side: Use GHDL-builtin make process
#------------------------------------------------------------------------------


$(VENV_DIR): $(INSTALL_DIR)
	@echo "--- Initialize template toolchain"
	@python -m venv $(VENV_DIR)
	@git submodule update $(INSTALL_DIR)
	@source $(VENV_DIR)/bin/activate; \
	 cd $(INSTALL_DIR); \
	 python -m pip install .

.PHONY: hardware
hardware: $(VENV_DIR)
	@echo
	@echo "--- Generating hardware sources"
	@source $(VENV_DIR)/bin/activate; \
	 python -m dfaccto_tpl --tpldir=$(HW_TPL_DIR) --outdir=$(HW_GEN_DIR) --config=$(HW_CONFIG)
	@find $(HW_STATIC_DIR) -name '*.vhd' > $(HW_SRCS_LIST)
	@find $(HW_GEN_DIR) -name '*.vhd' >> $(HW_SRCS_LIST)


.PHONY: sim_lib
sim_lib:
	@echo
	@echo "--- Compiling simulation software library"
	@make -e -C $(SIM_LIB_DIR) all


.PHONY: sim_config
sim_config: hardware
	@mkdir -p $(SIM_BUILD_DIR)
	@if [ "$(shell cat $(SIM_SRCS_LIST) | sort | md5sum)" != \
	      "$(shell cat $(HW_SRCS_LIST) | sort | md5sum)" ]; then \
	  cat $(HW_SRCS_LIST) > $(SIM_SRCS_LIST); \
	  echo; \
	  echo "--- Updating hardware source library"; \
	  cat $(SIM_SRCS_LIST) | while read HW_SRC; do \
	    echo "----- Import file: $$HW_SRC"; \
	    ghdl -i --workdir=$(SIM_BUILD_DIR) $$HW_SRC; \
	  done; \
	else \
	  echo; \
	  echo "--- Keeping hardware source library"; \
	fi


.PHONY: sim_model
sim_model: sim_lib sim_config
	@echo
	@echo "--- Building hardware model for: $(SIM_TOP)"
	@mkdir -p $(SIM_BUILD_DIR)
	@ghdl -m --LINK=$(CXX) --workdir=$(SIM_BUILD_DIR) -Wl,$(SIM_LIB_TARGET) -o $(SIM_TARGET) $(SIM_TOP)


.PHONY: sim
sim: sim_model
	@echo
	@echo "--- Simulating hardware model for: $(SIM_TOP)"
	@echo "----- Waveform at: $(TRACE_DIR)/$(TRACE_FILE)"
	@mkdir -p $(TRACE_DIR)
	@$(SIM_TARGET) --wave=$(TRACE_DIR)/$(TRACE_FILE) || true
	@ln -sfr $(TRACE_DIR)/$(TRACE_FILE) $(TRACE_DIR)/$(TRACE_LINK)


.PHONY: clean
clean:
	rm -rf $(HW_GEN_DIR)/*
	rm -f  $(HW_SRCS_LIST)
	make -e -C $(SIM_LIB_DIR) clean
	rm -rf $(SIM_BUILD_DIR)/*


.PHONY: clean-all
clean-all: clean
	rm -rf $(VENV_DIR)
	rm -rf $(TRACE_DIR)

