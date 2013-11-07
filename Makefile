

NAME		:= correlations
VERSION		:= 0.2

ROOT		:= root
ROOTFLAGS	:= -l -b -q 
ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS	:= $(shell root-config --libs)
CXX			:= g++ -c 
CXXFLAGS	:= -Wall -Wextra -Weffc++ -g -O 
CXXFLAGS	:= -Wall -Wextra -Weffc++ -ansi -pedantic -g -O3
CPPFLAGS	:= -I.
ifneq ($(FULL),)
USE7		:= yes
USE8		:= yes
endif
ifneq ($(USE8),)
CPPFLAGS	+= -DCORRELATIONS_CLOSED_ENABLE_U8
endif
ifneq ($(USE7),)   
CPPFLAGS	+= -DCORRELATIONS_CLOSED_ENABLE_U7
endif 
LD		:= g++ 
LDFLAGS		:= 
HEADERS		:= correlations/Correlator.hh			\
		   correlations/FromQVector.hh			\
		   correlations/QVector.hh			\
		   correlations/NestedLoops.hh			\
		   correlations/Result.hh			\
		   correlations/Types.hh			\
		   correlations/closed/FromQVector.hh		\
		   correlations/recurrence/FromQVector.hh	\
		   correlations/recursive/FromQVector.hh	\
		   correlations/recursive/NestedLoops.hh
TESTS		:= correlations/test/Distribution.hh		\
		   correlations/test/Random.hh			\
		   correlations/test/ReadData.hh		\
		   correlations/test/Stopwatch.hh		\
		   correlations/test/Weights.hh			\
		   correlations/test/WriteData.hh	
PROGS		:= correlations/test/prog.cc 			\
		   correlations/test/print.cc			\
		   correlations/test/Test.C
EXEC		:= $(notdir $(basename $(PROGS)))
EXEC_ARGS	:= -L
EXTRA		:= Makefile 				\
		   Doxyfile.in 				\
		   style.css				\
		   data/ante.mc

%.o:correlations/test/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< 

%.o:correlations/test/%.C
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< 

%:%.o
	$(LD) $(LDFLAGS) -o $@ $^

%.vlg:%
	valgrind --tool=callgrind --callgrind-out-file=$@ ./$<

all:	$(EXEC)

data.dat:prog
	@echo "=== Generating data file ======================="
	@./prog -w -i $@ -e 10 -m 8 -M 10
	@echo ""

closed.dat recurrence.dat recursive.dat:data.dat prog
	@echo "=== Analysing using $(basename $@) ======================="
	./prog -t $(basename $@) -i $< -o $@ -n 8 $(EXEC_ARGS) 
	@echo ""
	
recurrence.dat:data.dat prog closed.dat
recursive.dat:data.dat prog recurrence.dat
closed.dat: EXEC_ARGS=-L
recurrence.dat:	EXEC_ARGS=-c closed.dat -L
recursive.dat:  EXEC_ARGS=-c recurrence.dat -L

recursive.png recurrence.png closed.png:correlations/test/Test.C data.dat $(HEADERS) $(TESTS)
	$(ROOT) $(ROOTFLAGS) $<+\(\"$(basename $@)\",$(MAXH),\"data.dat\"\)
	
test:	recursive.dat 

retest:
	rm -f *.dat 
	$(MAKE) test
	
Test.o: correlations/test/Test.C 
Test.o:	CPPFLAGS:=$(CPPFLAGS) $(ROOTCFLAGS) -DAS_PROG
Test.o:	CXXFLAGS:=$(filter-out, -pedantic, $(CXXFLAGS))
Test:	LDFLAGS:=$(LDFLAGS) $(ROOTLIBS) 

prog:	prog.o
prog.o:	correlations/test/prog.cc $(HEADERS) $(TESTS)

print:print.o

Doxyfile:Doxyfile.in 
	sed -e 's/@PACKAGE@/${PACKAGE}/' -e 's/@VERSION@/${VERSION}/' < $< > $@

doc:	Doxyfile $(HEADERS) $(TESTS) $(PROGS)
	doxygen Doxyfile

clean:
	find . -name "*~" -or -name "*_C.*" -or -name "*_hh.*" | xargs rm -f
	rm -f core.* TAGS *~ *.o *.png *.vlg Test.png Doxyfile
	rm -f *_C.* recursive.dat closed.dat data.dat prog print Test
	rm -rf html $(NAME)-$(VERSION) $(NAME)-$(VERSION).tar.gz TAGS

dist:
	mkdir -p $(NAME)-$(VERSION)
	$(foreach f, $(HEADERS) $(TESTS) $(EXTRA) $(PROGS), \
	  mkdir -p $(NAME)-$(VERSION)/$(dir $(f)); \
	  cp $(f) $(NAME)-$(VERSION)/$(f);)
	tar -czvf $(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
	rm -rf $(NAME)-$(VERSION)

distcheck:dist
	tar -xzf $(NAME)-$(VERSION).tar.gz
	(cd $(NAME)-$(VERSION) && $(MAKE) test doc)
	rm -rf $(NAME)-$(VERSION)

#
# EOF
#

