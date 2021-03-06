/***************************************************************************
                          CollectiveIO.h  -  Abstract Base class
                             -------------------
    begin                : Wed Jun 2 2004
    copyright            : (C) 2004 Los Alamos National Laboratory
    developers           : Stefano Markidis, Giovanni Lapenta
    email                : markidis@lanl.gov, lapenta@lanl.gov
 ***************************************************************************/

#ifndef CollectiveIO_H
#define CollectiveIO_H
#include <string>
#include <iostream>

using namespace std;

/**
*  Abstract base class for inputing physical parameters for simulation.
*g
* @date Fri Jun 4 2004
* @par Copyright:
* (C) 2004 Los Alamos National Laboratory
* @author Stefano Markidis, Giovanni Lapenta
* @version 1.0
*/
class CollectiveIO {
  public:
    /** print simulation parameters */
    virtual void Print(void)=0;
    /** get simulation box length - direction X */
    virtual double getLx(void)=0;
    /** get simulation box length - direction Y */
    virtual double getLy(void)=0;
    /** Get central position of the refined grid - direction X */
    virtual double getL1_CX()=0;
    /** Get central position of the refined grid - direction Y */
    virtual double getL1_CY()=0;
    /** get number of cells - direction X */
    virtual int getNxc(void)=0;
    /** get number of cells - direction Y */
    virtual int getNyc(void)=0;
    /** get grid spacing - direction X */
    virtual double getDx(void)=0;
    /** get grid spacing - direction Y */
    virtual double getDy(void)=0;
    /** get the time step */
    virtual double getDt()=0;
    /** get the SubCycling */
    virtual int getSubCycling()=0;
    /** get the TimeRatio */
    virtual int getTimeRatio()=0;
    /** get the decentering parameter */
    virtual double getTh()=0;
    /** get the Smoothing value*/
    virtual double getSmooth()=0;
    /** get the Smoothing times*/
    virtual int getNvolte()=0;
    /** get the number of time cycles */
    virtual int getNcycles()=0;
    /** get the number of grids */
    virtual int getNgrids()=0;
    /** get the size ratio between grids */
    virtual int getRatio()=0;
    /** get the number of processors x - direction */
    virtual int getXLEN()=0;
    /** get the number of processors y - direction */
    virtual int getYLEN()=0;
    /** get the number of species */
    virtual int getNs()=0;
    /** get the number of particles array for different species */
    virtual int getNp(int nspecies)=0;
    /** get the number of particles per cell  */
    virtual int getNpcel(int nspecies)=0;
    /** get the number of particles per cell - direction X  */
    virtual int getNpcelx(int nspecies)=0;
    /** get the number of particles per cell - direction Y  */
    virtual int getNpcely(int nspecies)=0;
    /** get maximum number of particles for different species */
    virtual int getNpMax(int nspecies)=0;
	/** get NpMax/Np is the ratio between the maximum number of particles allowed on a processor and the number of particles*/
	virtual double getNpMaxNpRatio()=0;
    /** get charge to mass ratio for different species */
    virtual double getQOM(int nspecies)=0;
    /** get thermal velocity  - X direction    */
    virtual double getUth(int nspecies)=0;
    /** get thermal velocity  - Y direction    */
    virtual double getVth(int nspecies)=0;
    /** get thermal velocity  - Z direction    */
    virtual double getWth(int nspecies)=0;
    /** get Drift velocity - Direction X         */
    virtual double getU0(int nspecies)=0;
    /** get Drift velocity - Direction Y         */
    virtual double getV0(int nspecies)=0;
    /** get Drift velocity - Direction Z         */
    virtual double getW0(int nspecies)=0;
    /** get the boolean value for TrackParticleID */
    virtual bool getTrackParticleID(int nspecies)=0;
    /** get SaveDirName  */
    virtual string getSaveDirName()=0;
    /** get RestartDirName  */
    virtual string getRestartDirName()=0;
    /** get inputfile  */
    virtual string getinputfile()=0;



    /** get Boundary Condition Particles: FaceXright */
    virtual int getBcPfaceXright()=0;
    /** get Boundary Condition Particles: FaceXleft */
    virtual int getBcPfaceXleft()=0;
    /** get Boundary Condition Particles: FaceYright */
    virtual int getBcPfaceYright()=0;
    /** get Boundary Condition Particles: FaceYleft */
    virtual int getBcPfaceYleft()=0;

    /** get Boundary Condition Electrostatic Potential: FaceXright */
    virtual int getBcPHIfaceXright()=0;
    /** get Boundary Condition Electrostatic Potential:FaceXleft */
    virtual int getBcPHIfaceXleft()=0;
    /** get Boundary Condition Electrostatic Potential:FaceYright */
    virtual int getBcPHIfaceYright()=0;
    /** get Boundary Condition Electrostatic Potential:FaceYleft */
    virtual int getBcPHIfaceYleft()=0;

    /** get Boundary ConditionElectric Field: FaceXright */
    virtual int getBcEMfaceXright()=0;
    /** get Boundary Condition Electric Field: FaceXleft */
    virtual int getBcEMfaceXleft()=0;
    /** get Boundary Condition Electric Field: FaceYright */
    virtual int getBcEMfaceYright()=0;
    /** get Boundary Condition Electric Field: FaceYleft */
    virtual int getBcEMfaceYleft()=0;

    /** Get GEM Challenge parameters */
    virtual double getRHOinit(int ns)=0;
    virtual double getDelta()=0;
    virtual double getB0x()=0;
    virtual double getB0y()=0;
    virtual double getB0z()=0;
    /** get the boolean value for verbose results */
    virtual bool getVerbose()=0;
    /** RESTART */
    virtual int getRestart_status()=0;
	// get CONSTANTS
	/** get the light speed */
	virtual double getC()=0;

	/** get the converging tolerance for CG solver */
	virtual double getCGtol()=0;
	/** get the converging tolerance for GMRES solver */
	virtual double getGMREStol()=0;
	/** get the numbers of iteration for the PC mover */
	virtual int getNiterMover()=0;

	/** output of fields */
	virtual int getFieldOutputCycle()=0;
	/** output of fields */
	virtual int getParticlesOutputCycle()=0;
	/** output of fields */
	virtual int getRestartOutputCycle()=0;

	/** get the velocity of injection of the plasma from the wall */
    virtual double getVinj()=0;

    /**AMR variables, ME*/
    virtual int GetPRA_Xleft()=0;
    virtual int GetPRA_Xright()=0;
    virtual int GetPRA_Yleft()=0;
    virtual int GetPRA_Yright()=0;



};
#endif
