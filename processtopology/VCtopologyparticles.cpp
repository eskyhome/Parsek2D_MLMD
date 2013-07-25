/***************************************************************************
                  VCtopologyparticles.h  -  Virtual cartesian topology
                  A virtual topology is a mechanism for naming the processes
                  in a communicator in a way that fits the communication
                  pattern better. Since our processes will communicate mainly
                  with the nearest neighbours after the fashion of a two-dimensional
                  grid, we create a virtual topology to reflect this fact
                             -------------------

 ***************************************************************************/


#include "mpi.h"

#include <iostream>

#include "VCtopologyparticles.h"

using std::cout;
using std::endl;

/**
*
* @date Fri Jun 4 2007
* @author Stefano Markidis, Giovanni Lapenta.
* @version 2.0
*
*/

/** constructor: initialize a virtual cartesian topology */
VCtopologyparticles::VCtopologyparticles(){
    /*************************************************************/
    /***  HERE YOU SET THE TOPOLOGY    XLEN*YLEN = n_procs     ***/
    /*************************************************************/
    /****** PERIODICITY OF PROCESS TOPOLOGY   ***/
    /** IF ONE OF THIS PERIODICITY IS TRUE PERIODICITY IS AUTOMATICALLY IMPOSED ON BC !!!!*/
              /** NO, PERIODICITY MUST BE FORCED TO TRUE TO RUN ON VIC3, BC CHANGED (Beck 7/06/2011) **/
    /**************************************************************/


    PERIODICX = false;
    PERIODICY = false;
    XDIR = 0;
    YDIR = 1;
    RIGHT = 1;
    LEFT = -1;
    reorder = 1;
    divisions = new int[3];
    periodic_divisions = new int[2];
    periods = new int[3];
    periods[0] = false;
    periods[1] = false;
    periods[2] = false;
    coordinates = new int[2];
    global_coordinates = new int[3];
    cVERBOSE = false;
    twoDperiods = new int[2];

}
/** destructor */
VCtopologyparticles::~VCtopologyparticles(){
  delete[] periods;
  delete[] divisions;
  delete[] coordinates;
  delete[] global_coordinates;
  delete[] periodic_divisions;
  delete[] twoDperiods;
}
/** Within CART_COMM, processes find about their new rank numbers, their cartesian coordinates,
    and their neighbors  */
void VCtopologyparticles::setup_vctopology(MPI_Comm old_comm){
      int remain_dims[3];
      PROCDIM =2;
      remain_dims[0]= 0;
      remain_dims[1]= 1;      
      remain_dims[2]= 1;      

    MPI_Cart_create(old_comm, 3, divisions, periods, reorder, &CART_COMM_TOTAL);
    MPI_Cart_sub(CART_COMM_TOTAL,remain_dims, &CART_COMM_test);
    // Trying to make the coarsest level communicator periodic
    MPI_Comm_rank   (CART_COMM_TOTAL, &cartesian_rank);
    MPI_Cart_coords(CART_COMM_TOTAL, cartesian_rank, 3, global_coordinates);
    if (global_coordinates[0] == 0){  //If level == 0
        PERIODICX = twoDperiods[0];
        PERIODICY = twoDperiods[1];
        if (cartesian_rank == 0){
            if (PERIODICX)
                cout << "Topology is periodic in the X direction"<<endl;
            if (PERIODICY)
                cout << "Topology is periodic in the Y direction"<<endl;
        }
        periodic_divisions[0] = divisions[1];
        periodic_divisions[1] = divisions[2];
        MPI_Cart_create(CART_COMM_test, 2, periodic_divisions, twoDperiods, reorder, &CART_COMM);// Gives the correct periodicity to the coarsest grid
    } else {
        CART_COMM = CART_COMM_test;
    } 

    if (CART_COMM != MPI_COMM_NULL){
      int *tempCoor = new int[2];
      int bx, by;
      MPI_Comm_rank   (CART_COMM, &cartesian_rank);
      MPI_Cart_coords (CART_COMM, cartesian_rank, 2, coordinates);

      MPI_Cart_shift  (CART_COMM, XDIR, RIGHT, &xleft_neighbor, &xright_neighbor);
      MPI_Cart_shift  (CART_COMM, YDIR, RIGHT, &yleft_neighbor, &yright_neighbor);
      // know the 12 processors in diagonal that share edges
      /** cartesian rank of XLEFT(-) YLEFT(-) */
      if ( (xleft_neighbor!= MPI_PROC_NULL) && (yleft_neighbor!= MPI_PROC_NULL) ){
          bx =   ((coordinates[0])==(0))?(XLEN):(0);
          by =   ((coordinates[1])==(0))?(YLEN):(0);

          tempCoor[0] = (coordinates[0]-1) + bx;
          tempCoor[1] = (coordinates[1]-1) + by;
          MPI_Cart_rank(CART_COMM,tempCoor,&XleftYleft_neighbor);

      } else {
           XleftYleft_neighbor =  MPI_PROC_NULL;

      }
      /** cartesian rank of XLEFT(-) YRIGHT(+)*/
      if( (xleft_neighbor!= MPI_PROC_NULL) && (yright_neighbor!= MPI_PROC_NULL) ){
        bx = ((coordinates[0])==(0))?(XLEN):(0);
        tempCoor[0] = (coordinates[0]-1) + bx;
        tempCoor[1] = (coordinates[1]+1)%(YLEN);
        MPI_Cart_rank(CART_COMM,tempCoor,&XleftYright_neighbor);

      } else
      {
         XleftYright_neighbor =  MPI_PROC_NULL;
      }
      /** cartesian rank of XRIGHT(+) YLEFT(-) SAME Z neighbor */
      if((xright_neighbor!= MPI_PROC_NULL) && (yleft_neighbor!= MPI_PROC_NULL)){
        by =  ((coordinates[1])==(0))?(YLEN):(0);
        tempCoor[0] = (coordinates[0]+1)%XLEN;
        tempCoor[1] = coordinates[1]-1 + by;
        MPI_Cart_rank(CART_COMM,tempCoor,&XrightYleft_neighbor);
      }  else
      {
         XrightYleft_neighbor =  MPI_PROC_NULL;
      }
      /** cartesian rank of XRIGHT(+) YRIGHT(+) SAME Z neighbor */
      if ((xright_neighbor!= MPI_PROC_NULL) && (yright_neighbor!= MPI_PROC_NULL)){
        tempCoor[0] = (coordinates[0]+1)%XLEN;
        tempCoor[1] = (coordinates[1]+1)%YLEN;
        MPI_Cart_rank(CART_COMM,tempCoor,&XrightYright_neighbor);
      } else
      {
        XrightYright_neighbor =  MPI_PROC_NULL;
      }


    } else
    {
      cout << "A process is trown away from the new topology. VCtopologyparticles.h" << endl;
    }
}

/** print topology info */
void VCtopologyparticles::Print(){
  cout << endl;
  cout << "Virtual Cartesian Processors Topology" << endl;
  cout << "-------------------------------------"<< endl;
  cout << "Processors Topology dimension = " << PROCDIM << endl;
  cout << "Processors grid: " << XLEN << "x" << YLEN << endl;
  cout << "Periodicity X: " << PERIODICX << endl;
  cout << "Periodicity Y: " << PERIODICY << endl;
  cout << "Number of levels : " << ngrids << endl;
  cout << endl;
}
/** print cartesian rank of neighbors and coordinate of process */
void VCtopologyparticles::PrintMapping(){
  cout << endl;
  cout << "Mapping of process " << cartesian_rank << endl;
  cout << "----------------------" << endl;
  cout << "Coordinates: X = " << coordinates[0] << "; Y = " << coordinates[1] << endl;
  cout << "Neighbors: xLeft = " << xleft_neighbor << "; xRight = " << xright_neighbor << "; yLeft = " << yleft_neighbor << "; yRight = "<< yright_neighbor << endl;
  cout << "Neighbors: xRightYright = " << XrightYright_neighbor << "; xRightYleft = " << XrightYleft_neighbor << "; xLeftYright = " << XleftYright_neighbor << "; xLeftYleft = "<< XleftYleft_neighbor << endl;

  cout << endl;
}
/** get and set XLEN */
int VCtopologyparticles::getXLEN(){
  return(XLEN);
}
void VCtopologyparticles::setXLEN(int XLENinput){
  XLEN = XLENinput;
}
/** get and set YLEN */
int VCtopologyparticles::getYLEN(){
  return(YLEN);
}
void VCtopologyparticles::setYLEN(int YLENinput){
  YLEN = YLENinput;
}
/** get CART_COMM and CART_COMM_TOTAL **/
MPI_Comm VCtopologyparticles::getCART_COMM(){
   return(CART_COMM);
}
MPI_Comm VCtopologyparticles::getCART_COMM_TOTAL(){
   return(CART_COMM_TOTAL);
}
/** set divisions **/
void VCtopologyparticles::setDivisions(){
    divisions[0] = ngrids;
    divisions[1] = XLEN;
    divisions[2] = YLEN;
}
/** get and set nprocs */
int VCtopologyparticles::getNprocs(){
  return(nprocs);
}
void VCtopologyparticles::setNprocs(){
  nprocs = XLEN*YLEN;
}
/** get and set ngrids */
int VCtopologyparticles::getNgrids(){
  return(ngrids);
}
void VCtopologyparticles::setNgrids(int ngridinput){
  ngrids = ngridinput;
}
/** get periodicity on boundaries - DIRECTION X*/
bool VCtopologyparticles::getPERIODICX(){
  return(PERIODICX);
}
/** get periodicity on boundaries - DIRECTION Y*/
bool VCtopologyparticles::getPERIODICY(){
  return(PERIODICY);
}
/** set periodicity **/
void VCtopologyparticles::setPeriodicity(int xleft,int xright,int yleft,int yright){
  if(xleft == 2|| xright ==2){
      twoDperiods[0] = true;
  } else {
      twoDperiods[0] = false;
  }
  if(yleft == 2|| yright ==2){
      twoDperiods[1] = true;
  } else {
      twoDperiods[1] = false;
  }
}
/** get the cartesian rank of the process */
int VCtopologyparticles::getCartesian_rank(){
  return(cartesian_rank);
}

// AMR ME 
// get the rank of the process in MPI_COMM_WORLD
int VCtopologyparticles::getCartesian_rank_COMMTOTAL(){
  int r;
  MPI_Comm_rank(CART_COMM_TOTAL, &r);
  return r;
}

int VCtopologyparticles::getRank_MPI_COMM_WORLD(){
  int r;
  MPI_Comm_rank(MPI_COMM_WORLD, &r);
  return r;
}
// end AMR ME 
/** get the cartesian rank of XLEFT neighbor */
int VCtopologyparticles::getXleft_neighbor(){
  return(xleft_neighbor);
}
/** get the cartesian rank of XRIGHT neighbor */
int VCtopologyparticles::getXright_neighbor(){
  return(xright_neighbor);
}
/** get the cartesian rank of YLEFT neighbor */
int VCtopologyparticles::getYleft_neighbor(){
  return(yleft_neighbor);
}
/** get the cartesian rank of YRIGHT neighbor */
int VCtopologyparticles::getYright_neighbor(){
  return(yright_neighbor);
}
/** get the cartesian rank of XLEFT(-) YLEFT(-) SAME Z neighbor  */
int VCtopologyparticles::getXleftYleft_neighbor(){
  return(XleftYleft_neighbor);
}
/** cartesian rank of XLEFT(-) YRIGHT(+) SAME Z neighbor */
int VCtopologyparticles::getXleftYright_neighbor(){
  return(XleftYright_neighbor);
}
/** cartesian rank of XRIGHT(+) YLEFT(-) SAME Z neighbor */
int VCtopologyparticles::getXrightYleft_neighbor(){
  return(XrightYleft_neighbor);
}
/** cartesian rank of XRIGHT(+) YRIGHT(+) SAME Z neighbor */
int VCtopologyparticles::getXrightYright_neighbor(){
  return(XrightYright_neighbor);
}



/** if cVERBOSE == true, print to the screen all the comunication */
bool VCtopologyparticles::getcVERBOSE(){
   return(cVERBOSE);
}
/** get the coordinates in dir direction of process*/
int VCtopologyparticles::getCoordinates(int dir){
   return(coordinates[dir]);
}
/** get Periodicity condition in dir direction */
int VCtopologyparticles::getPeriods(int dir){
   return(periods[dir]);
}

// set RefLevelAd                                                                                                                                    
void VCtopologyparticles::setRefLevelAdj(int val)
{
  RefLevelAdj= val;
}
// get RefLevelAd                                                                                                                                    
int VCtopologyparticles::getRefLevelAdj()
{
  return RefLevelAdj;
}