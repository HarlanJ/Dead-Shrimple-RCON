#ifndef ___RCON_HPP___
#define ___RCON_HPP___

#include <cstdint>
#include <string>

#include "plibsys.h"

class RCON{
    public:

        enum Type : int32_t{
            SERVERDATA_RESPONSE_VALUE = 0,
            SERVERDATA_EXECCOMMAND    = 2,
            SERVERDATA_AUTH_RESPONSE  = 2,
            SERVERDATA_AUTH           = 3
        };

        RCON(std::string payload, int32_t ID=0, RCON::Type type=RCON::Type::SERVERDATA_EXECCOMMAND);

        std::string getPrintable(bool showID=false);

        void encodeRCON(pchar*& rawRCON);
        static RCON* decodeRCON(pchar*& rawRCON);

        // Returns total buffer size
        int32_t     getSize()   { return size+sizeof(size);    }
        int32_t     getID()     {   return ID;    }
        RCON::Type  getType()   { return type;    }
        std::string getPayload(){ return payload; }

        void setID(int32_t ID)              { this->ID = ID;      }
        void setType(RCON::Type type)       { this->type = type;    }
        void setPayload(std::string payload){ this->payload = payload; }
    
    private:
        // RCON size, not payload or total size.
        int32_t size;
        int32_t ID;
        RCON::Type type;
        std::string payload;

        static constexpr decltype(size) MIN_SIZE = sizeof(decltype(ID))+sizeof(RCON::Type)+1+1; // size of ID + size of Type + payload null + msg null

        template< typename T, typename std::enable_if< std::is_integral<T>::value >::type* = nullptr >
        static void littleEndian(T srcVal, pchar* dest){
            for(int i = 0; i < sizeof(T); i++){
                dest[i] = static_cast<pchar>((srcVal>>(8*i)) & 0xFF);
            }
        }

        template< typename T, typename std::enable_if< std::is_integral<T>::value >::type* = nullptr >
        static void littleEndian(pchar* src, T *destVal){
            *destVal = 0;
            for(int i = 0; i < sizeof(T); i++){
                *destVal |= static_cast<T>(src[i])<<(8*i);
            }
        }

};

#endif