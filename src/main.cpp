#include <stdio.h>
#include <stdint.h>
#include <string>

#include "plibsys.h"
#include "CLI/CLI.hpp"

#include "RCON.hpp"

/**
 * @brief Stores the RCON type field
 * 
 * Per RCON standard this is a *signed* 32-bit value. Ugh.
 * 
 */
enum RCON_TYPE : int32_t{
    SERVERDATA_AUTH           = 3,
    SERVERDATA_AUTH_RESPONSE  = 2,
    SERVERDATA_EXECCOMMAND    = 2,
    SERVERDATA_RESPONSE_VALUE = 0
};

int main(int argc, char** argv){
    /// This is a partially implemented quite mode, for scripts.
    bool quiet{false};

    /// CLI11 app, not sure why it has a name...
    CLI::App app{"Dead Shrimple RCON"};

    /// RCON password, passed in over CLI11, it's plaintext transmitted over TCP anyway
    std::string password;

    /// Default connection is localhost. plibsys doens't ahndle domain lookup, not sure if that includes localhost.
    std::string ips{"127.0.0.1"};

    /// 27015 is the default RCON port
    puint16 port{27015};
    /// flag for the interactive mode
    bool interactive{false};
    /// List of commands to run. Mutually exclusive with interactive
    std::vector<std::string> run;
    
    app.add_flag("-q,--quiet", quiet, "Reduces textual output, use this for scripts");
    app.add_flag("-i,--interactive", interactive, "Useful for debugging and use as an RCON client. Mutually exclusive with --run"); // exclusion at --run

    /// A note about `add_option`: the option names that don't start with a `-` are positional arguments.
    ///     Positional arguments are in the order of declaration, ex: `Dead_Shrimple_RCON 127.0.0.1 27015 AdminPass`
    ///     would have the expected outcome, but any other order will not work. See the 
    ///     [CLI11 GitBook page on Options](https://cliutils.github.io/CLI11/book/chapters/options.html) for more
    ///     details.
    app.add_option("-4,--ip4,ip", ips, "IPv4 address to connect to");
    app.add_option("-p,--port,port", port, "RCON port to connect to");
    app.add_option("-a,--auth,password", password, "The authentication password for the server");
    app.add_option("-r,--run", run, "Commands to run, in order. Mutually exclusive with --interactive")->excludes("--interactive");

    /// Actually perform the input parsing
    CLI11_PARSE(app, argc, argv);

    /// just some nice user output
    if(!quiet) printf("Connecting to %s on port %d...", ips.c_str(), port);

    /// Initialize pslibsys. duh.
    p_libsys_init();

    /// Start setting up a socket to use
    PError* err = NULL;
    PSocket* socket = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM , P_SOCKET_PROTOCOL_DEFAULT, &err);
    if(err != NULL){
        printf("Could not create socket: %s\n", p_error_get_message(err));
        p_error_free(err);
        p_socket_free(socket);

        if(!quiet) printf("exiting\n");
        return 0;
    }

    /// Connect the socket to the server
    PSocketAddress* address = p_socket_address_new(ips.c_str(), port);
    p_socket_set_blocking(socket, TRUE);
    p_socket_set_timeout(socket, 5000);
    p_socket_set_buffer_size(socket, P_SOCKET_DIRECTION_RCV, 4096, NULL);
    if(!p_socket_connect(socket, address, &err)){
        printf("Failed to connect: %s\nNative: %d\n", p_error_get_message(err), p_error_get_code(err));

        p_error_free(err);
        p_socket_address_free(address);
        p_socket_free(socket);

        if(!quiet) printf("exiting\n");
        return 0;
    }
    p_socket_address_free(address);


    /// More user output
    if(!quiet) printf("connected.\n");

    /// Basic authentication with the RCON server
    RCON* msg;
    constexpr size_t bufferSize = 4096;
    pchar* buffer = new pchar[bufferSize];

    msg = new RCON(password, 0, RCON::SERVERDATA_AUTH);
    msg->encodeRCON(buffer);
    p_socket_send(socket, buffer, msg->getBufferSize(), nullptr);
    delete msg;

    p_socket_receive(socket, buffer, bufferSize, nullptr);
    msg = RCON::decodeRCON(buffer);
    printf("%s\n", msg->getPrintable().c_str());
    delete msg;

    /// Two different loops. One for interactive mode (like a REPL), one for input commands
    if(interactive){
        /// The command from the user
        std::string cmd;

        /// Loop until the user wants to quit
        while(true){
            uint16_t cmdLen;
            scanf("%s", buffer);
            cmd.assign(buffer);

            /// Exit command breaks out of the loop
            if(!cmd.compare("__EXIT__")) break;

            /// Assemble and send the RCON message
            msg = new RCON(cmd, 0, RCON::SERVERDATA_EXECCOMMAND);
            msg->encodeRCON(buffer);
            p_socket_send(socket, buffer, msg->getBufferSize(), nullptr);
            delete msg;

            p_socket_receive(socket, buffer, bufferSize, nullptr);
            msg = RCON::decodeRCON(buffer);
            printf("%s\n", msg->getPrintable().c_str());
            delete msg;
        }
    } else {
        /// Loop through all of the command strings passed in form the CLI
        for(std::string cmd : run){
            msg = new RCON(cmd, 0, RCON::SERVERDATA_EXECCOMMAND);
            msg->encodeRCON(buffer);
            p_socket_send(socket, buffer, msg->getBufferSize(), nullptr);
            delete msg;

            p_socket_receive(socket, buffer, bufferSize, nullptr);
            msg = RCON::decodeRCON(buffer);
            printf("%s\n", msg->getPrintable().c_str());
            delete msg;
        }
    }
}