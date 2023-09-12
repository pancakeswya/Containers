OS         := $(shell uname -s)

TEST_LIB   := gtest

INC_DIR    := includes
TEST_DIR   := test

CXX        := g++
CXXFLAGS   := -Wall -Wextra -Werror -std=c++17 -I $(INC_DIR)

TEST_LDLIB := $(addprefix -l,$(TEST_LIB))

OPEN       := $(if $(filter Linux,$(OS)),xdg-open,open)

GCOV       := --coverage
LCOV       := lcov --no-external -c

VALGRIND   := valgrind --tool=memcheck --trace-children=yes --track-origins=yes --leak-check=full

TEST_SRCS  := $(TEST_DIR)/s21-containers-test.cc
TEST_NAME  := s21-containers-test
LCOV_NAME  := s21-containers.info

REPORT_DIR := report

RM         := rm -rf

MAKEFLAGS  += --no-print-directory

all: test

test:
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) $(TEST_LDLIB) -o $(TEST_NAME)
	./$(TEST_NAME)

.PHONY: test

check-valgrind: test
	CK_FORK=NO $(VALGRIND) ./$(TEST_NAME)

check-style:
	clang-format -style=google -n $(INC_DIR)/*.h  $(TEST_DIR)/*.cc

gcov_report:
	$(CXX) $(CXXFLAGS) $(GCOV) $(TEST_SRCS) $(TEST_LDLIB) -o $(TEST_NAME)
	./$(TEST_NAME)
	$(LCOV) -t $(TEST_NAME) -d . -o $(LCOV_NAME)
	genhtml $(LCOV_NAME) -o $(REPORT_DIR)
	$(OPEN) $(REPORT_DIR)/index.html

clean:
	$(RM) $(TEST_NAME)

fclean: clean
	$(RM) $(LCOV_NAME)
	$(RM) $(REPORT_DIR)
	$(RM) *.gcno *.gcda

rebuild:
	$(MAKE) clean
	$(MAKE) all