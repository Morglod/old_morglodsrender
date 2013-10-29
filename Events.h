#pragma once

#ifndef _MR_EVENTS_H_
#define _MR_EVENTS_H_

#include <vector>

namespace MR{
    class EventCallback{
    private:
        void (*defaultCallback)(void* data1, void* data2) = nullptr;
    public:
        virtual inline void Set(void* callback){
            this->defaultCallback = callback;
        }
        virtual inline void Invoke(){
            if(this->defaultCallback != nullptr) this->defaultCallback(nullptr, nullptr);
        }
        virtual inline void Invoke(void* data1, void* data2){
            if(this->defaultCallback != nullptr) this->defaultCallback(data1, data2);
        }
    };

    class Event{
    protected:
        std::vector<EventCallback*> _callback_list;

        void inline _for_each_invoke(EventCallback* cb){
            cb->Invoke();
        }
    public:
        inline void Add(EventCallback* callback){
            this->_callback_list.push_back(callback);
        }
        inline void Sub(EventCallback* callback){
            auto it = std::find(this->_callback_list.begin(), this->_callback_list.end(), callback);
            if(it != this->_callback_list.end()) this->_callback_list.erase(it);
        }
        inline operator+=(EventCallback*& callback){
            this->Add(callback);
        }
        inline operator-=(EventCallback*& callback){
            this->Sub(callback);
        }
        inline Event Append(Event& e){
            Event e(e._callback_list.data(), e._callback_list.size());
            e._callback_list.insert(e._callback_list.end(), this->_callback_list.data(), this->_callback_list.size());
            return e;
        }
        inline void Invoke(){
            std::for_each(this->_callback_list.begin(), this->_callback_list.end(), _for_each_invoke);
        }
        inline void Invoke(void* data1, void* data2){
            if(this->defaultCallback != nullptr) this->defaultCallback(data1, data2);
        }

        Event(){}
        Event(EventCallback** callback_list, unsigned short num){
            this->_callback_list.insert(this->_callback_list.begin(), callback_list, num);
        }
        ~Event(){
            this->_callback_list.clear();
        }
    };
}

#endif
