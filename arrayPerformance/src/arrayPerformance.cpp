// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

/* arrayPerformance.cpp */

/**
 * @author mrk
 * @date 2013.08.08
 */

#include <pv/lock.h>

#define epicsExportSharedSymbols
#include <pv/arrayPerformance.h>



namespace epics { namespace exampleCPP { namespace arrayPerformance { 

using namespace std;
using namespace epics::pvData;
using namespace epics::pvDatabase;

ArrayPerformancePtr ArrayPerformance::create(
    std::string const & recordName,
        size_t size,
        double delay)
{
    epics::pvData::PVStructurePtr pvStructure =
       epics::pvData::getStandardPVField()->scalarArray(epics::pvData::pvLong,"timeStamp,alarm");
    ArrayPerformancePtr pvRecord(
        new ArrayPerformance(recordName,pvStructure,size,delay));
    if(!pvRecord->init()) pvRecord.reset();
    return pvRecord;
}

ArrayPerformance::ArrayPerformance(
    std::string const & recordName,
    epics::pvData::PVStructurePtr const & pvStructure,
    size_t size,
    double delay)
: PVRecord(recordName,pvStructure),
  size(size),
  delay(delay),
  isDestroyed(false),
  threadName("arrayPerformance")
{    
    pvTimeStamp.attach(pvStructure->getSubField("timeStamp"));
}



bool ArrayPerformance::init()
{
    initPVRecord();
    pvValue = getPVStructure()->getSubField<PVLongArray>("value");
    thread = std::auto_ptr<epicsThread>(new epicsThread(
        *this,
        threadName.c_str(),
        epicsThreadGetStackSize(epicsThreadStackSmall),
        epicsThreadPriorityLow));
     thread->start();
    return true;
}

void ArrayPerformance::process()
{
    timeStamp.getCurrent();
    pvTimeStamp.set(timeStamp);
}

void ArrayPerformance::destroy()
{
    if(isDestroyed) return;
    isDestroyed = true;
    PVRecord::destroy();
}

void ArrayPerformance::stop()
{
    runStop.signal();
    runReturn.wait();
}


void ArrayPerformance::run()
{
    TimeStamp timeStamp;
    TimeStamp timeStampLast;
    timeStampLast.getCurrent();
    int nSinceLastReport = 0;
    int64 value = 0;
    while(true) {
        if(runStop.tryWait()) {
             runReturn.signal();
             return;
        }    
        if(delay>0.0) epicsThreadSleep(delay);
        //event.wait();
        timeStamp.getCurrent();
        double diff = TimeStamp::diff(timeStamp,timeStampLast);
        if(diff>=1.0) {
            ostringstream out;
            out << "arrayPerformance value " << value;
            out << " time " << diff ;
            double iterations = nSinceLastReport;
            iterations /= diff;
            if(iterations>10.0e9) {
                 iterations /= 1e9;
                 out << " gigaIterations/sec " << iterations;
            } else if(iterations>10.0e6) {
                 iterations /= 1e6;
                 out << " megaIterations/sec " << iterations;
            } else if(iterations>10.0e3) {
                 iterations /= 1e3;
                 out << " kiloIterations/sec " << iterations;
            } else  {
                 out << " Iterations/sec " << iterations;
            }
            double elementSize = size;
            double elementsPerSecond = elementSize*nSinceLastReport;
            elementsPerSecond /= diff;
            if(elementsPerSecond>10.0e9) {
                 elementsPerSecond /= 1e9;
                 out << " gigaElements/sec " << elementsPerSecond;
            } else if(elementsPerSecond>10.0e6) {
                 elementsPerSecond /= 1e6;
                 out << " megaElements/sec " << elementsPerSecond;
            } else if(elementsPerSecond>10.0e3) {
                 elementsPerSecond /= 1e3;
                 out << " kiloElements/sec " << elementsPerSecond;
            } else  {
                 out << " Elements/sec " << elementsPerSecond;
            }
            cout << out.str() << endl;
            timeStampLast = timeStamp;
            nSinceLastReport = 0;
        }
        ++nSinceLastReport;
        lock();
        try {
            if(getTraceLevel()>1) {
                 cout << "arrayPerformance size " << size;
                 cout << " value " << value +1 << endl;
            }
            shared_vector<int64> xxx(size,value++);
            shared_vector<const int64> data(freeze(xxx));
                beginGroupPut();
                pvValue->replace(data);
                process();
                endGroupPut();
        } catch(...) {
           unlock();
           throw;
        }
        unlock();
    }
}

}}}

