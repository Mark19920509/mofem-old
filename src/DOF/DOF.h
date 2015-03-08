#pragma once

#include <string>

namespace DOF{

    enum  Lone { X = 0, Y = 1, Z = 2, RX = 3, RY = 4, RZ = 5, COUNT = 6 };
    using Id = int;
    using Set = char;
    using Count = int;
    
    Set setInverse(Set S);
    Set setUnion(Set A, Set B);
    Set setRemove(Set A, Set B);
    Set setIntersection(Set A, Set B);
    int setContains(Set S, Lone D);
    Count setSize(Set S);

    Lone firstDOF(Set S);
    Lone nextDOF(Set S, Lone d);

    std::string setStr(Set S);
    std::string setStr(Lone d);

    Set setCreate(Lone last);
    template<typename... Args> Set setCreate(Lone first, Args... rest){
        return setUnion(1 << first, setCreate(rest...));
    };

    DOF::Set const SET_EMPTY = 0;
    DOF::Set const SET_XYZ = setCreate(X, Y, Z);
    DOF::Set const SET_ALL = setCreate(X, Y, Z, RX, RY, RZ);
}

namespace std{
    /** Get string representation of DOF set S*/
    inline string to_string(DOF::Set s){
        return DOF::setStr(s);
    }

    /** Get string representation of DOF d*/
    inline string to_string(DOF::Lone d){
        return DOF::setStr(d);
    }
}
