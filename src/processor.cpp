#include "processor.h"
#include "linux_parser.h"

#include <string>
#include <vector>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    std::vector<std::string> SystemCPUJiffies = LinuxParser::CpuUtilization();
    float idle = std::stol(SystemCPUJiffies[LinuxParser::kIdle_]) + std::stol(SystemCPUJiffies[LinuxParser::kIOwait_]);
    float nonIdle = 
        std::stol(SystemCPUJiffies[LinuxParser::kUser_]) + 
        std::stol(SystemCPUJiffies[LinuxParser::kNice_]) + 
        std::stol(SystemCPUJiffies[LinuxParser::kSystem_]) + 
        std::stol(SystemCPUJiffies[LinuxParser::kIRQ_]) +
        std::stol(SystemCPUJiffies[LinuxParser::kSoftIRQ_]) + 
        std::stol(SystemCPUJiffies[LinuxParser::kSteal_]);
    float total = idle + nonIdle;
    return (total - idle) / total; 
}