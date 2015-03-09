#include "DOF.h"

using namespace DOF;

Set DOF::setInverse(Set S){
    return ~S;
}

 Set DOF::setUnion(Set A, Set B){
    return A | B;
}

 Set DOF::setIntersection(Set A, Set B){
     return A & B;
 }

 Set DOF::setRemove(Set A, Set B){
     return A & setInverse(B);
 }

int DOF::setContains(Set S, Lone D){
    return (S & (1 << D)) >> D;
}

int DOF::setIndexOf(Set S, Lone D){
    int index = -1;
    for (int d = 0; d < COUNT; d++){
        if (setContains(S, (Lone)d)) index++;
        if (d == D) return index;
    }
    return index;
}

Count DOF::setSize(Set S){
    return setContains(S, X) + setContains(S, Y) + setContains(S, Z) +
           setContains(S, RX) + setContains(S, RY) + setContains(S, RZ);
}

DOF::Set DOF::setCreate(Lone last){
    return (1 << last);
}

std::string DOF::setStr(Set S){
    std::string s(6, '-');
    s[0] = setContains(S, X) ? 'X' : '-';
    s[1] = setContains(S, Y) ? 'Y' : '-';
    s[2] = setContains(S, Z) ? 'Z' : '-';
    s[3] = setContains(S, RX) ? 'U' : '-';
    s[4] = setContains(S, RY) ? 'V' : '-';
    s[5] = setContains(S, RZ) ? 'W' : '-';
    return s;
}

std::string DOF::setStr(Lone d){
    if (d == 0) return "X";
    if (d == 1) return "Y";
    if (d == 2) return "Z";
    if (d == 3) return "RX";
    if (d == 4) return "RY";
    if (d == 5) return "RZ";
    return std::to_string((int)d);
}

Lone DOF::nextDOF(Set S, Lone d){
    for (int i = d+1; i < COUNT; i++){
        if (setContains(S, (Lone)i)) return (Lone)i;
    }
    return COUNT;
}

Lone DOF::firstDOF(Set S){
    return nextDOF(S, (Lone)(-1));
}