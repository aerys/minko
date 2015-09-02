/************************************************************************************

Filename    :   OVR_Lockless.cpp
Content     :   Test logic for lock-less classes
Created     :   December 27, 2013
Authors     :   Michael Antonov

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#include "OVR_Lockless.h"

#ifdef OVR_LOCKLESS_TEST

#include "OVR_Threads.h"
#include "OVR_Timer.h"
#include "OVR_Log.h"

namespace OVR { namespace LocklessTest {


const int TestIterations = 10000000;

// Use volatile dummies to force compiler to do spinning.
volatile int Dummy1;
int          Unused1[32];
volatile int Dummy2;
int          Unused2[32];
volatile int Dummy3;
int          Unused3[32];


// Data block out of 20 consecutive integers, should be internally consistent.
struct TestData
{
    enum { ItemCount = 20 };

    int Data[ItemCount];


    void Set(int val)
    {
        for (int i=0; i<ItemCount; i++)
        {
            Data[i] = val*100 + i;
        }
    }

    int ReadAndCheckConsistency(int prevValue) const
    {
        int val = Data[0];

        for (int i=1; i<ItemCount; i++)
        {
            
            if (Data[i] != (val + i))
            {
                // Only complain once per same-value entry
                if (prevValue != val / 100) 
                {
                    LogText("LocklessTest Fail - corruption at %d inside block %d\n",
                            i, val/100);
                    // OVR_ASSERT(Data[i] == val + i);
                }
                break;
            }
        }

        return val / 100;
    }
};



volatile bool              FirstItemWritten = false;
LocklessUpdater<TestData, TestData>  TestDataUpdater;

// Use this lock to verify that testing algorithm is otherwise correct...
Lock                       TestLock;   


//-------------------------------------------------------------------------------------

// Consumer thread reads values from TestDataUpdater and
// ensures that each one is internally consistent.

class Consumer : public Thread
{
    virtual int Run()
    {
        LogText("LocklessTest::Consumer::Run started.\n");
        
        while (!FirstItemWritten)
        {
            // spin until producer wrote first value...
        }

        TestData d;
        int      oldValue = 0;
        int      newValue;

        do 
        {
            {
                //Lock::Locker scope(&TestLock);
                d = TestDataUpdater.GetState();
            }
            
            newValue = d.ReadAndCheckConsistency(oldValue);
            
            // Values should increase or stay the same!
            if (newValue < oldValue)
            {
                LogText("LocklessTest Fail - %d after %d;  delta = %d\n",
                        newValue, oldValue, newValue - oldValue);
         //       OVR_ASSERT(0);
            }
            

            if (oldValue != newValue)
            {
                oldValue = newValue;

                if (oldValue % (TestIterations/30) == 0)
                {
                    LogText("LocklessTest::Consumer - %5.2f%% done\n",
                            100.0f * (float)oldValue/(float)TestIterations);
                }
            }            

            // Spin a while
            for (int j = 0; j< 300; j++)
            {
                Dummy3 = j;
            }


        } while (oldValue < (TestIterations * 99 / 100));

        LogText("LocklessTest::Consumer::Run exiting.\n");
        return 0;
    }

};


//-------------------------------------------------------------------------------------

class Producer : public Thread
{

    virtual int Run()
    {
        LogText("LocklessTest::Producer::Run started.\n");        

        for (int testVal = 0; testVal < TestIterations; testVal++)
        {
            TestData d;
            d.Set(testVal);

            {
                //Lock::Locker scope(&TestLock);
                TestDataUpdater.SetState(d);
            }

            FirstItemWritten = true;

            // Spin a bit
            for(int j = 0; j < 1000; j++)
            {
                Dummy2 = j;
            }

            if (testVal % (TestIterations/30) == 0)
            {
                LogText("LocklessTest::Producer - %5.2f%% done\n",
                        100.0f * (float)testVal/(float)TestIterations);
            }
        }

        LogText("LocklessTest::Producer::Run exiting.\n");
        return 0;
    }
};


} // namespace LocklessTest



void StartLocklessTest()
{
    // These threads will release themselves once done
    Ptr<LocklessTest::Producer> producerThread = *new LocklessTest::Producer;
    Ptr<LocklessTest::Consumer> consumerThread = *new LocklessTest::Consumer;

    producerThread->Start();
    consumerThread->Start();

    while (!producerThread->IsFinished() && consumerThread->IsFinished())
    {
        Thread::MSleep(500);
    }
}


} // namespace OVR

#endif // OVR_LOCKLESS_TEST
