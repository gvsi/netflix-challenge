.DEFAULT_GOAL := test

FILES :=                              \
    Netflix.c++                       \
    Netflix.h                         \
    Netflix.log                       \
    html                              \
    RunNetflix.c++                    \
    RunNetflix.in                     \
    RunNetflix.out                    \
    TestNetflix.c++                   \
    TestNetflix.out										\
   # 	collatz-tests/gca386-RunNetflix.in   \
   # 	collatz-tests/gca386-RunNetflix.out  \
   # 	collatz-tests/gca386-TestNetflix.c++ \
   # 	collatz-tests/gca386-TestNetflix.out \

ifeq ($(shell uname), Darwin)                                        # Apple
    CXX          := g++
    INCLUDE      := /usr/local/include
    CXXFLAGS     := -pedantic -std=c++11 -I$(INCLUDE) -Wall -Weffc++
    LIB          := /usr/local/lib
    LDFLAGS      := -lgtest_main
    CLANG-CHECK  := clang-check
    GCOV         := gcov
    GCOVFLAGS    := -fprofile-arcs -ftest-coverage
    VALGRIND     := valgrind
    DOXYGEN      := doxygen
    CLANG-FORMAT := clang-format
else ifeq ($(CI), true)                                              # Travis CI
    CXX          := g++-5
    INCLUDE      := /usr/include
    CXXFLAGS     := -pedantic -std=c++11 -Wall -Weffc++
    LIB          := /usr/lib
    LDFLAGS      := -lgtest -lgtest_main -pthread -lboost_serialization
    CLANG-CHECK  := clang-check
    GCOV         := gcov-5
    GCOVFLAGS    := -fprofile-arcs -ftest-coverage
    VALGRIND     := valgrind
    DOXYGEN      := doxygen
    CLANG-FORMAT := clang-format
else ifeq ($(shell uname -p), unknown)                               # Docker
    CXX          := g++
    INCLUDE      := /usr/include
    CXXFLAGS     := -pedantic -std=c++11 -L/usr/lib/x86_64-linux-gnu/ -lboost_serialization -Wall -Weffc++
    LIB          := /usr/lib
    LDFLAGS      := -lgtest -lgtest_main -pthread
    CLANG-CHECK  := clang-check
    GCOV         := gcov
    GCOVFLAGS    := -fprofile-arcs -ftest-coverage
    VALGRIND     := valgrind
    DOXYGEN      := doxygen
    CLANG-FORMAT := clang-format-3.5
else                                                                 # UTCS
    CXX          := g++-4.8
    INCLUDE      := /usr/include
    CXXFLAGS     := -pedantic -std=c++11 -Wall -Weffc++
    LIB          := /usr/lib
    LDFLAGS      := -lgtest -lgtest_main -pthread
    CLANG-CHECK  := clang-check-3.8
    GCOV         := gcov-4.8
    GCOVFLAGS    := -fprofile-arcs -ftest-coverage
    VALGRIND     := valgrind
    DOXYGEN      := doxygen
    CLANG-FORMAT := clang-format-3.8
endif

collatz-tests:
	git clone https://github.com/cs371p-fall-2016/netflix-tests/

html: Doxyfile Netflix.h Netflix.c++ RunNetflix.c++ TestNetflix.c++
	doxygen Doxyfile

Netflix.log:
	git log > Netflix.log

Doxyfile:
	doxygen -g

RunNetflix: Netflix.h Netflix.c++ RunNetflix.c++
	$(CXX) $(CXXFLAGS) Netflix.c++ RunNetflix.c++ -o RunNetflix -lboost_serialization

RunNetflix.tmp: RunNetflix
	./RunNetflix < probe.txt > RunNetflix.tmp

TestNetflix: Netflix.h Netflix.c++ TestNetflix.c++
	$(CXX) $(CXXFLAGS) $(GCOVFLAGS) Netflix.c++ TestNetflix.c++ -o TestNetflix $(LDFLAGS)
	-$(CLANG-CHECK) -extra-arg=-std=c++11          TestNetflix.c++ --
	-$(CLANG-CHECK) -extra-arg=-std=c++11 -analyze TestNetflix.c++ --

TestNetflix.tmp: TestNetflix
	$(VALGRIND) ./TestNetflix                               >  TestNetflix.tmp 2>&1
	$(GCOV) -b Netflix.c++ | grep -A 5 "File 'Netflix.c++'" >> TestNetflix.tmp
	cat TestNetflix.tmp

check:
	@not_found=0;                                 \
    for i in $(FILES);                            \
    do                                            \
        if [ -e $$i ];                            \
        then                                      \
            echo "$$i found";                     \
        else                                      \
            echo "$$i NOT FOUND";                 \
            not_found=`expr "$$not_found" + "1"`; \
        fi                                        \
    done;                                         \
    if [ $$not_found -ne 0 ];                     \
    then                                          \
        echo "$$not_found failures";              \
        exit 1;                                   \
    fi;                                           \
    echo "success";

clean:
	rm -f  *.gcda
	rm -f  *.gcno
	rm -f  *.gcov
	rm -f  *.plist
	rm -f  RunNetflix
	rm -f  RunNetflix.tmp
	rm -f  TestNetflix
	rm -f  TestNetflix.tmp
	rm -rf *.dSYM

config:
	git config -l

format:
	clang-format -i Netflix.c++
	clang-format -i Netflix.h
	clang-format -i RunNetflix.c++
	clang-format -i TestNetflix.c++

scrub:
	make clean
	rm -f  Netflix.log
	rm -rf html
	rm -rf latex

status:
	make clean
	@echo
	git branch
	git remote -v
	git status

# test: html Netflix.log RunNetflix.tmp TestNetflix.tmp netflix-tests check

test: RunNetflix.tmp

versions:
	which make
	make --version
	@echo
	which git
	git --version
	@echo
	which $(CXX)
	$(CXX) --version
	@echo
	ls -ald $(INCLUDE)/boost
	@echo
	ls -ald $(INCLUDE)/gtest
	@echo
	ls -al $(LIB)/*gtest*
	@echo
	which $(CLANG-CHECK)
	$(CLANG-CHECK) --version
	@echo
	which $(GCOV)
	$(GCOV) --version
	@echo
	which $(VALGRIND)
	$(VALGRIND) --version
	@echo
	which $(DOXYGEN)
	$(DOXYGEN) --version
	@echo
	which $(CLANG-FORMAT)
	$(CLANG-FORMAT) --version
