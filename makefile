
# Makefile B3230 - TP Multitâche Carrefour

# Commandes
CP = cp
RM = rm
ECHO = @echo
EDL = g++
COMP = g++
EFF = clean

# Fichiers sources
INT = Mere.h Interface.h Feux.h Voie.h
REAL = $(INT:.h=.cpp) 
OBJ = $(INT:.h=.o)

# Exécutable
EXE = Carrefour

# Options des commandes
CPFLAGS = -p
RMFLAGS = -f
EDLFLAGS = -o
COMPFLAGS = -c
CPPFLAGS =

# Chemins pour les options
CPPATH = backup/
LIBPATH = -L/public/tp/tp-multitache
INCPATH = -I/public/tp/tp-multitache

# Librairies pour l'édition des liens
LIBS = -ltp -lncurses -ltcl

# Règles
EFF = clean
BACKUP = backup


#---- PHONY
.PHONY: $(EFF) $(BACKUP)


#---- Règles
# Edition des liens
$(EXE): $(OBJ)
	$(ECHO) "EDL de <makefile>"
	$(EDL) $(LIBPATH) $(EDLFLAGS) $(EXE) $(OBJ) $(LIBS)

# Compilation des sources
# --> Compile tout les *.o à l'aide du *.cpp correspondant et de l'ensemble des *.h
%.o: %.cpp $(INT)
	$(ECHO) "Compil de <"$<">"
	$(COMP) $(INCPATH) $(CPPFLAGS) $(COMPFLAGS) $<

# Clean
# --> Efface les fichiers *.o, l'exécutable et le core
$(EFF):
	$(RM) $(RMFLAGS) $(EXE) $(OBJ) core

# Backup
# --> Copie les fichiers *.o et *.h dans le dossier $(CPPath) (par défaut backup/)
$(BACKUP):
	$(CP) $(CPFLAGS) $(INT) $(REAL) $(CPPATH)