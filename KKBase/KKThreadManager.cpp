/* KKThreadManager.cpp -- Manages the threads that perform the image extraction process.
 * Copyright (C) 2012-2014 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#include "FirstIncludes.h"
#include <string>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "MsgQueue.h"
#include "KKThread.h"
#include "OSservices.h"
#include "KKThreadManager.h"
using namespace KKB;






KKThreadManager::KKThreadManager (const KKStr&   _managerName,
                                  kkuint32       _maxNumThreads,
                                  MsgQueuePtr    _msgQueue
                                 ):
  KKThread (_managerName, NULL, _msgQueue),
  crashed            (false),
  doneExecuting      (false),
  maxNumThreads      (_maxNumThreads),
  shutdownFlag       (false),
  shutdownRequested  (false),
  terminateFlag      (false),
  terminateRequested (false),
  threads            (NULL)

{
  threads = new KKThreadList (true);
}




KKThreadManager::~KKThreadManager ()
{
  delete  threads;
  threads = NULL;
}



void  KKThreadManager::AddThread (KKThreadPtr  _thread)
{
  if  (!threads)
    threads = new KKThreadList (true);

  threads->PushOnBack (_thread);
}



void  KKThreadManager::RequestShutdown ()
{
  shutdownRequested = true;
}


void  KKThreadManager::RequestTerminate ()
{
  terminateRequested = true;
}




void  KKThreadManager::TerminateProcessing ()
{
  terminateFlag = true;
  if  (threads)
  {
    KKThreadList::iterator  idx;
    for  (idx = threads->begin ();  idx != threads->end ();  ++idx)
    {
      KKThreadPtr  thread = *idx;
      thread->TerminateThread ();
    }
  }
}  /* TerminateProcessing */




void  KKThreadManager::ShutdownProcessing (kkint32  miliSecsToWait)
{
  shutdownFlag = true;

  if  (!threads)
    return;

  kkint32  numMiliSecsWaited = 0;
  shutdownFlag = true;

  kkint32  lastNumThreadsRunning = 0;
  kkint32  lastNumThreadsStopped = 0;

  bool  allTreadsAreShutdown = false;

  while  (!allTreadsAreShutdown)
  {
    allTreadsAreShutdown = true;
    kkint32  numThreadsRunning = 0;
    kkint32  numThreadsStopped = 0;

    KKThreadList::iterator  idx;
    for  (idx = threads->begin ();  idx != threads->end ();  ++idx)
    {
      KKThreadPtr  thread = *idx;

      switch  (thread->Status ())
      {
      case  KKThread::ThreadStatus::Null:
    	break;

      case  KKThread::ThreadStatus::NotStarted:
      case  KKThread::ThreadStatus::Stopped:
        // Nothing to do;  this thread is not running,
        ++numThreadsStopped;
        break;

      case  KKThread::ThreadStatus::Stopping:
        allTreadsAreShutdown = false;
        ++numThreadsRunning;
        break;

      case  KKThread::ThreadStatus::Running:
        {
          allTreadsAreShutdown = false;
          if  (!(thread->ShutdownFlag ()))
          {
            if  (thread->OkToShutdown ())
            {
              thread->ShutdownThread ();
              numMiliSecsWaited = 0;
            }
          }
          ++numThreadsRunning;
        }
        break;

      case  KKThread::ThreadStatus::Starting:
        // Have to let it finish starting before we can flag it to shutdown.
        ++numThreadsRunning;
        break;
      }  /* End of Switch */
    }

    if  ((numThreadsRunning != lastNumThreadsRunning)  ||
         (numThreadsStopped != lastNumThreadsStopped)
        )
    {
      // The status of one or more threads changed;  can reset the 'numMiliSecsWaited' clock.
      numMiliSecsWaited = 0;
    }

    if  (!allTreadsAreShutdown)
    {
      if  ((miliSecsToWait > 0)  &&  (numMiliSecsWaited > miliSecsToWait))
      {
        // The shutdown process is taking longer than the 'miliSecsToWait' parameter will allow for;  we need to terminate
        // all running threads now.
        KillAllRunningThreads ();
        break;
      }
      else
      {
        osSleep (0.01f);
        numMiliSecsWaited += 10;
      }
    }
  }
}  /* ShutdownProcessing */



void  KKThreadManager::KillAllRunningThreads ()
{
  if  (!threads)
    return;

  KKThreadList::iterator  idx;
  for  (idx = threads->begin ();   idx != threads->end ();  ++idx)
  {
    KKThreadPtr  thread = *idx;
    thread->Kill ();
  }
}  /* KillAllRunningThreads*/



void  KKThreadManager::StartThreads (bool&  successful)
{
  AddMsg ("KKThreadManager::StartThreads");

  successful = true;

  if  (!threads)
    return;

  bool  allThreadsStarted = false;

  while  (!allThreadsStarted)
  {
    allThreadsStarted = true;
    KKThreadList::iterator  idx;
    for  (idx = threads->begin ();  idx != threads->end ();  ++idx)
    {
      KKThreadPtr  thread = *idx;
      if  (thread->Crashed ())
      {
        crashed = true;
        successful = false;
        break;
      }

      if  (thread->Status () == KKThread::ThreadStatus::NotStarted)
      {
        if  (thread->OkToStart ())
        {
          bool  successfulStart = false;
          thread->Start (ThreadPriority::Normal, successfulStart);
          if  (!successfulStart)
          {
            allThreadsStarted = false;
            successful = false;
            break;
          }
        }
        else
        {
          allThreadsStarted = false;
        }
      }
    }

    if  (!allThreadsStarted)
    {
      osSleep (0.01f);
    }
  }

}  /* StartThreads */



bool  KKThreadManager::AnyProcessorsCrashed ()
{
  bool  anyProcessorsCrashed = false;

  if  (threads)
  {
    KKThreadList::iterator  idx;
    for  (idx = threads->begin ();  idx != threads->end ();  ++idx)
    {
      KKThreadPtr  t = *idx;
      if  (t->Crashed ())
      {
        anyProcessorsCrashed = true;
        break;
      }
    }
  }

  crashed = anyProcessorsCrashed;

  return  anyProcessorsCrashed;
}  /* AnyProcessorsCrash */



bool  KKThreadManager::AllThreadsTerminated ()
{
  bool  allThreadsTerminated = true;

  if  (threads)
  {
    KKThreadList::const_iterator  idx;
    for  (idx = threads->begin ();  idx != threads->end ();  ++idx)
    {
      KKThreadPtr  t = *idx;
      if  (t->Status () != KKThread::ThreadStatus::Stopped)
      {
        allThreadsTerminated = false;
        break;
      }
    }
  }

  return  allThreadsTerminated;
}  /* AllThreadsTerminated */





void  KKThreadManager::ManageThreads (bool&  successful)
{
  successful= false;
  if (!threads)
    return;

  StartThreads (successful);
  if  (!successful)
    return;


  while  (true)
  {
    if  (shutdownRequested)
    {
      shutdownRequested = false;
      ShutdownProcessing (0);
      break;
    }

    else if  (terminateRequested)
    {
      terminateRequested = false;
      TerminateProcessing ();
      break;
    }

    else
    {
      osSleep (0.05f);
    }
  }


  if  (AnyProcessorsCrashed ())
  {
    AddMsg ("One or more processors crashed!!!");
    crashed = true;
  }
}  /* ManageThreads */



void  KKThreadManager::Run ()
{
}
