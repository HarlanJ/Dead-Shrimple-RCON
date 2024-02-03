#include <cstring>

#include "RCON.hpp"

RCON::RCON(std::string payload, int32_t ID, RCON::Type type)
:size(RCON::MIN_SIZE + payload.size())
,ID(ID)
,type(type)
,payload(payload)
{}

std::string RCON::getPrintable(bool showID)
{
    if(showID){
        constexpr size_t IDLen = 4;
        pchar ID[IDLen+1] = {' '};
        sprintf(ID, "%*d ", IDLen, payload.size());
        std::string ret(payload);
        ret.insert(0, ID, IDLen+1);
        return ret; 
    } else {
        return payload;
    }
}

RCON* RCON::decodeRCON(pchar*& rawRCON){
    decltype(RCON::size) size;
    RCON::littleEndian(&rawRCON[0],  &size);
    
    decltype(RCON::ID) ID;
    RCON::littleEndian(&rawRCON[sizeof(RCON::size)], &ID);

    RCON::Type type;
    std::underlying_type<RCON::Type>::type typeUnderlying;
    RCON::littleEndian(&rawRCON[sizeof(RCON::size)+sizeof(RCON::ID)], &typeUnderlying);
    type = static_cast<RCON::Type>(typeUnderlying);
    
    return new RCON(
        std::string(&rawRCON[12], size-RCON::MIN_SIZE), 
        ID,
        type
    );
}

void RCON::encodeRCON(pchar*& rawRCON){
    if(rawRCON == nullptr){
        rawRCON = new pchar[this->getBufferSize()];
    }

    RCON::littleEndian(this->size, &rawRCON[0]);
    RCON::littleEndian(this->ID,   &rawRCON[sizeof(this->size)]);
    RCON::littleEndian(
        static_cast<std::underlying_type<RCON::Type>::type>(this->type),
        &rawRCON[sizeof(this->size)+sizeof(this->ID)]
    );
    memcpy(
        &rawRCON[sizeof(this->size)+sizeof(this->ID)+sizeof(this->type)],
        payload.c_str(), payload.size()
    );
    rawRCON[this->getBufferSize()-2] = rawRCON[this->getBufferSize()-1] = 0x00;
}
