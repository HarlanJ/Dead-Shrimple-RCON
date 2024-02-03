#ifndef ___RCON_HPP___
#define ___RCON_HPP___

#include <cstdint>
#include <string>

#include "plibsys.h"

/**
 * @brief The RCON class represents an RCON message.
 * 
 * RCON is a fairly simple protocol, and it's mostly
 * a transport layer, so RCON is pretty damn near a
 * wrapper around a string.
 */
class RCON{
    public:

        /**
         * @brief RCON::Type is an enum of the RCON packet types.
         */
        enum Type : int32_t{
            SERVERDATA_RESPONSE_VALUE = 0,
            SERVERDATA_EXECCOMMAND    = 2,
            SERVERDATA_AUTH_RESPONSE  = 2,
            SERVERDATA_AUTH           = 3
        };

        /**
         * @brief Construct a new RCON object
         * 
         * @param payload std::string the data to be sent to the server, typically a command.
         * @param ID int32_t the message ID. Responses to client messages have the same ID
         * as the original message. Default 0.
         * @param type RCON::Type of the message. First message will probably be
         * SERVERdATA_AUTH, most others will be SERVERDATA_EXECCOMMAND
         */
        RCON(std::string payload, int32_t ID=0, RCON::Type type=RCON::Type::SERVERDATA_EXECCOMMAND);

        /**
         * @brief Get a string that represents the RCON message. This is not the TCP
         * payload that is sent to the RCON server, instead, as the name suggests,
         * it's printable.
         * 
         * @param showID bool whether or not to show the RCON message ID
         * @return std::string pretty-printable string 
         */
        std::string getPrintable(bool showID=false);

        /**
         * @brief Populates a byte pointer with the data from this RCON object
         * 
         * encodeRCON takes in a byte pointer reference and will fill it with the
         * TCP payload. If the pointer is nullptr, space will be allocated on the heap.
         * If the pointer is not nullptr, and the allocated space is insufficient, the 
         * memory will be overrun. Yeah.
         * 
         * @param rawRCON pchar*& to load with data to send to the server
         */
        void encodeRCON(pchar*& rawRCON);

        /**
         * @brief RCON object factory
         * 
         * Accepts a byte pointer reference that contains the TCP payload
         * received from the server.
         * 
         * @param rawRCON pchar*& The TCP payload received from the server
         * @return RCON* Newly allocated RCON object.
         */
        static RCON* decodeRCON(pchar*& rawRCON);

        /**
         * @brief Gets the total TCP packet payload size
         * 
         * @return int32_t 
         */
        int32_t     getBufferSize(){return size+sizeof(size); }

        /**
         * @brief Get the size of the RCON packet as reported in the transmission to the server
         * 
         * @return int32_t 
         * 
         * @note This is not the total size of the RCON packet.
         * @see RCON::getBufferSize()
         */
        int32_t     getSize()      { return size;    }

        /**
         * @brief Get the ID associated with this RCON object
         * 
         * @return int32_t 
         */
        int32_t     getID()        { return ID;      }

        /**
         * @brief Get the RCON::Type of this RCON message
         * 
         * @return RCON::Type 
         */
        RCON::Type  getType()      { return type;    }
        /**
         * @brief Gets the std::string payload of this RCON message by value.
         * 
         * @return std::string 
         */
        std::string getPayload()   { return payload; }

        /**
         * @brief Sets the ID of this RCON object.
         * 
         * @param ID 
         */
        void setID(int32_t ID)              { this->ID = ID;      }

        /**
         * @brief Set the Type of this RCON message
         * 
         * @param type 
         */
        void setType(RCON::Type type)       { this->type = type;    }

        /**
         * @brief Set the payload string of this RCON message.
         * 
         * @param payload 
         */
        void setPayload(std::string payload){ this->payload = payload; }
    
    private:
        /// @brief RCON size. Not the full payload size.
        int32_t size;

        /// @brief RCON message ID. Used to identify messages returning from the server
        int32_t ID;

        /// @brief RCON::Type of the message
        RCON::Type type;

        /// @brief RCON payload string
        std::string payload;

        /// @brief The minimum size of the TCP payload of an RCON packet.
        static constexpr decltype(size) MIN_SIZE = sizeof(decltype(ID))+sizeof(RCON::Type)+1+1; // size of ID + size of Type + payload null + msg null

        /**
         * @brief Copies an integral value into a byte array in little endian.
         * 
         * @tparam T The integral type of the source
         * @tparam std::enable_if< std::is_integral<T>::value >::type The magic soup that ensures T is integral
         * @param srcVal T The source value to copy into dest
         * @param dest pchar* to copy srcVal to
         * 
         * @warning dest is assumed to be allocated. No check or allocation is performed.
         * 
         * @todo check or allocate dest
         */
        template< typename T, typename std::enable_if< std::is_integral<T>::value >::type* = nullptr >
        static void littleEndian(T srcVal, pchar* dest){
            for(int i = 0; i < sizeof(T); i++){
                dest[i] = static_cast<pchar>((srcVal>>(8*i)) & 0xFF);
            }
        }

        /**
         * @brief Copes an integral value stored in little endian in a byte array into an integral variable.
         * 
         * @tparam T The integral type of the destination
         * @tparam std::enable_if< std::is_integral<T>::value >::type The magic soup that ensures T is integral
         * @param src pchar* to copy into destVal
         * @param destVal T* the destination variable for the value in src
         * 
         * @warning src and destVal are assumed to be allocated. No check or alloacion is performed.
         * 
         * @todo check src for null and allocated destVal if required
         */
        template< typename T, typename std::enable_if< std::is_integral<T>::value >::type* = nullptr >
        static void littleEndian(pchar* src, T *destVal){
            *destVal = 0;
            for(int i = 0; i < sizeof(T); i++){
                *destVal |= static_cast<T>(src[i])<<(8*i);
            }
        }

};

#endif