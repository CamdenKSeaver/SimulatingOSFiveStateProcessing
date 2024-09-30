#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

#include<vector>
#include<algorithm>  //for sort
using namespace std;

#include "process.h"

inline bool procComp(const Process& p1, const Process& p2)
{
    return p1.arrivalTime > p2.arrivalTime;
}

inline bool ioComp(const IOEvent& e1, const IOEvent& e2)
{
    return e1.time < e2.time;
}


class ProcessManagement
{
    public:
      ProcessManagement(list<Process>& procList) : m_procList(procList) {};

      void readProcessFile(const string& fname);

      void activateProcesses(const int& time);

      bool moreProcessesComing() {return m_pending.size() != 0;}

    private:
      vector<Process> m_pending;

      list<Process>& m_procList;
};

#endif