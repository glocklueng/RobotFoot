#include "MotorControl.h"
#include "WalkStatus.h"

#include "LinuxDARwIn.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <sstream>
#include <iterator>

using namespace Robot;

const std::string fileName = "log.txt";
const double uDt = 0.016; // Time step in second

int run()
{
   std::cout << "Starting program. Sleeping 4 sec\n";
   sleep(4);
   
   // Framework initialization
   LinuxCM730 linux_cm730("/dev/ttyUSB0");
   CM730 cm730(&linux_cm730);
   if(cm730.Connect() == false)
   {
      std::cout << "Fail to connect CM-730!\n";
      return 0;
   }
   
   /*
   std::string strLine;
   std::ifstream file;
   
   file.open( fileName.c_str() );
   */
   
   WalkStatus calculR(2), calculL(2);   
   MotorControl motion = MotorControl( &cm730 );
   
   // Enable Torque
   if( !motion.SetTorque( MotorControl::ALL_LEGS ) )
   {
      std::cout << "SetTorque Failed\n";
      //file.close(); 
      return 0;
   }
   
   // Set inital position
   if( 1/*!file.eof()*/ )
   {
      std::vector<double> vPos;
      /*
      getline( file, strLine );
      std::istringstream iss(strLine);

      std::copy(std::istream_iterator<double>(iss),
	        std::istream_iterator<double>(),
	        std::back_inserter(vPos));      
      */
      calculR.getMotorPosition( vPos );
      std::vector<double>::iterator itr = vPos.begin();
      const std::vector<double>::iterator end = vPos.end();
      for( ; itr != end; itr++ )
      {
	vPos.push_back( -*itr );
      }
      
      if( !motion.InitPosition( vPos, MotorControl::ALL_LEGS, 3000 ) )
      {
	 //file.close(); 
	 return 0;
      }
   }   
   
   std::cout << "Beginning mouvement. Sleeping 1 sec\n";
   sleep(1);
   /*
   // Process mouvement
   while( !file.eof() )
   {
      std::vector<double> vPos;
      
      getline( file, strLine );
      std::istringstream iss(strLine);

      std::copy(std::istream_iterator<double>(iss),
	        std::istream_iterator<double>(),
	        std::back_inserter(vPos));
      
      if( file.eof() || !motion.SetPosition( vPos, MotorControl::ALL_LEGS ) )
      {
	 break;
      }
      usleep(uDt**1000*1000);
   } 
   */
   
   const double tf = 3.1;
   double dt = uDt;
   
   for( int i = 0; i < 5; i++ )
   {  
      if( i % 2 == 0 )
      {
	calculR.initAllTrajParam(0.03, 0.02);
	calculL.initAllTrajParam(-0.03, 0.00);
      }
      else
      {
	calculR.initAllTrajParam(-0.03, 0.00);
	calculL.initAllTrajParam(0.03, 0.02);
      }
      
      for( double time = 0.0; time <= tf + dt; time += dt )
      {
	std::vector<double> vPos;
	
	// Right Leg movement
	motion.ReadPosition( vPos, MotorControl::RIGHT_LEGS );
	calculR.Process( time, vPos );
	calculR.getMotorPosition( vPos );
	if( !motion.SetPosition( vPos, MotorControl::RIGHT_LEGS ) )
	{
	    break;
	}
	
	// Left Leg movement
	vPos.clear();
	motion.ReadPosition( vPos, MotorControl::LEFT_LEGS );
	// Reverse motor angles
	std::vector<double>::iterator itr = vPos.begin();
	const std::vector<double>::iterator end = vPos.end();
	for( ; itr != end; itr++ )
	{
	    *itr = -*itr;
	}
	calculL.Process( time, vPos );
	calculL.getMotorPosition( vPos );
	// Reverse motor angles
	itr = vPos.begin();
	for( ; itr != end; itr++ )
	{
	    *itr = -*itr;
	}
	if( !motion.SetPosition( vPos, MotorControl::LEFT_LEGS ) )
	{
	    break;
	}
	
	usleep(dt*1000*1000);
      }   
    
   }
   
   std::cout << "Finishing program. Sleeping 4 sec\n";
   sleep(4);
   
   //file.close();
   std::cout << "Program finished\n";
   
   return 0;
}

int main()
{   
   std::cout << "Use text file : 1" << std::endl;
   std::cout << "Use script : 2" << std::endl;
   return run();	
}


