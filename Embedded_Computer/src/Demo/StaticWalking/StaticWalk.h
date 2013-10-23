#include "../../Utilities/logger.h"
#include "../../Control/STM32F4.h"
#include "../../Control/MotorControl_2.h"
#include "Utilities/ThreadManager.h"

#include <string>
#include <vector>

class StaticWalk
{
public:

    StaticWalk(ThreadManager *threadManager, MotorControl &mc);
    ~StaticWalk();

    void init(const std::string filename, const bool isUsingAlgorithm, const bool isMotorActivated, const bool isStandAlone);

    void initPosition(const int msInitializationTime);
    void run(double uDt);

private:
    bool bIsUsingAlgorithm;
    bool bIsMotorActivated;
    bool bIsStandAlone;

    std::vector<std::vector<double>> vPosition;
    std::vector<std::vector<double>>::iterator itrPos;
    std::vector<std::vector<double>>::iterator itrEnd;
	
    MotorControl _motion;
    ThreadManager *_threadManager;
};