//--------------
//MorglodsRender
//--------------

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

namespace MR{
    class Log{
    public:
        typedef void (*LogStringPtr)(std::string, int);
    protected:
        static std::vector<LogStringPtr> _callbacks;
    public:
        static void Add(LogStringPtr ptr){
            _callbacks.push_back(ptr);
        }

        static void Remove(LogStringPtr ptr){
            _callbacks.erase( std::find(_callbacks.begin(), _callbacks.end(), ptr) );
        }

        static void LogString(std::string s, int level = MR_LOG_LEVEL_DEFAULT){
            for(auto it = _callbacks.begin(); it != _callbacks.end(); it++){
                (*it)(s, level);
            }
        }

        static void LogException(std::exception & e, int level = MR_LOG_LEVEL_EXCEPTION){
            for(auto it = _callbacks.begin(); it != _callbacks.end(); it++){
                (*it)(e.what(), level);
            }
        }
    };
}

#endif
