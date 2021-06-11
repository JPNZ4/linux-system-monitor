#include "processor.h"
#include "linux_parser.h"

#include <string>
#include <vector>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long idleJiffies = LinuxParser::IdleJiffies();
    long activeJiffies = LinuxParser::ActiveJiffies();
    long totalJiffies = LinuxParser::Jiffies();

    if (previousTotal_ && previousIdle_) {
        float currentTotal = totalJiffies - previousTotal_;
        float currentIdle = idleJiffies - previousIdle_;
        previousTotal_ = totalJiffies;
        previousIdle_ = idleJiffies;
        return (currentTotal - currentIdle) / currentTotal;
    }
    previousTotal_ = totalJiffies;
    previousIdle_ = idleJiffies;
    return activeJiffies / totalJiffies; 
}