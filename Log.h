//--------------
//MorglodsRender
//--------------

#pragma once

#ifndef _MR_LOG_H_
#define _MR_LOG_H_

#include <vector>
#include <algorithm>
#include <string>

namespace MR{
    class Log{
    public:
        typedef void (*LogStringPtr)(std::string);
    protected:
        static std::vector<LogStringPtr> _callbacks;
    public:
        static void Add(LogStringPtr ptr){
            _callbacks.push_back(ptr);
        }

        static void Remove(LogStringPtr ptr){
            _callbacks.erase( std::find(_callbacks.begin(), _callbacks.end(), ptr) );
        }

        static void LogString(std::string s){
            for(auto it = _callbacks.begin(); it != _callbacks.end(); it++){
                (*it)(s);
            }
        }

        static void LogException(std::exception & e){
            for(auto it = _callbacks.begin(); it != _callbacks.end(); it++){
                (*it)(e.what());
            }
        }
    };
}

#endif
