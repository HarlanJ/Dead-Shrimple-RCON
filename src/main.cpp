#include <stdio.h>
#include <stdint.h>
#include <string>

#include "plibsys.h"
#include "CLI/CLI.hpp"

#include "RCON.hpp"

enum RCON_TYPE : int32_t{
    SERVERDATA_AUTH           = 3,
    SERVERDATA_AUTH_RESPONSE  = 2,
    SERVERDATA_EXECCOMMAND    = 2,
    SERVERDATA_RESPONSE_VALUE = 0
};

// const pchar* ip = "10.203.81.120";
// puint16 port = 49493;

int main(int argc, char** argv){
    bool quiet{false};

    CLI::App app{"Dead Shrimple RCON"};

    std::string password;
    std::string ips{"127.000.000.001"};
    puint16 port{27015};
    bool interactive{false};
    std::vector<std::string> run;
    
    app.add_flag("-q,--quiet", quiet, "Reduces textual output, use this for scripts");
    app.add_flag("-i,--interactive", interactive, "Useful for debugging and use as an RCON client. Mutually exclusive with --run"); // exclusion at --run

    app.add_option("-4,--ip4,ip", ips, "IPv4 address to connect to");
    app.add_option("-p,--port,port", port, "RCON port to connect to");
    app.add_option("-a,--auth,password", password, "The authentication password for the server");
    app.add_option("-r,--run", run, "Commands to run, in order. Mutually exclusive with --interactive")->excludes("--interactive");

    CLI11_PARSE(app, argc, argv);

    if(!quiet) printf("Connecting to %s on port %d...", ips.c_str(), port);

    p_libsys_init();

    PError* err = NULL;
    PSocket* socket = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM , P_SOCKET_PROTOCOL_DEFAULT, &err);
    if(err != NULL){
        printf("Could not create socket: %s\n", p_error_get_message(err));
        p_error_free(err);
        p_socket_free(socket);
        return 0;
    }

    PSocketAddress* address = p_socket_address_new(ips.c_str(), port);
    p_socket_set_blocking(socket, TRUE);
    p_socket_set_timeout(socket, 5000);
    p_socket_set_buffer_size(socket, P_SOCKET_DIRECTION_RCV, 4096, NULL);
    if(!p_socket_connect(socket, address, &err)){
        printf("Failed to connect: %s\nNative: %d\n", p_error_get_message(err), p_error_get_code(err));

        p_error_free(err);
        p_socket_address_free(address);
        p_socket_free(socket);
    }
    p_socket_address_free(address);

    printf("connected.\n");

    
    RCON* msg;
    constexpr size_t bufferSize = 4096;
    pchar* buffer = new pchar[bufferSize];

    // -------- AUTH -------- //
    msg = new RCON(password, 0, RCON::SERVERDATA_AUTH);
    msg->encodeRCON(buffer);
    p_socket_send(socket, buffer, msg->getSize(), nullptr);
    delete msg;

    p_socket_receive(socket, buffer, bufferSize, nullptr);
    msg = RCON::decodeRCON(buffer);
    printf("%s\n", msg->getPrintable().c_str());
    delete msg;
    // -------- AUTH -------- //

    if(interactive){
        std::string cmd;

        while(true){
            uint16_t cmdLen;
            scanf("%s", buffer);
            cmd.assign(buffer);

            if(!cmd.compare("__EXIT__")) break;

            msg = new RCON(cmd, 0, RCON::SERVERDATA_EXECCOMMAND);
            msg->encodeRCON(buffer);
            p_socket_send(socket, buffer, msg->getSize(), nullptr);
            delete msg;

            p_socket_receive(socket, buffer, bufferSize, nullptr);
            msg = RCON::decodeRCON(buffer);
            printf("%s\n", msg->getPrintable().c_str());
            delete msg;
        }
    } else {
        for(std::string cmd : run){
            msg = new RCON(cmd, 0, RCON::SERVERDATA_EXECCOMMAND);
            msg->encodeRCON(buffer);
            p_socket_send(socket, buffer, msg->getSize(), nullptr);
            delete msg;

            p_socket_receive(socket, buffer, bufferSize, nullptr);
            msg = RCON::decodeRCON(buffer);
            printf("%s\n", msg->getPrintable().c_str());
            delete msg;
        }
    }
    

    /*
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
    */
}