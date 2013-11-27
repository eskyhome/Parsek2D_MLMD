/********************************************************************************************
ParsekEM.cpp  - A main file for running a parallel Particle-in-Cell with Electromagnetic field
			        -------------------
developers: Stefano Markidis, Enrico Camporeale, Giovanni Lapenta, David Burgess
********************************************************************************************/


// MPI
#include "mpi.h"
#include "mpidata/MPIdata.h"
#include "hdf5.h"
// topology
#include "processtopology/VirtualTopology.h"
#include "processtopology/VCtopology.h"
#include "processtopology/VCtopologyparticles.h"
// input
#include "inputoutput/CollectiveIO.h"
#include "inputoutput/Collective.h"
// grid
#include "grids/Grid.h"
#include "grids/Grid2DCU.h"
// fields
#include "fields/Field.h"
#include "fields/EMfields.h"
// particle
#include "particles/Particles.h"
#include "particles/Particles2Dcomm.h"
#include "particles/Particles2D.h"
//  output
#include "PSKOutput2D/PSKOutput.h"
#include "PSKOutput2D/PSKhdf5adaptor.h"
#include "inputoutput/Restart.h"
#include "inputoutput/SerialIO.h"
// performance
#include "performances/Timing.h"
// wave
//#include "perturbation/Planewave.h"


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using std::cerr;
using std::endl;




int main (int argc, char **argv) {

  bool TEST=0; //this takes a lot od reduces, put to 0 normally
  bool TEST_B=0; // to eliminate the barriers; when debugging, the barriers mark the beginning/ end of each phase

  int proj=1;
 int interp=1;
 int solveFields=1;
 int P2Gops=1;
 int PRASendRcvOps=1; 
 int PRACollectionMethod=1;   /* =0, coarse particle to be used for repopulation collected one by one during communicate ops; WORKING ON
				    =1, coarse particles collected all together after mover */
 int RefLevelAdj=0; /* options:                                                                      
		      --0: same adjust for coarse and refined level (multiply)-- chec with double periodic reconnection                      
		      --1: interp of OS particles for the refined level; more expensive, use it for influxes */

 // initialize MPI environment
 int nprocs, myrank, mem_avail;
 MPIdata *mpi = new MPIdata(&argc,&argv);
 nprocs = mpi->nprocs; // nprocs = number of processors
 myrank = mpi->rank;   // myrank = rank of the process (ID)

 Collective *col = new Collective(argc,argv); // Every proc loads the parameters of simulation from class Collective
 bool verbose = col->getVerbose();
 string SaveDirName = col->getSaveDirName();
 string RestartDirName = col->getRestartDirName();
 int level; //Level of the grid the process is living on
 const int restart = col->getRestart_status();
 const int ns = col->getNs(); // get the number of particle species involved in simulation
 const int ngrids = col->getNgrids(); // get the number of particle species involved in simulation
 const int first_cycle = col->getLast_cycle()+1; // get the last cycle from the restart
 double dt = col->getDt();
 // initialize the virtual cartesian topology
 VCtopology *vct = new VCtopology();
 VCtopologyparticles *vctparticles = new VCtopologyparticles();
 vct->setXLEN(col->getXLEN());
 vct->setYLEN(col->getYLEN());
 vct->setNgrids(ngrids);
 vct->setDivisions();
 vct->setNprocs();
 vct->setPeriodicity(col->getBcEMfaceXleft(),col->getBcEMfaceXright(),col->getBcEMfaceYleft(),col->getBcEMfaceYright());
 vctparticles->setXLEN(col->getXLEN());
 vctparticles->setYLEN(col->getYLEN());
 vctparticles->setNgrids(ngrids);
 vctparticles->setDivisions();
 vctparticles->setNprocs();
 vctparticles->setPeriodicity(col->getBcEMfaceXleft(),col->getBcEMfaceXright(),col->getBcEMfaceYleft(),col->getBcEMfaceYright());
 vctparticles->setRefLevelAdj(RefLevelAdj);
 vct->setRefLevelAdj(RefLevelAdj);

 if (nprocs/ngrids != vct->getNprocs()){ // Check if we can map the processes into a matrix ordering defined in Collective.cpp
    if (myrank == 0 ) {
      cerr << "Error: " << nprocs << " processes cant be mapped into " << vct->getXLEN() << "x" << vct->getYLEN() << " matrix: Change XLEN,YLEN in method VCTopology.init()" << endl;
      mpi->finalize_mpi();
      return(1);
    }
  }
  vct->setup_vctopology(MPI_COMM_WORLD); // field 2D Cartesian Topology
  vctparticles->setup_vctopology(MPI_COMM_WORLD); // particles 2D Cartesian Topology
  // initialize the central cell index
  const int nx0 = col->getNxc() / vct->getXLEN();// get the number of cells in x for each processor
  const int ny0 = col->getNyc() / vct->getYLEN();// get the number of cells in y for each processor
  // Print the initial settings from the INPUT file
  if (myrank==0){
	  mpi->Print();
	  vct->Print();
	  col->Print();
  }
  cout << "Prints done"<<endl;

/*** Check grid and particles are on the same level ***/
int coord[3], coord_particles[3];
MPI_Cart_coords(vct->getCART_COMM_TOTAL(), myrank, 3, coord);
MPI_Cart_coords(vctparticles->getCART_COMM_TOTAL(), myrank, 3, coord_particles);
if (coord[0] != coord_particles[0]) {
    if (myrank == 0 ) {
      cerr << "Error: " << myrank << " proces is not on the same level for grid and particles" << endl;
      mpi->finalize_mpi();
      return(1);
    }
 }
  cout << "Checks initializations done"<<endl;

/******************************************************/
  level = coord[0];
  Grid2DCU* grid = new Grid2DCU(col,vct,level); // Create the grids

  EMfields *EMf = new EMfields(col, grid, vct); // Create Electromagnetic Fields Object
  cout << "Grids and fields constructions done"<<endl;
  // Initial Condition for FIELD if you are not starting from RESTART
  //EMf->initUniform(vct,grid); // initialize with constant values
  EMf->initDoubleHarris(vct,grid); // initialize with constant values 
  //EMf->initLightwave(vct,grid); // initialize with a dipole
  cout << "vct->getNgrids() " << vct->getNgrids() <<endl;
  cout << "interp " << interp <<endl;
  if (vct->getNgrids()>1 && interp)
    {
      int initBCres;
      initBCres=EMf->initWeightBC(vct,grid,col);// Initialize the weight for interpolation of BC between grids
      if (initBCres<0)
	{cout <<"problems with initWeightBC, exiting...\n";
	  mpi->Abort();
	  return (-1);
	}
    }

  if (TEST_B) 
    {
      MPI_Barrier(vct->getCART_COMM_TOTAL()) ;
      if (vct->getCartesian_rank_COMMTOTAL()==0)
	{
	  cout << "initWeightBC ended well\n";
	}
    }// end TEST
  
  if (vct->getNgrids()>1 && proj)
    {
      int initProjres;
      initProjres=EMf->initWeightProj(vct,grid,col);// Initialize the weight for projection of refined fields between grids
      if (initProjres<0)
        {cout <<"problems with initWeightProj, exiting...\n";
          mpi->Abort();
          return (-1);
        }
    }

  if (TEST_B)
    {
      MPI_Barrier(vct->getCART_COMM_TOTAL()) ;
      if (vct->getCartesian_rank_COMMTOTAL()==0)
	{
	  cout << "initWeightProj ended well\n";
	}
    }  // end TEST
  // Allocation of particles
  Particles2D *part = new Particles2D[ns];
  
  if (TEST_B)
    {
      MPI_Barrier(vct->getCART_COMM_TOTAL());
      if (vct->getCartesian_rank_COMMTOTAL()==0)
	{
	  cout << "I am starting allocating particles\n";
	}
    }// end TEST


  // end for debugging
  for (int i=0; i < ns; i++)
    {
      // init operations for particle repopulation
      part[i].setPRACollectionMethod(PRACollectionMethod);
      part[i].allocate(i,col,vctparticles,grid);
      //cout << "R"<< myrank <<"Before initPRAVariables, is " <<i <<endl;
      int PRASucc=part[i].initPRAVariables(i, col, vctparticles, grid, EMf);
      //int PRASucc=1;
      //cout << "R"<< myrank <<"After initPRAVariables, is " <<i <<endl;
      if (PRASucc<0)
	{
	  cout << "*******************************************************************************************" << endl;
	  cout << "PRA parameters need to be rechecked (most likely, overlapping PRA areas), stopping the sim)" << endl;
	  cout << "*******************************************************************************************" << endl;
	  mpi->Abort();
	  return (-1);
	}
      part[i].checkAfterInitPRAVariables(i, col, vctparticles, grid);
    }
  cout << "Allocating particle done"<<endl;

  // Initial Condition for PARTICLES if you are not starting from RESTART
  if (restart==0)
  {
    for (int i=0; i < ns; i++)
      {
	//part[i].maxwellian(grid,EMf,vctparticles);  // all the species have Maxwellian distribution in the velocity
	part[i].DoubleHarris(grid,EMf,vctparticles);
	//int out;
	//out =part[i].maxwellian_sameParticleInit(grid,EMf,vctparticles);
	//if (out<0)
	//  {
	//    cout << "Error with maxwellian_sameParticleInit"<<endl;
	//    mpi->Abort();
	//    return -1;
	//    }
	//part[i].maxwellian_ball(grid,EMf,vctparticles);  // all the species have Maxwellian distribution in the velocity
      } 
  }// end restart
  // Initialize the output (simulation results and restart file)
  PSK::OutputManager< PSK::OutputAdaptor > output_mgr;  // Create an Output Manager
  myOutputAgent< PSK::HDF5OutputAdaptor > hdf5_agent; // Create an Output Agent for HDF5 output
  hdf5_agent.set_simulation_pointers(EMf, grid, vct, mpi, col);
  for (int i=0 ; i<ns ; ++i)
      hdf5_agent.set_simulation_pointers_part(&part[i]);
  output_mgr.push_back( &hdf5_agent ); // Add the HDF5 output agent to the Output Manager's list
  if (myrank ==0 & restart<2){
	  hdf5_agent.open(SaveDirName+"/settings.hdf");  // write in proc dir
	  output_mgr.output("collective + total_topology + proc_topology",0);
	  hdf5_agent.close();
	  hdf5_agent.open(RestartDirName+"/settings.hdf"); // write in restart dir
	  output_mgr.output("collective + total_topology + proc_topology",0);
	  hdf5_agent.close();
  }
  stringstream num_proc;
  num_proc << myrank;
  if (restart==0){ // new simulation from input file
    hdf5_agent.open(SaveDirName+"/proc"+num_proc.str()+".hdf");
    output_mgr.output("proc_topology ",0);
	hdf5_agent.close();
    hdf5_agent.open(SaveDirName+"/part"+num_proc.str()+".hdf");
    output_mgr.output("proc_topology ",0);
	hdf5_agent.close();
  }
  else{ // restart append the results to the previous simulation
    hdf5_agent.open_append(SaveDirName+"/proc"+num_proc.str()+".hdf");
	output_mgr.output("proc_topology ",0);
	hdf5_agent.close();
    hdf5_agent.open_append(SaveDirName+"/part"+num_proc.str()+".hdf");
	output_mgr.output("proc_topology ",0);
	hdf5_agent.close();
  }
 
  //  Conserved Quantities

  double Eenergy, Benergy, TOTKenergy, TOTmomentum;
  double *Kenergy;
  double *momentum;
  string cq;
  Kenergy = new double[ns];
  momentum = new double[ns];
  if (TEST)
    {
      stringstream levelstr;
      levelstr << level;
      cq = SaveDirName + "/ConservedQuantities_"+ levelstr.str() +".txt";
      if (vct->getCartesian_rank() == 0) { // this is the rank on the level
	ofstream my_file(cq.c_str());
	my_file.close();
      }
    }
  if (TEST_B)
    {
      cout<<"R"<<myrank << " Init ops done"<<endl;
      MPI_Barrier(vct->getCART_COMM()) ;
      if (! (vct->getCartesian_rank_COMMTOTAL()%(vct->getXLEN()*vct->getYLEN()))   )
	cout << "Level " << level << ": Init ops done\n";
  
    }//end TEST
  //*******************************************//<<
  //****     Start the  Simulation!         ***//
  //*******************************************//

  // time stamping
  Timing *my_clock = new Timing(myrank);
  // end time stamping

  for (int cycle = first_cycle; cycle < (col->getNcycles()+first_cycle); cycle++)
  {
    if (myrank==0 && verbose)
    {
      cout << "***********************" << endl;
      cout << "*   cycle = " << cycle + 1 <<"        *" << endl;
      cout << "***********************" << endl;
    }
    if (P2Gops)
    {
      
      EMf->setZeroDensities(); // set to zero the densities
      for (int i=0; i < ns; i++)
      {
	part[i].interpP2G(EMf,grid,vct); // interpolate Particles to Grid(Nodes)
	if (cycle >0 && level >0 && RefLevelAdj==1)
	  {
	    int out= part[i].interpP2G_OS(EMf,grid,vct); 
	    if (out<0)
	      {
		cout << "Error with interpP2G_OS"<<endl;
		mpi->Abort();
		return -1;
		}
	  }
      }
       
      // adjustNonPeriodicDensities taken out of sumOverSpecies
      EMf->adjustNonPeriodicDensities(vct, cycle);

      EMf->sumOverSpecies(vct);               // correct boundaries if necessary and sum the charge densities  all over the species
      MPI_Barrier(vct->getCART_COMM());
      
      EMf->interpDensitiesN2C(vct,grid);   // calculate densities on centers from nodes

      EMf->calculateHatFunctions(grid,vct); // calculate the hat quantities for the implicit method
    }// end P2Gops      
    MPI_Barrier(vct->getCART_COMM());
	  
    // OUTPUT to large file, called proc**
    if (cycle%(col->getFieldOutputCycle())==0 || cycle==first_cycle)
    {
      hdf5_agent.open_append(SaveDirName+"/proc"+num_proc.str()+".hdf");
      output_mgr.output("k_energy + E_energy + B_energy + pressure",cycle);
      output_mgr.output("Eall + Ball + rhos + rho + phi + Jsall",cycle);
      hdf5_agent.close();

    }
    if (cycle%(col->getParticlesOutputCycle())==0 && col->getParticlesOutputCycle()!=1)
    {
      hdf5_agent.open_append(SaveDirName+"/part"+num_proc.str()+".hdf");
      output_mgr.output("position + velocity + q +ID",cycle, 1);
      hdf5_agent.close();
    }
    if (interp)
    {
      if (level > 0)
      {
	//cout << "receive BC" << endl;
	//fflush(stdout);
	EMf->receiveBC(grid,vct, col);
      }
    }//end interp
    // solve Maxwell equations
    if (solveFields)
    {
      EMf->calculateField(grid,vct); // calculate the EM fields
    }

    //If needed: receive or send  boundary conditions to other levels
    if (interp)
    {
      if (level < ngrids - 1 && ngrids >1)
      {
	EMf->sendBC(grid,vct);
      }
    }//end intero
    //If needed: receive or send refined fields from/to finer/coarser levels 
    if(proj)
    {
      if (level > 0)
      {
	//cout << "send Proj" << endl;
	//fflush(stdout);
	EMf->sendProjection(grid,vct);
      }
      if (level < ngrids - 1)
      {
	EMf->receiveProjection(col,grid,vct);
      }
    }// end projection
    if ((cycle%(col->getFieldOutputCycle())==0 || cycle==first_cycle) and TEST){
	    EMf->outputghost(vct, col, cycle);
	}
    // Here we made the assumption that level n can be updated by non updated level n+1.
            
    for (int i=0; i < ns; i++) // move each species
    {
      // initialize the buffers for communicatin of PRA particles from the coarser to the finer grids
      part[i].initPRABuffers (grid, vctparticles);
	     
      mem_avail = part[i].mover_PC(grid,vctparticles,EMf); // use the Predictor Corrector scheme
      if (mem_avail < 0)
      { // not enough memory space allocated for particles: stop the simulation		
	cout << "*************************************************************" << endl;
	cout <<"R" << myrank <<"L" << level << "Simulation stopped. Not enough memory allocated for particles" << endl;
	cout << "*************************************************************" << endl;
	mpi->Abort();
	return (-1);	
	//cycle = col->getNcycles()+1; // exit from the time loop
      }// end bracket memavail<0

      //cout << "Collection Method: " << part[i].getPRACollectionMethod()<< endl;
      if (part[i].getPRACollectionMethod()==1)
      {
	int resCollMethod;
	resCollMethod= part[i].CollectivePRARepopulationAdd(vctparticles, grid);
	if (resCollMethod<0)
	{
	  cout << "CollectivePRARepopulationAdd failed, check buffers\n";
	  mpi->Abort();
	  return -1;
	}
      }

      /* // debug ops
      if(0 && i==0)
      {
	part[i].printPRAparticles(vct, grid);
      }
      //very expensive debug op
      if (0 && level>0) 
      { 
	if (myrank == vct->getXLEN()* vct->getYLEN()) cout << "\n\nPRA particle stats AFTER MOVER\n";
	if (part[i].CountPRAParticles(vct)<0)
	  {mpi->Abort(); return -1;}
	  }*/
      
    }//end bracket on species
     //} // end to remove	  
    //MPI_Barrier(vct->getCART_COMM_TOTAL()); //maybe to be kept at Level level?
    	  
    int mem_avail1=2, mem_avail2=2;
    // communicate PRA particles for all species
    if(PRASendRcvOps)
    {
      for (int i=0; i < ns; i++) 
      {
	if (level < ngrids - 1)
	{
	  mem_avail1=part[i].PRASend(grid, vctparticles);
	}
	
	if (level>0)
	{
	  mem_avail2= part[i].PRAReceive(grid,vctparticles,EMf);
	}
	
	if (mem_avail1 < 0 || mem_avail2<0)
	{ // just one of the 2 is tested, according to the level	 
	  fflush(stdout);
	  cout << "**************************************************************************************" << endl;
	  cout <<"R" << myrank <<"L" << level << endl;
	  cout << "Simulation stopped. Not enough memory allocated for particles after PRA send or receive" << endl;
	  cout << "***************************************************************************************" << endl;
	  //cycle = col->getNcycles()+1; 
	  mpi->Abort();
	  return (-1);
	}
	/* debug, remove
	if (level>0) 
	{
	  if (myrank == vct->getXLEN()* vct->getYLEN()) 
	    {cout << "\n\nPRA particle stats AFTER REPOPULATION\n";}
	  if (part[i].CountPRAParticles(vct)<0)
	    {mpi->Abort(); return -1;}
	    }	  */
      }// end species
    }// end of PRASendRcvOps

    /*// to remove
    MPI_Barrier(vct->getCART_COMM());// barrier at Level level
    if (myrank==0 || myrank == 100)
      {
	cout << "L " <<level << " Barrier at level levelafter particle mover/send/receive, cycle " << cycle+1 << endl;
	}*/


    
    /*// to remove
    MPI_Barrier(vct->getCART_COMM_TOTAL()); 
    if (myrank==0)
    {
      cout << "L " <<level << " Barrier TOTAL after particle mover/send/receive, cycle " << cycle+1 << endl;
      }*/

    // Output save a file for the RESTART
    if (cycle%(col->getRestartOutputCycle())==0 && cycle != first_cycle)
      writeRESTART(RestartDirName,myrank,cycle,ns,mpi,vct,col,grid,EMf,part,0); // without ,0 add to restart file	   


    if (TEST)// and !(cycle%20 ))
      {
	Eenergy= EMf->getEenergy(vct);
	Benergy= EMf->getBenergy(vct);
	TOTKenergy=0.0;
	TOTmomentum=0.0;
	for (int is=0; is<ns; is++)
	  {
	    Kenergy[is]=part[is].getKenergy(vctparticles);
	    TOTKenergy+=Kenergy[is];
	    momentum[is]=part[is].getP(vctparticles);
	    TOTmomentum+=momentum[is];
	    }

	if (vct->getCartesian_rank() == 0) {
	  ofstream my_file(cq.c_str(), fstream::app);
	  
	  my_file << cycle << "\t" << "\t" << (Eenergy + Benergy + TOTKenergy) << "\t" << TOTmomentum << "\t" << Eenergy << "\t" << Benergy << "\t" << TOTKenergy <<"\t";
	  for (int is=0; is<ns; is++)
	    {
	      my_file <<Kenergy[is] <<"\t" <<momentum[is] <<"\t" ;
	      }
	  my_file <<endl;
	  my_file.close();
	 
	}
	
      }// end TEST


  }  // end of the cycle
  if (mem_avail==0) // write the restart only if the simulation finished succesfully
    writeRESTART(RestartDirName,myrank,(col->getNcycles()+first_cycle)-1,ns,mpi,vct,col,grid,EMf,part,0);

    // time stamping
    my_clock->stopTiming();
  // end time stamping

   // close MPI
  mpi->finalize_mpi();

  delete[] Kenergy;
  delete[] momentum;
  
  return(0);

}

