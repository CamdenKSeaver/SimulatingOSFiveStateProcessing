#include "processMgmt.h"

void ProcessManagement::readProcessFile(const string& fname)
{
    stringstream ss;
    ifstream in(fname.c_str());
    string line, strItem;
    Process proc;
    unsigned int ioIDctrl(0), procIDctrl(0);
    int ioTime, ioDur;

    m_pending.clear();

    if(!in.good())
    {
        cerr << "initProcessSetFromFile error     unable to open file \"" << fname << "\"" << endl;
        return;
    }

    m_pending.reserve(20);

    while(getline(in, line))
    {
        ss.clear();
        ss.str(line);

        proc.id = procIDctrl;
        ++procIDctrl;

        ss >> proc.arrivalTime;

        ss >> proc.reqProcessorTime;

        proc.ioEvents.clear();
        while(ss >> ioTime)
        {
            ss >> ioDur;

            proc.ioEvents.push_back(IOEvent(ioTime, ioDur, ioIDctrl));
            ++ioIDctrl;
        }
        proc.ioEvents.sort(ioComp);

        m_pending.push_back(proc);
    }

    sort(m_pending.begin(), m_pending.end(), procComp);
}

void ProcessManagement::activateProcesses(const int& time)
{
    if(m_pending.size() > 0)
    {
        while(m_pending.back().arrivalTime == time)
        {
            m_procList.push_back(m_pending.back());
            m_pending.pop_back();
        }
    }
}