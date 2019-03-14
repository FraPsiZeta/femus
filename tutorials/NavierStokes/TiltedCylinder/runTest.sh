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
cp $FEMUS_DIR/solvers/MGSolverNS/*.h SRC/
cp $FEMUS_DIR/solvers/MGSolverNS/*.C SRC/

echo "Compiling application "    >> $TUTORIAL_LOG
if [ -f $FEMUS_DIR/lib/libfemus_3d_$METHOD.so ]; then
  echo "FEMuS 3D library found " >> $TUTORIAL_LOG
  echo "Compiling with library " >> $TUTORIAL_LOG
  make withlib_3d -j2
else
  echo "FEMuS 3D library NOT found " >> $TUTORIAL_LOG
  echo "Compiling without library "  >> $TUTORIAL_LOG
  make -j2
fi  
  
if [ "$?" != 0 ]; then
      echo "${red}${bold}==============================================" 
      echo "             APPLICATION COMPILATION ERROR"
      echo "==============================================${reset}"
      
      echo "==============================================" >> $TUTORIAL_LOG
      echo "             APPLICATION COMPILATION ERROR"                  >> $TUTORIAL_LOG
      echo "=============================================="      >> $TUTORIAL_LOG
      
      return
fi


femus_gencase_run_lib3D 1

mpiexec -np 1 $FM_MYAPP-$METHOD 2> messages.log

