#pragma once

#include <Util/BoostEnumString.h>
#include <string>

struct Status{
    // CHECK RESULT HERE
    ENUM_STR(Code, 
        (UNDEFINED)

        (SUCCESS)
        (FAILURE)

        (NOT_IMPLEMENTED)

        (FILE_NOT_FOUND)
        (FILE_NOT_PARSED)

        (DOF_INVALID_LONE)

        (NODE_INVALID_ID)

        (MATERIAL_INVALID_ID)
        (MATERIAL_INVALID_TYPE)
        (MATERIAL_INVALID_NUM_PARAMETERS)

        (ELEMENT_INVALID_TYPE)
        (ELEMENT_INVALID_NUM_PARAMETERS)
        (ELEMENT_INVALID_NUM_NODES)
        )

    Code code;
    std::string detail;

    static std::string str[];
    
    inline Status() : code(UNDEFINED) {};

    inline Status(Code code, std::string message = "") : code(code), detail(message){}

    inline Status(Status& status, std::string message = "") : code(status.code) {
        detail = status.detail;
        if (status.detail != "" && message != "") detail = status.detail + " | " + message;
        else if (status.detail != "") detail = status.detail;
        else if (message != "") detail = message;
    }

    inline Status(Status& a, Status& b){
        if (a.code == Status::SUCCESS && b.code == Status::SUCCESS){
            code = Status::SUCCESS;
        }
        else if (a.code == Status::SUCCESS){
            code = b.code;
            detail = b.detail;
        }
        else{
            code = a.code;
            detail = a.detail;
        }
    }

    inline bool operator== (const Code other_code) { return (code == other_code); }

    inline bool operator!= (const Code other_code) { return (code != other_code); }
};

inline std::string to_string(Status s){ return std::string(Status::enum_cstr(s.code)) + " (" + s.detail + ")"; }

#define CHECK_STATUS(r) if (r != Status::SUCCESS){ std::cout << __FUNCTION__  << " L" << __LINE__ << ": " <<  to_string(r) << std::endl; exit(0); }
