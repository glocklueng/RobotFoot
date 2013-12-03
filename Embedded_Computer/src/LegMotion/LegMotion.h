/**
******************************************************************************^M
* @file    LegMotion.h
* @author  Camille Hébert & Antoine Rioux
* @date    2013-11-19
* @brief   Class to control the leg movements
******************************************************************************^M
*/

#ifndef LEGMOTION_H
#define LEGMOTION_H


#include "../Utilities/logger.h"
#include "../Control/STM32F4.h"
#include "../Control/MotorControl_2.h"
#include "Utilities/ThreadManager.h"

#include <string>
#include <vector>
#include <memory> // shared_ptr

#include "../../ThirdParty/Eigen/Dense"
#include "Trajectory.h"

class MotionControl;

class LegMotion
{
public:

	enum MotionType {
		Walk,
		Kick
	};

	LegMotion(std::shared_ptr<ThreadManager> threadManager_ptr, std::shared_ptr<MotorControl> mc_ptr, XmlParser& config);
    ~LegMotion();

    // Initialize configuration of mouvement
    void InitWalk(Eigen::Vector2f destination, Eigen::Vector2f startingFeetAngles, Eigen::Vector2f destinationFeetAngles,
    		const bool isMotorActivated, const bool isStandAlone);
    void InitKick(const bool isMotorActivated, const bool isStandAlone, float kickTime);
    void Init(const std::string filename, const bool isMotorActivated, const bool isStandAlone);

    // Move robot to initial position
    void InitPosition(const int msInitializationTime);

    // Start the mouvement
    void Run(double uDt);

private:

    void ReorderQ(std::vector<double>& qVector);

    // Use Algorithm if true and text file input if false to generate mouvement
    bool m_bIsUsingAlgorithm;
    // Activated motor if true
    bool m_bIsMotorActivated;
    // Activated stand alone mode without multithreading
    bool m_bIsStandAlone;
    // Use COM instead of ZMP if true
    Trajectory::PelvisTrajectoryType m_pelvisTrajectoryType;

    float m_stepHeight;
    float m_stepLength;
    float m_stepTime;
    float m_pelvisPermanentPitch;

	Eigen::Vector3f m_vRightFootPosOffset;
	Eigen::Vector3f m_vRightFootAngleOffset;
	Eigen::Vector3f m_vLeftFootPosOffset;
	Eigen::Vector3f m_vLeftFootAngleOffset;
	Eigen::Vector3f m_vRightPelvisPosOffset;
	Eigen::Vector3f m_vRightPelvisAngleOffset;
	Eigen::Vector3f m_vLeftPelvisPosOffset;
	Eigen::Vector3f m_vLeftPelvisAngleOffset;

    // Contains motor position from the text file
    std::vector<std::vector<double>> m_vPosition;
    std::vector<std::vector<double>>::iterator m_itrPos;
    std::vector<std::vector<double>>::iterator m_itrEnd;
    std::vector<double> m_vInitialPosition;

    std::shared_ptr<ThreadManager> m_threadManager;
    std::shared_ptr<MotorControl> m_motion;

    Eigen::MatrixXf m_trajectoryMatrix;

    std::shared_ptr<MotionControl> m_motionControl;
};

#endif  //LEGMOTION_H
