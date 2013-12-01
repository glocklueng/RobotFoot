/**
******************************************************************************^M
* @file    LegMotion.cpp
* @author  Camille Hébert & Antoine Rioux
* @date    2013-11-19
* @brief   Class to control the leg movements
******************************************************************************^M
*/

#include "LegMotion.h"

#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <sstream>
#include <iterator>

#include "MotionControl.h"
#include "Trajectory.h"

#include "../Utilities/XmlParser.h"

#include <memory>

LegMotion::LegMotion(std::shared_ptr<ThreadManager> threadManager_ptr, std::shared_ptr<MotorControl> mc_ptr, XmlParser& config):
    m_threadManager(threadManager_ptr),
    m_motion(mc_ptr)
{
	float distanceThreshold = config.getIntValue(XmlPath::LegsMotors / XmlPath::DISTANCETHRESHOLD);
	float angleThreshold = config.getIntValue(XmlPath::LegsMotors / XmlPath::ANGLETHRESHOLD);
	int iterationMax = config.getIntValue(XmlPath::LegsMotors / XmlPath::ITERATIONMAX);
	m_motionControl = new MotionControl(distanceThreshold, angleThreshold, iterationMax);

	m_stepHeight = config.getIntValue(XmlPath::LegsMotors / XmlPath::StepHeight);

	//Compensation offsets
	float RightPelvisPitchCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelvisPitchCompensationOffset);
	float RightPelvisRollCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelvisRollCompensationOffset);
	float RightPelvisYawCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelvisYawCompensationOffset);
	float RightPelvisxCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelvisxCompensationOffset);
	float RightPelvisyCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelvisyCompensationOffset);
	float RightPelviszCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightPelviszCompensationOffset);
	float LeftPelvisPitchCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelvisPitchCompensationOffset);
	float LeftPelvisRollCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelvisRollCompensationOffset);
	float LeftPelvisYawCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelvisYawCompensationOffset);
	float LeftPelvisxCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelvisxCompensationOffset);
	float LeftPelvisyCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelvisyCompensationOffset);
	float LeftPelviszCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftPelviszCompensationOffset);
	float RightFootPitchCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootPitchCompensationOffset);
	float RightFootRollCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootRollCompensationOffset);
	float RightFootYawCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootYawCompensationOffset);
	float RightFootxCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootxCompensationOffset);
	float RightFootyCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootyCompensationOffset);
	float RightFootzCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::RightFootzCompensationOffset);
	float LeftFootPitchCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootPitchCompensationOffset);
	float LeftFootRollCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootRollCompensationOffset);
	float LeftFootYawCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootYawCompensationOffset);
	float LeftFootxCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootxCompensationOffset);
	float LeftFootyCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootyCompensationOffset);
	float LeftFootzCompensationOffset = config.getIntValue(XmlPath::LegsMotors / XmlPath::LeftFootzCompensationOffset);

	m_vRightFootPosOffset = Eigen::Vector3f(RightFootxCompensationOffset, RightFootyCompensationOffset, RightFootzCompensationOffset);
	m_vRightFootAngleOffset = Eigen::Vector3f(RightFootPitchCompensationOffset, RightFootRollCompensationOffset, RightFootYawCompensationOffset);
	m_vLeftFootPosOffset = Eigen::Vector3f(LeftFootxCompensationOffset, LeftFootyCompensationOffset, LeftFootzCompensationOffset);
	m_vLeftFootAngleOffset = Eigen::Vector3f(LeftFootPitchCompensationOffset, LeftFootRollCompensationOffset, LeftFootYawCompensationOffset);
	m_vRightPelvisPosOffset = Eigen::Vector3f(RightPelvisxCompensationOffset, RightPelvisyCompensationOffset, RightPelviszCompensationOffset);
	m_vRightPelvisAngleOffset = Eigen::Vector3f(RightPelvisPitchCompensationOffset, RightPelvisRollCompensationOffset, RightPelvisYawCompensationOffset);
	m_vLeftPelvisPosOffset = Eigen::Vector3f(LeftPelvisxCompensationOffset, LeftPelvisyCompensationOffset, LeftPelviszCompensationOffset);
	m_vLeftPelvisAngleOffset = Eigen::Vector3f(LeftPelvisPitchCompensationOffset, LeftPelvisRollCompensationOffset, LeftPelvisYawCompensationOffset);

	m_pelvisPermanentPitch = config.getIntValue(XmlPath::LegsMotors / XmlPath::PermanentPelvisPitch);
}

LegMotion::~LegMotion()
{
	delete m_motionControl;
}

void LegMotion::InitWalk(Eigen::Vector2f destination, Eigen::Vector2f startingFeetAngles, Eigen::Vector2f destinationFeetAngles,
		const bool isMotorActivated, const bool isStandAlone, const int msInitializationTime, float stepTime)
{
	m_bIsMotorActivated = isMotorActivated;
	m_bIsUsingAlgorithm = true;
	m_bIsStandAlone = isStandAlone;

    std::unique_ptr<Trajectory> traj( new Trajectory(m_vRightFootPosOffset, m_vRightFootAngleOffset,
			m_vLeftFootPosOffset, m_vLeftFootAngleOffset, m_vRightPelvisPosOffset, m_vRightPelvisAngleOffset,
			m_vLeftPelvisPosOffset, m_vLeftPelvisAngleOffset, m_pelvisPermanentPitch) );
	m_trajectoryMatrix = traj->GenerateWalk(Eigen::Vector2f(0, 0), destination,
			destinationFeetAngles, startingFeetAngles, Trajectory::ZMP, stepTime, m_stepHeight);

	m_vInitialPosition = m_motionControl->GetInitialQPosition();

	//Set the initial position
	InitPosition(msInitializationTime);
}

void LegMotion::InitKick(const bool isMotorActivated, const bool isStandAlone, const int msInitializationTime, float kickTime)
{
	m_bIsMotorActivated = isMotorActivated;
	m_bIsUsingAlgorithm = true;
	m_bIsStandAlone = isStandAlone;

    std::unique_ptr<Trajectory> traj( new Trajectory(m_vRightFootPosOffset, m_vRightFootAngleOffset,
			m_vLeftFootPosOffset, m_vLeftFootAngleOffset, m_vRightPelvisPosOffset, m_vRightPelvisAngleOffset,
			m_vLeftPelvisPosOffset, m_vLeftPelvisAngleOffset, m_pelvisPermanentPitch) );

	m_trajectoryMatrix = traj->GenerateKick(0.5f);

    m_vInitialPosition = m_motionControl->GetInitialQPosition();

	//Set the initial position
	InitPosition(msInitializationTime);
}

void LegMotion::Init(const std::string filename, const bool isMotorActivated, const bool isStandAlone, const int msInitializationTime)
{
	m_bIsMotorActivated = isMotorActivated;
	m_bIsUsingAlgorithm = false;
	m_bIsStandAlone = isStandAlone;

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

        m_vPosition.push_back(vPos);
        getline( file, strLine );

    }

    m_itrPos = m_vPosition.begin();
   // m_vInitialPosition = std::vector<float>(*m_vPosition.begin()); //Erreurs possible!!!
    m_vInitialPosition = (*m_vPosition.begin());
    m_itrEnd = m_vPosition.end();

	//Set the initial position
	InitPosition(msInitializationTime);
}

void LegMotion::InitPosition(const int msInitializationTime)
{
    // Enable Torque
    if( m_bIsMotorActivated && !m_motion->SetTorque(true, MotorControl::Config::ALL_LEGS ) )
    {
        Logger::getInstance() << "SetTorque Failed\n";
        return;
    }

    if(m_bIsMotorActivated && !m_vInitialPosition.empty())
    {
        if( !m_motion->InitPositions( m_vInitialPosition, MotorControl::Config::ALL_LEGS, msInitializationTime ) )
        {
           Logger::getInstance() << "InitPosition Failed\n";
           return;
        }
    }

}

void LegMotion::Run(double msDt)
{
    try
    {
		if(!m_bIsUsingAlgorithm)
		{
			//m_threadManager->wait();
			boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
			// Process mouvement with file as input
			for(;m_itrPos != m_itrEnd; ++m_itrPos)
			{
				if (!m_bIsStandAlone)
				{
					boost::this_thread::interruption_point();
					Logger::getInstance(Logger::LogLvl::DEBUG) << "LegMotion : wait for MotorControl" << std::endl;
					m_threadManager->wait();
				}

				boost::chrono::duration<double> sec = boost::chrono::system_clock::now() - start;
				Logger::getInstance(Logger::LogLvl::DEBUG) << "took " << sec.count() << " seconds" << std::endl;

				start = boost::chrono::system_clock::now();

				if (m_bIsMotorActivated)
				{
					if (m_bIsStandAlone)
					{
						m_motion->HardSet( *m_itrPos, MotorControl::Config::ALL_LEGS );
					}
					else
					{
						if(!m_motion->SetPositions( *m_itrPos, MotorControl::Config::ALL_LEGS ) )
						{
							Logger::getInstance() << "SetPosition Failed\n";
							break;
						}
					}
				}
				//for(std::vector<double>::iterator it = m_itrPos->begin(); it != m_itrPos->end(); ++it)
				//{
				//	Logger::getInstance() << *it << " ";
				//}
				//Logger::getInstance() << std::endl;

				if (m_bIsStandAlone)
				{
					usleep(msDt*1000);
				}
				else
				{
					m_threadManager->resume(ThreadManager::Task::MOTOR_CONTROL);
					Logger::getInstance(Logger::LogLvl::DEBUG) << "StaticWalk : Iteration done" << std::endl;
					m_threadManager->resume(ThreadManager::Task::MOTOR_CONTROL);
				}
			}
		}
		else
		{
			std::vector<double> motorsPosition = m_vInitialPosition;
			for(int i = 0; i < m_trajectoryMatrix.rows(); ++i)
			{
				//boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();i
				if (!m_bIsStandAlone)
				{
					boost::this_thread::interruption_point();
					Logger::getInstance(Logger::LogLvl::DEBUG) << "StaticWalk : wait for MotorControl" << std::endl;
					m_threadManager->wait();
                }

				// Right Leg movement
				if(false && m_bIsMotorActivated)
				{
					motorsPosition.clear();
					m_motion->HardGet( motorsPosition, MotorControl::Config::ALL_LEGS );
				}

				//Update Q
				ReorderQ(motorsPosition);
				motorsPosition = m_motionControl->UpdateQ(m_trajectoryMatrix.row(i), motorsPosition);
				//m_motionControl->Move(m_trajectoryMatrix);

				//set motors
				if(m_bIsMotorActivated)
				{
					if (m_bIsStandAlone)
                    {
                        m_motion->HardSet( motorsPosition, MotorControl::Config::ALL_LEGS );
                    }
					else
					{
						if(!m_motion->SetPositions( motorsPosition, MotorControl::Config::ALL_LEGS ) )
						{
							Logger::getInstance() << "SetPosition Failed\n";
							break;
						}
					}
				}

				if (m_bIsStandAlone)
				{
					usleep(msDt*1000);
				}
				else
				{
					m_threadManager->resume(ThreadManager::Task::MOTOR_CONTROL);
					Logger::getInstance(Logger::LogLvl::DEBUG) << "StaticWalk : Iteration done" << std::endl;
					m_threadManager->resume(ThreadManager::Task::MOTOR_CONTROL);
				}
			}
		}
        while(1)
        {
        
                std::vector<double> dummy;
                m_motion->HardGet(dummy, MotorControl::Config::ALL_LEGS);
        }
    }
    catch(boost::thread_interrupted const &e)
    {
        Logger::getInstance() << "LEGS_CONTROL task Interrupted. " << std::endl;
    }
}


void LegMotion::ReorderQ(std::vector<double>& qVector)
{
	std::vector<double> tempQVector = qVector;

	for(int i = 0; i < 5; ++i)
	{
		tempQVector[i] = qVector[5-i];
	}
	qVector = tempQVector;
}

