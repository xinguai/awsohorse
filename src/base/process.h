/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */


#ifndef PROCESS_H_
#define PROCESS_H_

#include <sys/types.h>

namespace base{

typedef pid_t ProcessHandle;
typedef pid_t ProcessId;

const ProcessHandle kNullProcessHandle = 0;
const ProcessId     kNullProcessId     = 0;

class Process{
  public:
	Process():process_(kNullProcessHandle){
	}

	explicit Process(ProcessHandle handle):process_(handle){
	}

	//A handle to the current process.
	static Process Current();

	static bool CanBackgroundProcesses();

	//Get/Set the handle for this process. The Handle will be 0 if the process
	//is no longer running
	ProcessHandle handle() const {
		return process_;
	}

	void set_handle(ProcessHandle handle){
		process_ = handle;
	}

	//Get the pid of the process.
	ProcessId pid() const;

	//Is the this process the current process.
	bool is_current() const;

	//Close the process handle. This will not terminate the process.
	void Close();

	// Terminates the process with extreme prejudice. The given result code will
    // be the exit code of the process. If the process has already exited, this
    // will do nothing.
    void Terminate(int result_code);
	
	// A process is backgrounded when it's priority is lower than normal.
    // Return true if this process is backgrounded, false otherwise.
	bool IsProcessBackgrounded() const;
    // Set a process as backgrounded. If value is true, the priority
    // of the process will be lowered. If value is false, the priority
    // of the process will be made "normal" - equivalent to default
    // process priority.
    // Returns true if the priority was changed, false otherwise.
    bool SetProcessBackgrounded() const;
	// Returns an integer representing the priority of a process. The meaning
    // of this value is OS dependent.
    int GetPriority() const;
	
  private:
  	ProcessHandle process_;
};

}  //namespace base

#endif /* PROCESS_H_ */
