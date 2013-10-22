#include "StaticWalk.h" 

#include "WalkStatus.h"

#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <sstream>
#include <iterator>

StaticWalk::StaticWalk(MotorControl *mc):
    motion(mc)
{

}

StaticWalk::~StaticWalk()
{

}

void StaticWalk::init(ThreadManager *threadManager, const std::string filename, const bool isUsingAlgorithm, const bool isMotorActivated):
_threadManager(threadManager)
{
    bIsMotorActivated = isMotorActivated;
    bIsUsingAlgorithm = isUsingAlgorithm;
   
    std::string strLine;
    std::ifstream file;
    file.open( filename.c_str() );
    std::vector<double> vPos;  
    getline( file, strLine );
    
    while( !file.eof() )
    {
        vPos.clear();
        std::istringstream iss(strLine);

        std::copy(std::istream_iterator<double>(iss),
            std::istream_iterator<double>(),
            std::back_inserter(vPos));

        vPosition.push_back(vPos);
        getline( file, strLine );

    }

    itrPos = vPosition.begin();
    itrEnd = vPosition.end(); 
    /*for(std::vector<std::vector<double>>::iterator itr = vPosition.begin(); itr != vPosition.end(); ++itr)
    {
        for(std::vector<double>::iterator it = itr->begin(); it != itr->end(); ++it)
    	{
    		Logger::getInstance() << *it << " ";
        }
        Logger::getInstance() << std::endl;
    }*/
}

void StaticWalk::initPosition(const int msInitializationTime)
{
    // Enable Torque
    if( bIsMotorActivated && !motion->SetTorque( MotorControl::ALL_LEGS ) )
    {
        Logger::getInstance() << "SetTorque Failed\n";
        return;
    }
   
    if(bIsMotorActivated)
    {
        if( itrPos != itrEnd && !motion->InitPositions( *itrPos, MotorControl::ALL_LEGS, msInitializationTime ) )
        {
           Logger::getInstance() << "InitPosition Failed\n";
           return;
        }
    }

}

void StaticWalk::run(const double uDt)
{
    try
    {
        if(!bIsUsingAlgorithm)
        {
            // Process mouvement with file as input
            for(;itrPos != itrEnd; ++itrPos)
            {
                boost::this_thread::interruption_point();
                _threadManager->wait();

                if(bIsMotorActivated)
                {
                    if(!motion->SetPositions( *itrPos, MotorControl::ALL_LEGS ) )
                    {
                        Logger::getInstance() << "SetPosition Failed\n";
                        break;
                    }
                }
                for(std::vector<double>::iterator it = itrPos->begin(); it != itrPos->end(); ++it)
                {
                    Logger::getInstance() << *it << " ";
                }
                Logger::getInstance() << std::endl;

                //usleep(uDt*1000*1000);
                _threadManager.resume(ThreadManager::Task::MOTOR_CONTROL);
            } 
        }
        else
        { 
            //Process mouvement with algorithm

            //TODO remove HardCoded
            const double tf = 3.1;
            double dt = uDt;
            WalkStatus calculR(2);
            //const int numberOfExecution = 1;

            //for( int i = 0; i < numberOfExecution; i++ )
            //{  
            //if( i % 2 == 0 )
            //{
            calculR.initAllTrajParam(0.03, 0.02);
            //calculL.initAllTrajParam(-0.03, 0.00);
            //}
            //else
            //{
            //calculR.initAllTrajParam(-0.03, 0.00);
            //calculL.initAllTrajParam(0.03, 0.02);
            //}

            std::vector<double> vPos;
            for( double time = 0.0; time <= tf + dt; time += dt )
            {
                //boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();i
                boost::this_thread::interruption_point();
                _threadManager->wait();

                // Right Leg movement
                if(bIsMotorActivated)
                {
                    vPos.clear();
                    motion->ReadPositions( vPos, MotorControl::RIGHT_LEGS );
                }

                calculR.Process( time, vPos );
                calculR.getMotorPosition( vPos );

                if(bIsMotorActivated)
                {
                    if( !motion->SetPositions( vPos, MotorControl::RIGHT_LEGS ) )
                    {
                        break;
                    }
                }
                for(std::vector<double>::iterator it = vPos.begin(); it != vPos.end(); ++it)
                {
                    Logger::getInstance() << *it << " ";
                }
                Logger::getInstance() << std::endl;

                //boost::chrono::duration<double> sec = boost::chrono::system_clock::now() - start;
                std::cout << "took " << sec.count() << " seconds\n";
                ////TODO Left leg movement should be calculated in the calcul algorithm.
//            // Left Leg movement
//            vPos.clear();
//            motion->ReadPositions( vPos, MotorControl::LEFT_LEGS );
//            // Reverse motor angles
//            std::vector<double>::iterator itr = vPos.begin();
//            const std::vector<double>::iterator end = vPos.end();
//            for( ; itr != end; itr++ )
//            {
//             *itr = -*itr;
//             }
//             calculL.Process( time, vPos );
//             calculL.getMotorPosition( vPos );
//            // Reverse motor angles
//            itr = vPos.begin();
//            for( ; itr != end; itr++ )
//            {
//             *itr = -*itr;
//             }
//             if( !motion->SetPositions( vPos, MotorControl::LEFT_LEGS ) )
//             {
//             break;
//             }

                //usleep(dt*1000*1000);
                _threadManager.resume(ThreadManager::Task::MOTOR_CONTROL);
            }   
        //}
        }
    }
    catch(boost::thread_interrupted const &e)
    {
        Logger::getInstance() << "LEGS_CONTROL task Interrupted. " << e.what() << std::endl;
    } 
}

