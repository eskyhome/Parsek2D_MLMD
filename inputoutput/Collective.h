/***************************************************************************                     
 Collective.h  -  Stefano Markidis, Giovanni Lapenta                                              
 -------------------                                                                              
                                                                                              
                                                                                                  
 /**                                                                                              
 *  Collective properties. Input physical parameters for the simulation.                          
 *  Use ConfigFile to parse the input file                                                        
 *                                                                                                
 * @date Wed Jun 8 2011                                                                           
 * @par Copyright:                                                                                
 * (C) 2013 K.U. LEUVEN                                                 
 * @author Maria Elena Innocenti, Pierre Henri, Stefano Markidis     
 * @version 1.0 
 */

#ifndef Collective_H
#define Collective_H

#include "CollectiveIO.h"

#include "../ConfigFile/src/ConfigFile.h"
#include "../ConfigFile/src/input_array.h"

using std::cout;
using std::endl;
using std::ofstream;
using namespace std;

class Collective : public CollectiveIO {
  public:
    /** constructor: initialize physical parameters with values */
    Collective(int argc, char** argv);
    /** destructor */
    ~Collective();
    /** read input file */
    void ReadInput(string inputfile);
    /** read the restart input file from HDF5 */
    int ReadRestart(string inputfile);
    /** Print physical parameters */
    void Print();
    /** get the physical space dimensions            */
    int getDim();
    /** Get length of the system - direction X */
    double getLx();
    /** Get length of the system - direction Y */
    double getLy();
    /** Get central position of the refined grid - direction X */
    double getL1_CX();
    /** Get central position of the refined grid - direction Y */
    double getL1_CY();
    /** Get the number of cells - direction X*/
    int getNxc();
    /** Get the number of cells - direction Y*/
    int getNyc();
    /** Get the grid spacing - direction X*/
    double getDx();
    /** Get the grid spacing - direction Y*/
    double getDy();
    /** get the time step */
    double getDt();
    /** get if subCycling */
    int getSubCycling();
    /** get the Time Ratio between the grids */
    int getTimeRatio();
    /** get the decentering parameter */
    double getTh();
    /** get the Smoothing value*/
    double getSmooth();
    /** get the Smoothing times*/
    int getNvolte();
    /** get the number of time cycles */
    int getNcycles();
    /** get the number of grids */
    int getNgrids();
    /** get the number of processors - x direction */
    int getXLEN();
    /** get the number of processors - y direction */
    int getYLEN();
    /** get the size ratio */
    int getRatio();
    /** get the number of species */
    int getNs();
    /** get the number of particles for different species */
    int getNp(int nspecies);
    /** get the number of particles per cell  */
    int getNpcel(int nspecies);
    /** get the number of particles per cell - direction X  */
    int getNpcelx(int nspecies);
    /** get the number of particles per cell - direction Y  */
    int getNpcely(int nspecies);
    /** get maximum number of particles for different species */
    int getNpMax(int nspecies);
	/** NpMax/Np is the ratio between the maximum number of particles allowed on a processor and the number of particles*/
    double getNpMaxNpRatio();
    /** get charge to mass ratio for different species */
    double getQOM(int nspecies);
	 /** get background charge for GEM challenge */
    double getRHOinit(int nspecies);
    /** get thermal velocity  - X direction    */
    double getUth(int nspecies);
    /** get thermal velocity  - Y direction    */
    double getVth(int nspecies);
    /** get thermal velocity  - Z direction    */
    double getWth(int nspecies);
    /** get Drift velocity - Direction X         */
    double getU0(int nspecies);
    /** get Drift velocity - Direction Y         */
    double getV0(int nspecies);
    /** get Drift velocity - Direction Z         */
    double getW0(int nspecies);
    /** get the boolean value for TrackParticleID */
    bool getTrackParticleID(int nspecies);
    /** get SaveDirName  */
    string getSaveDirName();
    /** get last_cycle  */
    int getLast_cycle();
    /** get RestartDirName  */
    string getRestartDirName();

	/** get the velocity of injection of the plasma from the wall */
    double getVinj();

	/** get the converging tolerance for CG solver */
	double getCGtol();
	/** get the converging tolerance for GMRES solver */
	double getGMREStol();
	/** get the numbers of iteration for the PC mover */
	int getNiterMover();

	/** output of fields */
	int getFieldOutputCycle();
	/** output of fields */
	int getParticlesOutputCycle();
	/** output of fields */
	int getRestartOutputCycle();

    /** get Boundary Condition Particles: FaceXright */
    int getBcPfaceXright();
    /** get Boundary Condition Particles: FaceXleft */
    int getBcPfaceXleft();
    /** get Boundary Condition Particles: FaceYright */
    int getBcPfaceYright();
    /** get Boundary Condition Particles: FaceYleft */
    int getBcPfaceYleft();

    /** get Boundary Condition Electrostatic Potential: FaceXright */
    int getBcPHIfaceXright();
    /** get Boundary Condition Electrostatic Potential:FaceXleft */
    int getBcPHIfaceXleft();
    /** get Boundary Condition Electrostatic Potential:FaceYright */
    int getBcPHIfaceYright();
    /** get Boundary Condition Electrostatic Potential:FaceYleft */
    int getBcPHIfaceYleft();

    /** get Boundary ConditionElectric Field: FaceXright */
    int getBcEMfaceXright();
    /** get Boundary Condition Electric Field: FaceXleft */
    int getBcEMfaceXleft();
    /** get Boundary Condition Electric Field: FaceYright */
    int getBcEMfaceYright();
    /** get Boundary Condition Electric Field: FaceYleft */
    int getBcEMfaceYleft();

    /** get RESTART */
    int getRestart_status();

	/** Get the velocity of light in vacuum */
	double getC();

	/** Get GEM Challenge parameters */
	/** get the sheet thickness */
	double getDelta();
	/** get the amplitude of the magnetic field along x */
    double getB0x();
    	/** get the amplitude of the magnetic field along y */
    double getB0y();
	/** get the amplitude of the magnetic field along z */
    double getB0z();
    /** get the boolean value for verbose results */
    bool getVerbose();

    /**AMR variables, ME*/
    int GetPRA_Xleft();
    int GetPRA_Xright();
    int GetPRA_Yleft();
    int GetPRA_Yright();

  private:
    /** inputfile */
    string inputfile;
    /** light speed */
    double c;
    /** 4 greek pi */
    double fourpi;
    /** time step */
    double dt;
    /** SubCycling */
    int SubCycling;
    /** TimeRatio */
    int TimeRatio;
    /** decentering parameter */
    double th;
    /** Smoothing value*/
    int Smooth;
    /** Smoothing times*/
    int Nvolte;
    /** number of time cycles */
    int ncycles;
    /** physical space dimensions            */
    int dim;
    /** simulation box length - X direction   */
    double Lx;
    /** simulation box length - Y direction   */
    double Ly;
    /** central position of the refined grid - direction X */
    double L1_CX;
    /** central position of the refined grid - direction Y */
    double L1_CY;
    /** number of cells - X direction        */
    int nxc;
    /** number of cells - Y direction        */
    int nyc;
    /** grid spacing - X direction */
    double dx;
    /** grid spacing - Y direction */
    double dy;
    /** Number of grids **/
    int ngrids;
    /** Size ratio between two consecutive levels **/
    int ratio;
    /** number of processors x - direction */
    int XLEN;
    /** number of processors y - direction */
    int YLEN;
    /** number of species */
    int ns;
	/** NpMax/Np is the ratio between the maximum number of particles allowed on a processor and the number of particles*/
	double NpMaxNpRatio;
    /** number of particles per cell */
    int *npcel;
    /** number of particles per cell - X direction */
    int *npcelx;
    /** number of particles per cell - Y direction */
    int *npcely;
    /** number of particles array for different species */
    int *np;
    /** maximum number of particles array for different species */
    int *npMax;
    /** charge to mass ratio array for different species */
    double *qom;
	/** charge to mass ratio array for different species */
    double *rhoINIT;
    /** thermal velocity  - Direction X  */
    double *uth;
    /** thermal velocity  - Direction Y  */
    double *vth;
    /** thermal velocity  - Direction Z  */
    double *wth;
    /** Drift velocity - Direction X     */
    double *u0;
    /** Drift velocity - Direction Y    */
    double *v0;
    /** Drift velocity - Direction Z    */
    double *w0;





    /** TrackParticleID     */
    bool *TrackParticleID;
    /** SaveDirName     */
    string SaveDirName;
    /** RestartDirName     */
    string RestartDirName;
    /** get inputfile  */
    string getinputfile();
    /** restart_status  0 --> no restart; 1--> restart, create new; 2--> restart, append; */
    int restart_status;
    /** last cycle */
    int last_cycle;

    /** Boundary condition on particles
    0 = exit
    1 = perfect mirror
    2 = riemission
    */
    /** Boundary Condition Particles: FaceXright */
    int bcPfaceXright;
    /** Boundary Condition Particles: FaceXleft */
    int bcPfaceXleft;
    /** Boundary Condition Particles: FaceYright */
    int bcPfaceYright;
    /** Boundary Condition Particles: FaceYleft */
    int bcPfaceYleft;


    /** Field Boundary Condition
      0 =  Dirichlet Boundary Condition: specifies the valueto take pn the boundary of the domain
      1 =  Neumann Boundary Condition: specifies the value of derivative to take on the boundary of the domain
      2 =  Periodic Condition
    */
    /** Boundary Condition Electrostatic Potential: FaceXright */
    int bcPHIfaceXright;
    /** Boundary Condition Electrostatic Potential:FaceXleft */
    int bcPHIfaceXleft;
    /** Boundary Condition Electrostatic Potential:FaceYright */
    int bcPHIfaceYright;
    /** Boundary Condition Electrostatic Potential:FaceYleft */
    int bcPHIfaceYleft;

    /** Boundary Condition EM Field: FaceXright */
    int bcEMfaceXright;
    /** Boundary Condition EM Field: FaceXleft */
    int bcEMfaceXleft;
    /** Boundary Condition EM Field: FaceYright */
    int bcEMfaceYright;
    /** Boundary Condition EM Field: FaceYleft */
    int bcEMfaceYleft;


    /** GEM Challenge parameters */
    /** current sheet thickness */
	double delta;
	/* Amplitude of the field */
    double B0x;
    double B0y;
    double B0z;

    /** boolean value for verbose results */
    bool verbose;
    /** RESTART */
    bool RESTART1;

	/** velocity of the injection from the wall */
	double Vinj;

	/** CG solver stopping criterium tolerance */
	double CGtol;
	/** GMRES solver stopping criterium tolerance */
	double GMREStol;
	/** mover predictor correcto iteration */
	int NiterMover;

	/** Output for field */
	int FieldOutputCycle;
	/** Output for particles */
	int ParticlesOutputCycle;
	/** restart cycle */
	int RestartOutputCycle;

   /**AMR variables, ME*/
   /**number of cells, ghost cell INCLUDED, for particle repopulation; x left*/
   int PRA_Xleft;
   /**number of cells, ghost cell INCLUDED, for particle repopulation; x right*/
   int PRA_Xright;
   /**number of cells, ghost cell INCLUDED, for particle repopulation; y left*/
   int PRA_Yleft;
   /**number of cells, ghost cell INCLUDED, for particle repopulation; y right*/
   int PRA_Yright;



} ;

#endif







