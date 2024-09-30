#include "process.h"
#include "ioModule.h"
#include "processMgmt.h"

#include <chrono> // for sleep
#include <thread> // for sleep

int main(int argc, char* argv[])
{
    // single thread processor
    // it's either processing something or it's not


    // vector of processes, processes will appear here when they are created by
    // the ProcessMgmt object (in other words, automatically at the appropriate time)
    list<Process> processList;
    
    // this will orchestrate process creation in our system, it will add processes to 
    // processList when they are created and ready to be run/managed
    ProcessManagement processMgmt(processList);

    // this is where interrupts will appear when the ioModule detects that an IO operation is complete
    list<IOInterrupt> interrupts;   

    // this manages io operations and will raise interrupts to signal io completion
    IOModule ioModule(interrupts);  

    // Do not touch
    long time = 1;
    long sleepDuration = 50;
    string file;
    stringstream ss;
    enum stepActionEnum {noAct, admitNewProc, handleInterrupt, beginRun, continueRun, ioRequest, complete} stepAction;

    // Do not touch
    switch(argc)
    {
        case 1:
            file = "./procList.txt";  // default input file
            break;
        case 2:
            file = argv[1];         // file given from command line
            break;
        case 3:
            file = argv[1];         // file given
            ss.str(argv[2]);        // sleep duration given
            ss >> sleepDuration;
            break;
        default:
            cerr << "incorrect number of command line arguments" << endl;
            cout << "usage: " << argv[0] << " [file] [sleepDuration]" << endl;
            return 1;
            break;
    }

    processMgmt.readProcessFile(file);


    time = 0;
    bool processorAvailable = true;
    //pointer to process that is running
    Process* currentP = nullptr;
    //list of processes that are all ready
    list<Process*> readyList; 
    //tracks if a new process was made and if all the processes are done
    bool newP = false;
    bool finallyDone = true;
    //keep running the loop until all processes have been added and have run to completion
    while(processMgmt.moreProcessesComing()  || finallyDone == false )
    {
        //Update our current time step
        ++time;

        //let new processes in if there are any
        processMgmt.activateProcesses(time);

        //update the status for any active IO requests
        ioModule.ioProcessing(time);

        //If the processor is tied up running a process, then continue running it until it is done or blocks
        //   note: be sure to check for things that should happen as the process continues to run (io, completion...)
        //If the processor is free then you can choose the appropriate action to take, the choices (in order of precedence) are:
        // - admit a new process if one is ready (i.e., take a 'newArrival' process and put them in the 'ready' state)
        // - address an interrupt if there are any pending (i.e., update the state of a blocked process whose IO operation is complete)
        // - start processing a ready process if there are any ready


        //init the stepAction, update below
        stepAction = noAct;

        
        //TODO add in the code to take an appropriate action for this time step!
        //you should set the action variable based on what you do this time step. you can just copy and paste the lines below and uncomment them, if you want.
        //stepAction = continueRun;  //runnning process is still running
        //stepAction = ioRequest;  //running process issued an io request
        //stepAction = complete;   //running process is finished
        //stepAction = admitNewProc;   //admit a new process into 'ready'
        //stepAction = handleInterrupt;   //handle an interrupt
        //stepAction = beginRun;   //start running a process
 
        

        //   <your code here> 
        //if a process is running need to decide what to do with it
        if (processorAvailable== false) {
                currentP->processorTime++;
                //if the process is at the time it has needed then complete the process
                //processor is now free to run another
                if (currentP->processorTime >= currentP->reqProcessorTime) {
                    stepAction = complete;
                    currentP->state = done;
                    currentP = nullptr;
                    processorAvailable = true;
                //if there is an IOevent for the process and the IO event is supposed to happen at the current time
                //then do the IO event and remove it and set it into a blocked state while the IOevent hasnt finished
                //processor is also now free since it was blocked
                } else if (currentP->ioEvents.empty() ==false && currentP->processorTime == currentP->ioEvents.front().time) {
                    stepAction = ioRequest;
                    currentP->state = blocked;
                    IOEvent ioEvent = currentP->ioEvents.front();
                    ioModule.submitIORequest(time, ioEvent, *currentP);
                    currentP->ioEvents.pop_front();
                    processorAvailable = true;
                } else {
                    stepAction = continueRun; //if nothing needs to be done then just continue
                }
            }
            //if the processor is available to run a new process check interrupts first
            else {
                //save the interrupt object first before removing it
                //then check to see what process matches up with the interrupt
                //set that process to ready and put it readylist now that the interrupt has been handled
                if (interrupts.empty() == false) {
                    stepAction = handleInterrupt;
                    IOInterrupt interrupt = interrupts.front();
                    interrupts.pop_front();
                    for (auto& p : processList) {
                        if (p.id == interrupt.procID) {
                            p.state = ready;
                            readyList.push_back(&p); //dont want to make a copy so use a pointer
                            break;
                        }
                    }
                } else{
                    //bool just to hold that if a new process has been added then dont do anything else after
                    newP = false;
                    //there are no interrupts and nothing running so we can check to see if there are any new processes to set to ready
                    //add the new ready process to the readylist
                    for (auto& p : processList) {
                        if (p.state == newArrival) {
                            p.state = ready;
                            readyList.push_back(&p); 
                            stepAction = admitNewProc;
                            newP = true;
                            break;
                        }
                    }
                    //runs a new process from the ready list and removes it from the readylist
                    if (newP == false && readyList.empty() == false) {
                       currentP = readyList.front(); 
                       readyList.pop_front();
                       currentP->state = processing;
                       processorAvailable = false;
                       stepAction = beginRun;
                   }
               }


                //couldnt really figure out a simpler way to check if all the processes are done to end the while loop
                //checks through all the processes to see if the states all = done
               for (const auto& p : processList) {
                   if (p.state != done) {
                        finallyDone = false;
                        break;
                    }
                    finallyDone = true; //very exciting part 
                }


            }


        // Leave the below alone (at least for final submission, we are counting on the output being in expected format)
        cout << setw(5) << time << "\t"; 
        
        switch(stepAction)
        {
            case admitNewProc:
              cout << "[  admit]\t";
              break;
            case handleInterrupt:
              cout << "[ inrtpt]\t";
              break;
            case beginRun:
              cout << "[  begin]\t";
              break;
            case continueRun:
              cout << "[contRun]\t";
              break;
            case ioRequest:
              cout << "[  ioReq]\t";
              break;
            case complete:
              cout << "[ finish]\t";
              break;
            case noAct:
              cout << "[*noAct*]\t";
              break;
        }

        // You may wish to use a second vector of processes (you don't need to, but you can)
        printProcessStates(processList); // change processList to another vector of processes if desired

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    return 0;
}
