// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef _UNI_PROCESSINFO_H_
#define _UNI_PROCESSINFO_H_

// #include <muduo/base/Types.h>
#include "Timestamp.h"
#include <vector>
#include <sys/types.h>


namespace ProcessInfo
{
    pid_t pid();

    std::string pidString();

    uid_t uid();

    std::string username();

    uid_t euid();

    CTimestamp startTime();

    std::string hostname();

    /// read /proc/self/status
    std::string procStatus();

    int openedFiles();

    int maxOpenFiles();

    int numThreads();

    std::vector<pid_t> threads();
}


#endif  // MUDUO_BASE_PROCESSINFO_H
