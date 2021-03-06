  red=`tput setaf 1`
  bold=`tput bold `
  green=`tput setaf 2`
  reset=`tput sgr0`

if [ $TUTORIAL_LOG == "" ]; then
  export TUTORIAL_LOG=$PWD/tutorial_log
fi
  
# copy standard Makefile for applications
cp $FEMUS_DIR/solvers/Makefile .

# create directories for results
mkdir RESU
mkdir RESU_MED

# copy standard solvers into local SRC folder
cp $FEMUS_DIR/solvers/MGSolverFSI/*.h SRC/
cp $FEMUS_DIR/solvers/MGSolverFSI/*.C SRC/
cp $FEMUS_DIR/solvers/MGSolverDS/*.h SRC/
cp $FEMUS_DIR/solvers/MGSolverDS/*.C SRC/

echo "Compiling application "    >> $TUTORIAL_LOG
if [ -f $FEMUS_DIR/lib/libfemus_2d.so ]; then
  echo "FEMuS 2D library found " >> $TUTORIAL_LOG
  echo "Compiling with library " >> $TUTORIAL_LOG
  make withlib_2d -j2
else
  echo "FEMuS 2D library NOT found " >> $TUTORIAL_LOG
  echo "Compiling without library "  >> $TUTORIAL_LOG
  make -j2
fi  
  
if [ "$?" != 0 ]; then
      echo "${red}${bold}==============================================" 
      echo "             APPLICATION COMPILATION ERROR"
      echo "==============================================${reset}"
      
      echo "=============================================="              >> $TUTORIAL_LOG
      echo "             APPLICATION COMPILATION ERROR"                  >> $TUTORIAL_LOG
      echo "=============================================="              >> $TUTORIAL_LOG
      
      return
fi

femus_gencase_run_lib2D 1

mpiexec -np 1 $FM_MYAPP-opt 2> messages.log



