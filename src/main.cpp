#include <stdio.h>
#include <stdint.h>
#include <string>

#include "plibsys.h"

#include "RCON.hpp"

enum RCON_TYPE : int32_t{
    SERVERDATA_AUTH           = 3,
    SERVERDATA_AUTH_RESPONSE  = 2,
    SERVERDATA_EXECCOMMAND    = 2,
    SERVERDATA_RESPONSE_VALUE = 0
};

const pchar* ip = "10.203.81.120";
puint16 port = 49493;

int main(){
    p_libsys_init();

    PError* err = NULL;

    PSocket* socket = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM , P_SOCKET_PROTOCOL_DEFAULT, &err);
    if(err != NULL){
        printf("There was a problem:%s\n", p_error_get_message(err));
        p_error_free(err);
    }

    PSocketAddress* address = p_socket_address_new(ip, port);
    p_socket_set_blocking(socket, TRUE);
    p_socket_set_buffer_size(socket, P_SOCKET_DIRECTION_RCV, 4096, NULL);
    if(!p_socket_connect(socket, address, NULL)){
        printf("Failed to connect to server\n");
    }
    p_socket_address_free(address);


    RCON* msg;
    constexpr size_t bufferSize = 4096;
    pchar* buffer = new pchar[bufferSize];

    // -------- Auth -------- //
    msg = new RCON("Adminhey", 0, RCON::SERVERDATA_AUTH);
    msg->encodeRCON(buffer);
    p_socket_send(socket, buffer, msg->getSize(), nullptr);
    delete msg;

    p_socket_receive(socket, buffer, bufferSize, nullptr);
    msg = RCON::decodeRCON(buffer);
    printf("%s\n", msg->getPrintable().c_str());
    delete msg;
    // -------- Auth -------- //


    // -------- Shutdown -------- //
    msg = new RCON("Shutdown 15");
    msg->encodeRCON(buffer);
    p_socket_send(socket, buffer, msg->getSize(), nullptr);
    delete msg;

    p_socket_receive(socket, buffer, bufferSize, nullptr);
    msg = RCON::decodeRCON(buffer);
    printf("%s\n", msg->getPrintable().c_str());
    delete msg;
    // -------- Shutdown -------- //

    // -------- Save -------- //
    msg = new RCON("Save");
    msg->encodeRCON(buffer);
    p_socket_send(socket, buffer, msg->getSize(), nullptr);
    delete msg;

    p_socket_receive(socket, buffer, bufferSize, nullptr);
    msg = RCON::decodeRCON(buffer);
    printf("%s\n", msg->getPrintable().c_str());
    delete msg;
    // -------- Save -------- //

    delete[] buffer;

    return 0;
}