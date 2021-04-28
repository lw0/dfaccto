# Setup
VENV_DIR = $(shell realpath ./.venv)
INSTALL_DIR = $(shell realpath ./sub/dfaccto_tpl/)
BUILD_DIR = $(shell realpath ./build)

# Software testbench
SW_DIR = $(shell realpath ./sw)
SW_BUILD_DIR = $(BUILD_DIR)/sw
SW_LIB = $(SW_BUILD_DIR)/aux.a
SW_SRCS = $(shell find $(SW_DIR) -name ".*" -prune -o -type f -name '*.cpp' -print)
SW_OBJS = $(SW_SRCS:$(SW_DIR)/%.cpp=$(SW_BUILD_DIR)/%.o)
SW_DEPS = $(SW_OBJS:.o=.d)

# Hardware sources
HW_DIR = $(shell realpath ./hw)
HW_CONFIG_DIR =   $(HW_DIR)/cfg
HW_TEMPLATE_DIR = $(HW_DIR)/tpl
HW_GENERATE_DIR = $(HW_DIR)/gen
HW_STATIC_DIR =   $(HW_DIR)/hdl
HW_SRCS_LIST =    $(HW_DIR)/sources.lst
HW_BUILD_DIR = $(BUILD_DIR)/hw
HW_BUILD_SRCS_LIST = $(HW_BUILD_DIR)/sources.lst
#------------------------------------------------------------------------------
# Root config file for VHDL source generation (dfaccto_tpl)
HW_CONFIG = $(HW_CONFIG_DIR)/testbench.py
#------------------------------------------------------------------------------
# Toplevel VHDL Entity
HW_TOP = Testbench
#------------------------------------------------------------------------------
HW_MODEL = $(HW_BUILD_DIR)/$(HW_TOP)

# Simulation artifacts
WAVE_DIR = $(shell realpath ./wave)
WAVE_FILE = $(HW_TOP)_$(shell date +%Y_%m_%d_%H%M%S).ghw
WAVE_LINK = $(HW_TOP).ghw

# Flags:
CXXFLAGS += -std=c++17 -g
GHDLFLAGS += -g


#------------------------------------------------------------------------------
# User Interface
#------------------------------------------------------------------------------

.PHONY: info
info:
	@echo "Usage:"
	@echo " make info        - Display this information panel"
	@echo " make hwsrc       - Generate hardware sources from templates and config script"
	@echo " make swlib       - Build software library with testbench logic"
	@echo " make hwmodel     - Build hardware model from sources"
	@echo " make model       - Link software and hardware parts into simulation binary"
	@echo " make sim         - Run the simulation binary and generate trace files"
	@echo " make clean       - Remove build artifacts, but keep simulation traces and dfaccto_tpl environment"
	@echo " make clean-all   - Remove build artifacts, simulation traces and dfaccto_tpl environment"
	@echo
	@echo "Variables:"
	@echo " VENV_DIR           = $(VENV_DIR)"
	@echo " INSTALL_DIR        = $(INSTALL_DIR)"
	@echo " BUILD_DIR          = $(BUILD_DIR)"
	@echo
	@echo " SW_DIR             = $(SW_DIR)"
	@echo " SW_BUILD_DIR       = $(SW_BUILD_DIR)"
	@echo " SW_LIB             = $(SW_LIB)"
	@echo " SW_SRCS            = $(SW_DIR)/{ $(SW_SRCS:$(SW_DIR)/%=%) }"
	@echo " SW_OBJS            = $(SW_BUILD_DIR)/{ $(SW_OBJS:$(SW_BUILD_DIR)/%=%) }"
	@echo " SW_DEPS            = $(SW_BUILD_DIR)/{ $(SW_DEPS:$(SW_BUILD_DIR)/%=%) }"
	@echo
	@echo " HW_DIR             = $(HW_DIR)"
	@echo " HW_CONFIG_DIR      = $(HW_CONFIG_DIR)"
	@echo " HW_TEMPLATE_DIR    = $(HW_TEMPLATE_DIR)"
	@echo " HW_GENERATE_DIR    = $(HW_GENERATE_DIR)"
	@echo " HW_STATIC_DIR      = $(HW_STATIC_DIR)"
	@echo " HW_SRCS_LIST       = $(HW_SRCS_LIST)"
	@echo " HW_BUILD_DIR       = $(HW_BUILD_DIR)"
	@echo " HW_BUILD_SRCS_LIST = $(HW_BUILD_SRCS_LIST)"
	@echo
	@echo " HW_CONFIG          = $(HW_CONFIG)"
	@echo " HW_TOP             = $(HW_TOP)"
	@echo " HW_MODEL           = $(HW_MODEL)"
	@echo
	@echo " WAVE_DIR           = $(WAVE_DIR)"
	@echo " WAVE_FILE          = $(WAVE_FILE)"
	@echo " WAVE_LINK          = $(WAVE_LINK)"
	@echo
	@echo " CXXFLAGS           = $(CXXFLAGS)"
	@echo " GHDLFLAGS          = $(GHDLFLAGS)"


#------------------------------------------------------------------------------
# Hardware Sources: Generate hardware sources with dfaccto_tpl
#------------------------------------------------------------------------------

.PHONY: hwsrc
hwsrc: $(HW_SRCS_LIST)

$(VENV_DIR): $(INSTALL_DIR)
	@echo "--- Initialize template toolchain"
	@python -m venv $(VENV_DIR)
	@git submodule update --remote $(INSTALL_DIR)
	@source $(VENV_DIR)/bin/activate; \
	 cd $(INSTALL_DIR); \
	 python -m pip install --upgrade pip; \
	 python -m pip install .

.PHONY: $(HW_SRCS_LIST)
$(HW_SRCS_LIST): $(VENV_DIR)
	@echo
	@echo "--- Generating hardware sources"
	@source $(VENV_DIR)/bin/activate; \
	 python -m dfaccto_tpl --tpldir=$(HW_TEMPLATE_DIR) --outdir=$(HW_GENERATE_DIR) --config=$(HW_CONFIG)
	@find $(HW_STATIC_DIR) -name '*.vhd' > $(HW_SRCS_LIST)
	@find $(HW_GENERATE_DIR) -name '*.vhd' >> $(HW_SRCS_LIST)


#------------------------------------------------------------------------------
# Software Testbench: Compile static library from C++ sources
#------------------------------------------------------------------------------

.PHONY: swlib
swlib: $(SW_LIB)

$(SW_BUILD_DIR)/%.o: $(SW_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -MMD -o $@

$(SW_LIB): $(SW_OBJS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

-include $(SW_DEPS)


#------------------------------------------------------------------------------
# Hardware Model: Build and link with ghdl
#------------------------------------------------------------------------------

.PHONY: hwmodel
hwmodel: $(HW_SRCS_LIST)
	@mkdir -p $(HW_BUILD_DIR)
	@if [ "$(shell cat $(HW_BUILD_SRCS_LIST) | sort | md5sum)" != \
	      "$(shell cat $(HW_SRCS_LIST) | sort | md5sum)" ]; then \
	  cat $(HW_SRCS_LIST) > $(HW_BUILD_SRCS_LIST); \
	  echo; \
	  echo "--- Updating hardware source library"; \
	  cat $(HW_BUILD_SRCS_LIST) | while read HW_SRC; do \
	    echo "----- Import file: $$HW_SRC"; \
	    ghdl import --workdir=$(HW_BUILD_DIR) $$HW_SRC; \
	  done; \
	else \
	  echo; \
	  echo "--- Keeping hardware source library"; \
	fi
	@echo
	@echo "--- Building hardware model for: $(HW_TOP)"
	@ghdl make -b --workdir=$(HW_BUILD_DIR) $(GHDLFLAGS) -o $(HW_MODEL) $(HW_TOP)

.PHONY: model
model: $(SW_LIB) hwmodel
	@echo
	@echo "--- Linking model for: $(HW_TOP)"
	@ghdl link --LINK=$(CXX) --workdir=$(HW_BUILD_DIR) -Wl,$(SW_LIB) -Wl,-llua $(GHDLFLAGS) -o $(HW_MODEL) $(HW_TOP)


#------------------------------------------------------------------------------
# Simulation: Execute model and store waveforms
#------------------------------------------------------------------------------

.PHONY: sim
sim: model
	@echo
	@echo "--- Simulating hardware model for: $(HW_TOP)"
	@echo "----- Waveform at: $(WAVE_DIR)/$(WAVE_FILE)"
	@mkdir -p $(WAVE_DIR)
	@$(HW_MODEL) --wave=$(WAVE_DIR)/$(WAVE_FILE) || true
	@ln -sfr $(WAVE_DIR)/$(WAVE_FILE) $(WAVE_DIR)/$(WAVE_LINK)


#------------------------------------------------------------------------------
# Cleanup
#------------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(HW_GENERATE_DIR)/*
	rm -f  $(HW_SRCS_LIST)
	rm -rf $(SW_BUILD_DIR)
	rm -rf $(HW_BUILD_DIR)

.PHONY: clean-all
clean-all: clean
	rm -rf $(VENV_DIR)
	rm -rf $(WAVE_DIR)

