############################## MAKEFILE FOR FEMUS APPLICATION ############################

# EXECUTABLE FILE NAME
MAIN = $(FM_MYAPP)-$(METHOD)

# DIRECTORY FOR LOCAL COMPILED OBJECT FILES
OBJ_DIR = SRC/obj/

# DIRECTORY FOR LIBRARY COMPILED OBJECT FILES
LIB_OBJ_DIR = src/obj/

# COMPILER
CXX = mpicxx

# COMPILATION FLAGS - OPTIMIZED vs DEBUG
ifeq ($(METHOD),opt)
  CFLAGS = -O3 -w -fPIC -std=c++11 -DNDEBUG
else
  CFLAGS = -O0 -felide-constructors -g -ansi -pedantic -W -Wall -Wextra \
  -Wno-long-long -Wpointer-arith -Wformat -Wparentheses -fPIC -std=c++11
endif



# INCLUDE DIRECTORIES --------------------------------------------------------------------
# FEMUS - LOCAL
INCLUDES =  -I./
INCLUDES += -I./DATA/
INCLUDES += -I./SRC/

# FEMUS - BASE LEVEL
INCLUDES += -I$(FEMUS_DIR)/include/
INCLUDES += -I$(FEMUS_DIR)/src/
INCLUDES += -I$(FEMUS_DIR)/solvers/EqMapFiller/
INCLUDES += -I$(FEMUS_DIR)/solvers/ln_solvers/

# FEMUS - CONTRIBS
FEMUS_CONTRIB        =  $(FEMUS_DIR)/contrib
INCLUDES += -I$(FEMUS_CONTRIB)/matrix/
INCLUDES += -I$(FEMUS_CONTRIB)/parallel/

# HDF5 - PETSC - MED
CONTRIB_HDF5_INCPATH =  $(HDF5_PATH)/include
INCLUDES += -I$(CONTRIB_HDF5_INCPATH)
INCLUDES += -I$(PETSC_DIR)/include
INCLUDES += -I${MED_PATH}/include/salome/ 
INCLUDES += -I${med_PATH}/include/
INCLUDES += -I${MED_COUPL_PATH}/include/
# END INCLUDE ----------------------------------------------------------------------------


# the files below are set in order to know when it is necessary to recompile.
# If the INCLUDES_SRC are changed than the SRC files will be recompiled,
# if the INCLUDES_CONTRIB are changed than the SRC_CONTRIB files will be recompiled,
INCLUDES_SRC  =  $(wildcard ./DATA/*.h)
INCLUDES_SRC +=  $(wildcard $(FEMUS_DIR)/include/*.h)
INCLUDES_CONTRIB  =  $(wildcard $(FEMUS_CONTRIB)/matrix/*.h)
INCLUDES_CONTRIB +=  $(wildcard $(FEMUS_CONTRIB)/parallel/*.h)



# LIBRARY --------------------------------------------------------------------------------
# FLAGS FOR STANDARD COMPILATION
LFLAGS += -L$(HDF5_PATH)/lib/
LFLAGS += -L$(PETSC_DIR)/lib/
LFLAGS += -L$(MED_PATH)/lib/salome/
LFLAGS += -L$(med_PATH)/lib/
LFLAGS += -L$(MED_COUPL_PATH)/lib/

# FEMUS LIBRARY DIRECTORY
LFLAGSAPP += -L$(FEMUS_DIR)/lib/

LIBS =  -lm  -lhdf5 -lpetsc -lmed -lmedloader -lmedcoupling \
	-linterpkernel -lmedcouplingremapper -lmedC 

# FEMUS LIBRARY NAME (2D and 3D)
FEMUS_LIB_2D = -lfemus_2d_$(METHOD)
FEMUS_LIB_3D = -lfemus_3d_$(METHOD)

# TURBULENCE LIBRARY NAME (2D AND 3D)
TURB_LIB_2D = -lturb_2d_$(METHOD)
TURB_LIB_3D = -lturb_3d_$(METHOD)

TURB_SOLVERS_DIR = $(FEMUS_DIR)/solvers/MGSolverTURB/solvers/
TURB_UTILITY_DIR = $(FEMUS_DIR)/solvers/MGSolverTURB/utility/
FEMUS_LINK_SOLV_DIR = $(FEMUS_DIR)/solvers/ln_solvers/
# END LIBRARY ----------------------------------------------------------------------------



# SOURCES AND OBJECTS --------------------------------------------------------------------

# EQUATION MAP FILLER OBJECT FILE
SRC_FILLER_FILES = $(sort $(notdir $(wildcard $(FEMUS_DIR)/solvers/EqMapFiller/*.C )))
SRC_FILLER_OBJ = $(patsubst %.C, %-$(METHOD).o, $(SRC_FILLER_FILES))
OBJ_FILLER = $(addprefix $(OBJ_DIR), $(SRC_FILLER_OBJ))

# SOLVER FILES IN <ln_solvers> DIRECTORY
SRC_SOLVERS_FILES = $(sort $(notdir $(wildcard $(FEMUS_LINK_SOLV_DIR)/*.C )))
SRC_SOLVERS_OBJ = $(patsubst %.C, %-$(METHOD).o, $(SRC_SOLVERS_FILES))
OBJ_SOLVERS = $(addprefix $(OBJ_DIR), $(SRC_SOLVERS_OBJ))

# WHEN USING THE LIBRARY WE AVOID TO COMPILE TURBULENCE FILES
LIB_SRC_SOLVERS_FILES = $(sort $(notdir $(wildcard $(FEMUS_LINK_SOLV_DIR)/*.C )))
TURB_SOLVER_FILES = $(sort $(notdir $(wildcard $(TURB_SOLVERS_DIR)/*.C )))
TURB_UTILITY_FILES = $(sort $(notdir $(wildcard $(TURB_UTILITY_DIR)/*.C )))
LIB_SRC_SOLVERS_FILES := $(filter-out $(TURB_SOLVER_FILES), $(LIB_SRC_SOLVERS_FILES))
LIB_SRC_SOLVERS_FILES := $(filter-out $(TURB_UTILITY_FILES), $(LIB_SRC_SOLVERS_FILES))
LIB_SRC_SOLVERS_OBJ = $(patsubst %.C, %-$(METHOD).o, $(LIB_SRC_SOLVERS_FILES))
LIB_OBJ_SOLVERS = $(addprefix $(OBJ_DIR), $(LIB_SRC_SOLVERS_OBJ))

# LOCAL APP OBJECT FILES
SRC_APP_FILES = $(sort $(notdir $(wildcard ./SRC/*.C )))
SRC_APP_OBJS = $(patsubst %.C, %-$(METHOD).o, $(SRC_APP_FILES))
OBJS_APP = $(addprefix $(OBJ_DIR), $(SRC_APP_OBJS))

# FEMUS AND CONTRIB OBJECT FILES
LIB_SRC_APP_FILES = $(sort $(notdir $(wildcard $(FEMUS_DIR)/src/*.C )))
LIB_SRC_APP_OBJS = $(patsubst %.C, %-$(METHOD).o, $(LIB_SRC_APP_FILES))
LIB_SRC_FEMUS_OBJS = $(addprefix $(FEMUS_DIR)/src/obj/, $(LIB_SRC_APP_OBJS))

# CONTRIB
SRC_CONTRIB := $(wildcard $(FEMUS_CONTRIB)/matrix/*.C)
CONTRIB_OBJS = $(patsubst %.C, %-$(METHOD).o, $(SRC_CONTRIB))
# END SOURCE AND OBJECTS -----------------------------------------------------------------


# MAKE COMMANDS --------------------------------------------------------------------------
# set PHONY variables to distinguish between file names 
.PHONY: depend clean gencase gencase_clean interpolator interpolator_clean resu_clean src_clean all_clean
# this is the executable file rule, the one executed when you call make
all:    $(MAIN)
	@echo  $(MAIN) succesfully compiled!


# this is the rule for linking the objects to obtain the executable file.
# There are two groups of objects, the SRC and the CONTRIB objects, which
# are recompiled in different cases
$(MAIN): $(OBJS_APP)  $(LIB_SRC_FEMUS_OBJS) $(OBJ_FILLER) $(OBJ_SOLVERS) $(CONTRIB_OBJS)  
	@echo 
	@echo "Linking "$@"..."
	@echo 
	@echo $^          | tr " " "\n"
	@echo $(LFLAGS)   | tr " " "\n"
	@echo $(LIBS)     | tr " " "\n"
	@echo "with compiling flags" $(CFLAGS)
	@echo "=========" $(MAIN) "==========="
	@echo 
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $^ $(LFLAGS) $(LIBS)

# DEPENDENCIES FOR RECOMPILATION
$(OBJS_APP): $(INCLUDES_SRC)
$(LIB_SRC_FEMUS_OBJS): $(INCLUDES_SRC)
$(OBJ_FILLER): $(INCLUDES_SRC)
$(CONTRIB_OBJS): $(INCLUDES_CONTRIB)

# RULE FOR FEMUS OBJECT FILES
%-$(METHOD).o : %.C
	@echo "Compiling C++ (in "$(METHOD)" mode) "$<"..."
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# RULE FOR LOCAL FILES AND EQUATION MAP FILLER
SRC/obj/%-$(METHOD).o : %.C
	@mkdir -p $(dir $@)
	@echo "Compiling C++ (in "$(METHOD)" mode) "$<"..."
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# RULE FOR LIBRARY FILES
$(FEMUS_DIR)/src/obj/%-$(METHOD).o : %.C
	@mkdir -p $(dir $@)
	@echo "Compiling C++ (in "$(METHOD)" mode) "$<"..."
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@	
	
# vpath SETTINGS - WHERE TO LOOK FOR SOURCE FILES
vpath %.C ./SRC/
vpath %.C $(FEMUS_DIR)/src/
vpath %.C $(FEMUS_DIR)/solvers/EqMapFiller/
vpath %.C $(FEMUS_DIR)/solvers/ln_solvers/

# COMPILING WITH FEMUS 2D LIBRARY
withlib_2d: $(OBJS_APP) $(OBJ_FILLER) $(LIB_OBJ_SOLVERS)
	@echo " with femus library"
	@echo "Linking "$@"..."
	@echo 
	@echo $^          | tr " " "\n"
	@echo $(LFLAGS) $(LFLAGSAPP)   | tr " " "\n"
	@echo $(LIBS)     | tr " " "\n"
	@echo "with compiling flags" $(CFLAGS)
	@echo "=========" $(MAIN) "==========="
	@echo 
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $^ $(LFLAGS) $(LFLAGSAPP) $(FEMUS_LIB_2D) $(TURB_LIB_2D) $(LIBS)

# COMPILING WITH FEMUS 3D LIBRARY
withlib_3d: $(OBJS_APP) $(OBJ_FILLER) $(LIB_OBJ_SOLVERS)
	@echo " with femus library"
	@echo "Linking "$@"..."
	@echo 
	@echo $^          | tr " " "\n"
	@echo $(LFLAGS) $(LFLAGSAPP)  | tr " " "\n"
	@echo $(LIBS)     | tr " " "\n"
	@echo "with compiling flags" $(CFLAGS)
	@echo "=========" $(MAIN) "==========="
	@echo 
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $^ $(LFLAGS) $(LFLAGSAPP) $(FEMUS_LIB_3D) $(TURB_LIB_3D) $(LIBS)	
	
vpath %.C ./SRC/
vpath %.C $(FEMUS_DIR)/solvers/EqMapFiller/
vpath %.C $(FEMUS_DIR)/solvers/ln_solvers/
	
	
clean:
	$(RM) ./SRC/obj/*-$(METHOD).o *~ ./SRC/*-$(METHOD).o *~ $(MAIN)


gencase:
	@if [ \( -L "$(FEMUS_DIR)/bin/gencase" \) ]; then \
	  rm -f $(FEMUS_DIR)/bin/gencase ;\
	  echo $(FEMUS_DIR)/bin/gencase "removed";\
	fi
	@cd $(FEMUS_DIR)/applications/gencase; \
	$(MAKE); \
	ln -s $(FEMUS_DIR)/bin/gencase-opt $(FEMUS_DIR)/bin/gencase; \

gencase_clean:
	@if [ \( -L "$(FEMUS_DIR)/bin/gencase" \) ]; then \
	  rm -f $(FEMUS_DIR)/bin/gencase ;\
	fi
	@cd $(FEMUS_DIR)/applications/gencase; \
	$(MAKE) clean; \
	
interpolator:
	@if [ \( -e "$(FEMUS_DIR)/bin/Interpolator-opt" \) ]; then \
	  rm -f $(FEMUS_DIR)/bin/interpolator-opt ;\
	fi
	@cd $(FEMUS_DIR)/applications/interpolator; \
	$(MAKE); \
	ln -s $(FEMUS_DIR)/applications/interpolator/interpolator-opt $(FEMUS_DIR)/bin/interpolator; \

interpolator_clean:
	@if [ \( -e "$(FEMUS_DIR)/bin/interpolator" \) ]; then \
	  rm -f $(FEMUS_DIR)/bin/interpolator ;\
	fi
	@cd $(FEMUS_DIR)/applications/interpolator; \
	$(MAKE) clean; \
	
resu_clean:
	@rm -f RESU/sol.*.h5  RESU/sol.*.xmf RESU/mesh.*.*.h5  RESU/mesh.*.xmf RESU/case.*.h5  RESU/case.*.xmf  RESU/time.*.xmf

src_clean:
	@$(MAKE) clean
	@rm -f $(FEMUS_DIR)/src/*.o $(FEMUS_DIR)/src/obj/*.o $(FEMUS_DIR)/contrib/matrix/*.o $(FEMUS_DIR)/contrib/parallel/*.o

all_clean:
	@$(MAKE) resu_clean
	@$(MAKE) src_clean
	@rm .depend


# -------------   MAKE depend and depend file ------------------------
# include the dependency list 
DEPFILE	= .depend
DEPTOKEN	= '\# MAKEDEPENDS'
DEPFLAGS	= -Y -f $(DEPFILE) -s $(DEPTOKEN) -p ./


 depend:
	rm -f $(DEPFILE)
	make $(DEPFILE)


 $(DEPFILE):
	@echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CFLAGS) -- $(SRCS) >&/dev/null
	@echo "Updated .depend"




# "sinclude" allows for the Makefile to work when $(DEPFILE) is missing. 
sinclude  $(DEPFILE)

# DO NOT DELETE THIS LINE -- make depend needs it
