
# Makefile B3230 - TP Multitâche Carrefour

# Exécutable
EXE=Carrefour

# Commandes
CPPFLAGS = -Wall -g -I $(INCLUDE_TP)	
EDLFLAGS = -L $(LIBPATH_TP) $(LIB_SYS)

COPY = cp
REMOVE = rm
ECHO = @echo
GCC = g++

# Sources
INT = src/Appli.h src/GestClavier.h src/GestFeux.h src/GestVoie.h
REAL = $(INT:.h=.cpp) 
OBJ = $(INT:.h=.o)

# Chemins pour les options
CPPATH = backup/
BACKUPPATH = ./Compte-Rendu/2015/2/B3230
LIBPATH = -L ./
INCPATH = -I ./

# Librairies
INCLUDE_TP = ./
LIBPATH_TP = $(INCLUDE_TP)
LIB_SYS = -lncurses -ltcl -ltp


# PHONY
.PHONY: clean backup

# EDL
$(EXE): $(OBJ)
	$(GCC) -o $(EXE) $^ $(EDLFLAGS)


# COMPILATION
src/%.o: %.cpp $(INT)
	$(GCC) -c $< $(CPPFLAGS)			
# Clean
clear:
	$(RM) $(RMFLAGS) $(EXE) $(OBJ) core

# Backup
backup:
	$(CP) $(CPFLAGS) $(INT) $(REAL) $(CPPATH)