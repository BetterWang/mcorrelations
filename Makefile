

NAME		:= correlations
VERSION		:= 0.9

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
LDFLAGS		:= -Wl,--no-as-needed -lrt
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
		   correlations/test/WriteData.hh		\
		   correlations/test/Tester.hh			\
		   correlations/test/Printer.hh			\
		   correlations/test/Comparer.hh		
PROGS		:= correlations/progs/analyze.cc 		\
		   correlations/progs/write.cc 			\
		   correlations/progs/compare.cc 		\
		   correlations/progs/print.cc			\
		   correlations/progs/Write.C			\
		   correlations/progs/Analyze.C			\
		   correlations/progs/Compare.C			
DOCS		:= doc/Mainpage.hh				\
		   doc/License.hh				\
		   doc/Test.C					
EXEC		:= $(notdir $(basename $(PROGS)))
EXEC_ARGS	:= -L
EXTRA		:= Makefile 				\
		   doc/Doxyfile.in 			\
		   doc/style.css			\
		   data/ante.mc

%.o:correlations/progs/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< 

%.o:correlations/progs/%.C
	$(CXX) $(CPPFLAGS)  $(ROOTCFLAGS) -DAS_PROG \
		$(filter-out -Weffc++, $(filter-out -pedantic, $(CXXFLAGS))) \
		$< 

%:%.o
	$(LD) $(LDFLAGS) -o $@ $^

%.vlg:%
	valgrind --tool=callgrind --callgrind-out-file=$@ ./$<

all:	$(EXEC)

install: 
ifeq ($(PREFIX),)
	$(error PREFIX is not defined)
endif
	$(foreach f, $(HEADERS), \
	  mkdir -p $(PREFIX)/$(dir $(f)); cp $(f) $(PREFIX)/$(f);)

data.dat:write
	@echo "=== Generating data file ======================="
	@./$<  -e 10 -m 8 -M 10 -o $@
	@echo ""

closed.dat recurrence.dat recursive.dat:data.dat analyze
	@echo "=== Analysing using $(basename $@) ======================="
	@./analyze -t $(basename $@) -i $< -o $@ -n 6 -L 
	@echo ""

data.root:Write
	@echo "=== Generating data file ======================="
	@./$<  -e 10 -m 8 -M 10 
	@echo ""

closed.root recurrence.root recursive.root:data.root Analyze
	@echo "=== Analysing using $(basename $@) ======================="
	@./Analyze -t $(basename $@) -n 6 -L -B 
	@echo ""

recurrence.dat:data.dat analyze closed.dat
recursive.dat:data.dat analyze recurrence.dat
closed.dat: EXEC_ARGS=-L

recursive.png recurrence.png closed.png:correlations/progs/Test.C \
		data.dat $(HEADERS) $(TESTS)
	$(ROOT) $(ROOTFLAGS) $<+\(\"$(basename $@)\",$(MAXH),\"data.dat\"\)

test:	recursive.dat recurrence.dat closed.dat compare
	-./compare -a recurrence.dat -b closed.dat
	-./compare -a recursive.dat  -b closed.dat

Test:	recursive.root recurrence.root closed.root Compare
	./Compare -1 recurrence -2 closed -B
	./Compare -1 recursive  -2 closed -B

retest:
	rm -f *.dat 
	$(MAKE) test

Write.o: 	correlations/progs/Write.C 
Write:		LDFLAGS:=$(LDFLAGS) $(ROOTLIBS) 

Analyze.o: 	correlations/progs/Analyze.C 
Analyze:	LDFLAGS:=$(LDFLAGS) $(ROOTLIBS) 

Compare.o: 	correlations/progs/Compare.C 
Compare:	LDFLAGS:=$(LDFLAGS) $(ROOTLIBS) 

write:		write.o
write.o:	correlations/progs/write.cc 		\
		correlations/Types.hh			\
		correlations/test/Random.hh		\
		correlations/test/Distribution.hh	\
		correlations/test/Weights.hh		\
		correlations/test/Stopwatch.hh		\
		correlations/test/Printer.hh 		\
		correlations/test/WriteData.hh 

analyze:	analyze.o
analyze.o:	correlations/progs/analyze.cc $(HEADERS) \
		correlations/test/Tester.hh		\
		correlations/test/Printer.hh		\
		correlations/test/Stopwatch.hh

compare:	compare.o
compare.o:	correlations/progs/compare.cc 		\
		correlations/Types.hh			\
		correlations/test/Printer.hh		
print:		print.o

doc/Doxyfile:	doc/Doxyfile.in 
	sed -e 's/@PACKAGE@/${PACKAGE}/' -e 's/@VERSION@/${VERSION}/' < $< > $@

doc:	html/index.html
html/index.html:doc/Doxyfile $(HEADERS) $(TESTS) $(PROGS) $(DOCS)
	doxygen doc/Doxyfile

clean:
	find . -name "*~" -or -name "*_C.*" -or -name "*_hh.*" | xargs rm -f
	rm -f core.* TAGS *.o *.png *.vlg *.dat 
	rm -f analyze compare write print Analyze Write Compare doc/Doxyfile
	rm -rf html $(NAME)-$(VERSION) $(NAME)-$(VERSION).tar.gz TAGS

dist:
	mkdir -p $(NAME)-$(VERSION)
	$(foreach f, $(HEADERS) $(TESTS) $(EXTRA) $(PROGS) $(DOCS), \
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

