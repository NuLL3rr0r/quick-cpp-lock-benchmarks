# Copyright (c) 2025 Mamadou Babaei
#
# Author: Mamadou Babaei <info@babaei.net>
#


DIR_ROOT	:=	$(CURDIR)
DIR_SRC		:=	$(DIR_ROOT)/src
DIR_BIN		:=	$(DIR_ROOT)/bin

CXX		:=	clang++
CXXFLAGS	:=	-std=c++23 -O2 -fno-rtti -fno-exceptions -pthread

BINARIES	:=	spin-unsafe spin mutex hybrid

.PHONY: all
all: $(addprefix $(DIR_BIN)/,$(BINARIES))

$(DIR_BIN)/%: $(DIR_SRC)/%.cpp
	@echo "Building $(subst $(DIR_ROOT)/,,$@)..."
	@mkdir -p "$(DIR_BIN)"
	@$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -rf "$(DIR_BIN)"
	@echo "Cleaning up successfully!"
