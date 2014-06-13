#------------------------------------------------------------------------------

SOURCE=\
formulas/AllTomorrow.cpp \
formulas/AllTomorrow.h \
formulas/AllUntil.cpp \
formulas/AllUntil.h \
formulas/Atom.cpp \
formulas/Atom.h \
formulas/Conjunction.cpp \
formulas/Conjunction.h \
formulas/ExistsTomorrow.cpp \
formulas/ExistsTomorrow.h \
formulas/ExistsUntil.cpp \
formulas/ExistsUntil.h \
formulas/Formula.cpp \
formulas/Formula.h \
formulas/Negation.cpp \
formulas/Negation.h \
parser/CTLParser.cpp \
parser/CTLParser.h \
tableau/Tableau.cpp \
tableau/Tableau.h \
common.h \
main.cpp \


BINARY=ctl-sat

#------------------------------------------------------------------------------

all: $(BINARY)

$(BINARY): $(SOURCE)

	g++ $(SOURCE) -o $(BINARY)

clean:

	rm -f $(BINARY)

