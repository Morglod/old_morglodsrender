#pragma once
 //!!BAD BAD BAD !! BAD
#ifndef _MR_LIST_H_
#define _MR_LIST_H_

namespace MR{
    template<typename _T>
    class ListElement{
    public:
        MR::ListElement<_T>* PrevousElement = NULL;
        MR::ListElement<_T>* NextElement = NULL;
        _T Value;

        //Remove element from list
        inline void Remove(){
            if(this->PrevousElement != NULL) this->PrevousElement->NextElement = this->NextElement;
            if(this->NextElement != NULL) this->NextElement->PrevousElement = this->PrevousElement;
            //delete (this->Value);
        }

        ListElement(_T value, MR::ListElement<_T>* prev, MR::ListElement<_T>* next){
            this->Value = value;
            this->PrevousElement = prev;
            this->NextElement = next;
        }
        ~ListElement(){this->Remove();}
    };

    template<typename _T>
    class ListForEach{
    public:
        unsigned int CurrentIndex = 0;
        MR::ListElement<_T> * Current = NULL;

        inline MR::ListElement<_T> * Next(){
            CurrentIndex++;
            Current = Current->NextElement;
            return Current;
        }

        inline MR::ListElement<_T> * Prevous(){
            CurrentIndex--;
            Current = Current->PrevousElement;
            return Current;
        }

        ListForEach(MR::ListElement<_T> * el, unsigned int index){this->Current = el; this->CurrentIndex = index;}
    };

    template<typename _T>
    class List{
    public:
        MR::ListElement<_T>* FirstElement = NULL;
        unsigned int ElementsNum = 0;

        //Adds element to the end of list
        inline void Add(_T value){
            MR::ListElement<_T> * e = new MR::ListElement<_T>(value, NULL, NULL);
            if(this->ElementsNum > 1){
                e->PrevousElement = this->LastElement;
                this->LastElement->NextElement = e;
                this->LastElement = e;
                this->ElementsNum++;
            }
            else if(this->ElementsNum == 1){
                e->PrevousElement = this->FirstElement;
                this->FirstElement->NextElement = e;
                this->LastElement = e;
                this->ElementsNum++;
            }
            else if(this->ElementsNum == 0) this->FirstElement = e;
        }

        //Removes last element from list
        inline void RemoveLast(){
            if(this->ElementsNum > 2){
                MR::ListElement<_T> * e = this->LastElement->PrevousElement;
                this->LastElement->Remove();
                this->LastElement = e;
                this->ElementsNum--;
            }
            else if(this->ElementsNum == 2){
                this->LastElement->Remove();
                this->LastElement = NULL;
                this->FirstElement->NextElement = NULL;
                this->ElementsNum = 1;
            }
            else if(this->ElementsNum == 1){
                this->FirstElement->Remove();
                this->ElementsNum = 0;
            }
        }

        //Gets element at index or NULL
        MR::ListElement<_T> * At(unsigned int index){
            if(this->ElementsNum > index) return NULL;

            unsigned int current_index = 0;
            MR::ListElement<_T> * current_element = this->FirstElement;

            //Choose walk technique;
            //if(index < this->ElementsNum/2){
                while(current_element != NULL){
                    if(current_index == index) return current_element;
                    current_index++;
                    current_element = current_element->NextElement;
                }
            //}
            /*else{
                current_element = this->LastElement;
                current_index = this->ElementsNum-1;
                while(current_element != NULL){
                    if(current_index == index) return current_element;
                    if(current_index == 0) return NULL;
                    else current_index--;
                    current_element = current_element->PrevousElement;
                }
            }*/

            return NULL;
        }

        //Removes element at index
        inline void RemoveAt(unsigned int index){
            MR::ListElement<_T> * e = this->At(index);
            e->Remove();
        }

        //Insert element at index
        inline void Insert(unsigned int index, _T value){
            MR::ListElement<_T> * e = new MR::ListElement<_T>(value, NULL, NULL);
            if(index == 0){
                this->FirstElement->PrevousElement = e;
                e->NextElement = this->FirstElement;
                this->FirstElement = e;
            }
            else{
                MR::ListElement<_T> * ie = this->At(index);
                e->PrevousElement = ie->PrevousElement;
                ie->PrevousElement = e;
                e->NextElement = ie;
            }
        }

        //Removes all elements from list
        inline void Clear(){
            for(unsigned int i = this->ElementsNum; 0 < i; i--){
                this->RemoveLast();
            }
        }
    };
}

#endif // _MR_LIST_H_
