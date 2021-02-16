SIM_LIB_BUILD_DIR = ./build # expect override from ../Makefile
SIM_LIB_TARGET = $(SIM_LIB_BUILD_DIR)/aux.a # expect override from ../Makefile
SIM_LIB_SRCS = $(shell find . -type f -name '*.cpp')
SIM_LIB_OBJS = $(SIM_LIB_SRCS:%.cpp=$(SIM_LIB_BUILD_DIR)/%.o)
SIM_LIB_DEPS = $(SIM_LIB_OBJS:.o=.d)

.PHONY: info
info:
	@echo "Usage:"
	@echo " make info        - Display this information panel"
	@echo " make all         - Compile the static library"
	@echo " make clean       - Remove all build artifacts"
	@echo
	@echo "Variables:"
	@echo " SIM_LIB_BUILD_DIR     = $(SIM_LIB_BUILD_DIR)"
	@echo " SIM_LIB_TARGET        = $(SIM_LIB_TARGET)"
	@echo " SIM_LIB_SRCS          = $(SIM_LIB_SRCS)"
	@echo " SIM_LIB_OBJS          = $(SIM_LIB_OBJS)"
	@echo " SIM_LIB_DEPS          = $(SIM_LIB_DEPS)"

all: $(SIM_LIB_TARGET)

$(SIM_LIB_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -MMD -o $@

$(SIM_LIB_TARGET): $(SIM_LIB_OBJS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

-include $(SIM_LIB_DEPS)

.PHONY: clean
clean:
	rm -rf $(SIM_LIB_BUILD_DIR)
