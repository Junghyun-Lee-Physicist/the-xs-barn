# ============================================================
#  CMS Sample DB — Standalone C++ Build
# ============================================================
#  Prerequisites:
#    1. g++ with C++17 support (GCC ≥ 7, Clang ≥ 5)
#    2. nlohmann/json.hpp in include/nlohmann/
#       → wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \
#              -O include/nlohmann/json.hpp
#
#  Usage:
#    make              # build example
#    make run          # build & run with default JSON path
#    make clean        # remove build artifacts
# ============================================================

CXX       := g++
CXXFLAGS  := -std=c++17 -O2 -Wall -Wextra -pedantic
INCDIR    := include
SRCDIR    := src
BINDIR    := bin

SRCS      := $(wildcard $(SRCDIR)/*.cpp)
TARGETS   := $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(SRCS))

JSON_PATH ?= ../data/samples_2017UL.json

# ---- rules ----

.PHONY: all run clean check-json

all: $(TARGETS)

$(BINDIR)/%: $(SRCDIR)/%.cpp $(INCDIR)/cms/sample_loader.h | $(BINDIR) check-json
	$(CXX) $(CXXFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

run: all
	./$(BINDIR)/example $(JSON_PATH)

clean:
	rm -rf $(BINDIR)

check-json:
	@if [ ! -f $(INCDIR)/nlohmann/json.hpp ]; then \
		echo ""; \
		echo "  ERROR: include/nlohmann/json.hpp not found!"; \
		echo "  Run:  wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \\"; \
		echo "             -O include/nlohmann/json.hpp"; \
		echo ""; \
		exit 1; \
	fi
