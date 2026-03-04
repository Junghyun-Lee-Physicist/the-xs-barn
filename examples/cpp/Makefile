# ============================================================
#  The Barn — C++ Examples
# ============================================================
#  Prerequisites:
#    nlohmann/json.hpp in ../../include/nlohmann/
#    → wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \
#           -O ../../include/nlohmann/json.hpp
#
#  Usage:
#    make              # build all examples
#    make run          # build & run basic_usage
#    make clean
# ============================================================

CXX       := g++
CXXFLAGS  := -std=c++17 -O2 -Wall -Wextra -pedantic
INCDIR    := ../../include
BINDIR    := bin

JSON_PATH ?= ../../data/samples_2017UL.json

SRCS      := $(wildcard *.cpp)
TARGETS   := $(patsubst %.cpp,$(BINDIR)/%,$(SRCS))

.PHONY: all run clean check-deps

all: $(TARGETS)

$(BINDIR)/%: %.cpp | $(BINDIR) check-deps
	$(CXX) $(CXXFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

run: all
	./$(BINDIR)/basic_usage $(JSON_PATH)

clean:
	rm -rf $(BINDIR)

check-deps:
	@if [ ! -f $(INCDIR)/nlohmann/json.hpp ]; then \
		echo ""; \
		echo "  ERROR: nlohmann/json.hpp not found at $(INCDIR)/nlohmann/json.hpp"; \
		echo "  Run:"; \
		echo "    wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \\"; \
		echo "         -O $(INCDIR)/nlohmann/json.hpp"; \
		echo ""; \
		exit 1; \
	fi
