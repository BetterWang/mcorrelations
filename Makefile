

NAME		:= correlations
VERSION		:= 0.2

MAXH		:= 6

ROOT		:= root
ROOTFLAGS	:= -l -b -q 
ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS	:= $(shell root-config --libs)
CXX		:= g++ -c 
CXXFLAGS	:= -Wall -Wextra -Weffc++ -g -O 
CXXFLAGS	:= -Wall -Wextra -Weffc++ -ansi -pedantic -g -O3
ifneq ($(FULL),)
CPPFLAGS	:= -I. -DCORRELATIONS_CLOSED_ENABLE_QC8 \
		       -DCORRELATIONS_CLOSED_ENABLE_QC7
else		       
CPPFLAGS	:= -I.
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
	./prog -w -i $@ -e 1000 -m 10 -M 20

closed.dat:data.dat prog
	./prog -i $< -o $@ -c -n $(MAXH) -l 

recursive.dat:data.dat closed.dat prog
	./prog -i $< -o $@ -r -n $(MAXH) -l -O closed.dat

recursive.png:correlations/test/Test.C data.dat $(HEADERS) $(TESTS)
	$(ROOT) $(ROOTFLAGS) $<+\(1,$(MAXH),\"data.dat\"\)

closed.png:correlations/test/Test.C data.dat $(HEADERS) $(TESTS)
	$(ROOT) $(ROOTFLAGS) $<+\(0,$(MAXH),\"data.dat\"\)


test:	recursive.dat 

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

