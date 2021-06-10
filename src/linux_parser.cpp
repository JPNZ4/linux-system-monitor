#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal, memAvailable, value;
  std::string key;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = value;
        } else if (key == "MemAvailable:") {
          memAvailable = value;
        }
      }
    }
  }
  return (memTotal - memAvailable) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = 0;
  std::string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  std::string line, value;
  std::vector<std::string> stats;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::stringstream linestream(line);
    while (linestream >> value) {
      stats.push_back(value);
    }
  }
  long totalTime = std::stol(stats[13]) + std::stol(stats[14]) + std::stol(stats[15]) + std::stol(stats[16]);
  return totalTime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> SystemCPUJiffies = LinuxParser::CpuUtilization();
  long idle = std::stol(SystemCPUJiffies[LinuxParser::kIdle_]) + std::stol(SystemCPUJiffies[LinuxParser::kIOwait_]);
  return idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::string value, line;
  vector<std::string> systemCPUJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::stringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        systemCPUJiffies.push_back(value);
      }
    }
  }    
  return systemCPUJiffies; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::string line;
  std::string key;
  int total;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> total) {
        if (key == "processes") {
          return total;
        }
      }
    }
  } 
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line;
  std::string key;
  int runningProcs;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> runningProcs) {
        if (key == "procs_running") {
          return runningProcs;
        }
      }
    }
  } 
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    return line;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::stringstream linestream(line);
      while (linestream >> key >> value) {      
        if (key == "VmSize:") {
          long megaBytes = std::stol(value) / 1000;        
          return to_string(megaBytes);
        }
      }
    }    
  }
  return string("0");
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::string uid = LinuxParser::Uid(pid);
  std::string line, username, x, value;
  std::ifstream filestreamPasswd(kPasswordPath);
  if (filestreamPasswd.is_open()) {
    while (getline(filestreamPasswd, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      while (linestream >> username >> x >> value) {
        if (value == uid) {
          return username;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::string line, value;
  std::vector<std::string> stats;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::stringstream linestream(line);
    while (linestream >> value) {
      stats.push_back(value);
    }
  }
  return LinuxParser::UpTime() - (std::stol(stats[21]) / sysconf(_SC_CLK_TCK));
}
