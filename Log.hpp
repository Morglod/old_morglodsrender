#pragma once

#ifndef _MR_LOG_H_
#define _MR_LOG_H_

#include <vector>
#include <algorithm>
#include <string>

#define MR_LOG_LEVEL_DEFAULT 0
#define MR_LOG_LEVEL_EXCEPTION -1
#define MR_LOG_LEVEL_WARNING -2
#define MR_LOG_LEVEL_ERROR -3
#define MR_LOG_LEVEL_INFO -4

#include "Exception.hpp"

namespace MR{
    class Log{
    public:
        typedef void (*LogStringPtr)(const std::string&, const int&);

        static void Add(const LogStringPtr& ptr){ _callbacks.push_back(ptr); }
        static void Remove(const LogStringPtr& ptr){ _callbacks.erase( std::find(_callbacks.begin(), _callbacks.end(), ptr) ); }

        static void LogString(const std::string & s, const int & level = MR_LOG_LEVEL_DEFAULT){
            for(size_t i = 0; i < _callbacks.size(); ++i){
                _callbacks[i](s, level);
            }
        }

        static void LogException(MR::Exception & e, const int & level = MR_LOG_LEVEL_EXCEPTION){
            for(size_t i = 0; i < _callbacks.size(); ++i){
                _callbacks[i](e.what(), level);
            }
        }

    protected:
        static std::vector<LogStringPtr> _callbacks;
    };
}

#endif
