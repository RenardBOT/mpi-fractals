# CREDITS: Nicholas Hamilton, Scot McPeak, and the Makefile Tutorial
# Version modifiée du Makefile partagé par Nicholas Hamilton, lui même modifié de Scot McPeak : https://stackoverflow.com/a/27794283

# Compilateur: MPICC
CC          := mpicc

# Le nom de l'exécutable
TARGET      := mandelbrot

# Les fichiers sources, les fichiers d'entête, les fichiers objets, les fichiers binaires et les ressources s'il y en a
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

# Specifique au projet

OUTPUTDIR   := output

# Les options de compilation, les librairies et les dossiers d'inclusion
CFLAGS      := -Wall -g
LIB         := -lm
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

# A modifier si besoin d'un exécutable sans ressources
default: $(TARGET)

#---------------------------------------------------------------------------------
# A NE PAS MODIFIER (sinon tout explose)
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Make complet
all: directories $(TARGET)

# Remake
remake:	cleaner all

# Création des dossiers
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OUTPUTDIR)

# Clean des fichiers objets
clean:
	@$(RM) -rf $(BUILDDIR)

# Full clean
cleaner:	clean
	@$(RM) -rf $(TARGETDIR)

# Inclure les informations de dépendance pour les fichiers .o existants
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Création de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

# Création des fichiers objets
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Non-File Targets
.PHONY: all remake clean cleaner resources

cleanoutput:
	-rm -f $(OUTPUTDIR)/*.bmp

allclean: cleanoutput cleaner
	