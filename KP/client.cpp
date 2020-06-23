#include <zmq.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <unistd.h>
#include <csignal>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include "data.h"
#include <array>

Message message;
std::array<void*,3> servers;

void menu() {
    printf("****************************************\n");
    printf("1 - Send message                        \n");
    printf("2 - Show history                        \n");
    printf("-1 - Try another server                 \n");
    printf("0 - Exit this server                    \n");
    printf("****************************************\n");
}

void menu1() {
    printf("****************************************\n");
    printf("1 - Chose server to work                \n");
    printf("2-Continue this server                  \n");
    printf("server 0 - 1st tour port and etc        \n");
    printf("0 - Exit  messager                      \n");
    printf("****************************************\n");
}

void send_recv(){


    message.action = SEND_TO_SUBS;

    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(Message));
    memcpy(zmq_msg_data(&request), &message, sizeof(Message));
    for(int i = 0; i<1; i++){
        zmq_msg_send(&request, servers[i], 0);
    }
    zmq_msg_close(&request);

    char ans[256];
    zmq_msg_init(&request);
    for(int i = 0; i<1; i++){
        zmq_msg_recv(&request, servers[i], 0);
    }
    strcpy(ans, (char *) zmq_msg_data(&request));
    zmq_msg_close(&request);

    printf("%s",ans);
    std::cout << std::endl;
    

}

void client_off(){


    message.action = CLIENT_OFF;

    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(Message));
    memcpy(zmq_msg_data(&request), &message, sizeof(Message));
    for(int i = 0; i<1; i++){
        zmq_msg_send(&request, servers[i], 0);
    }
    zmq_msg_close(&request);

    char ans[256];
    zmq_msg_init(&request);
    for(int i = 0; i<1; i++){
        zmq_msg_recv(&request, servers[i], 0);
    }
    strcpy(ans, (char *) zmq_msg_data(&request));
    zmq_msg_close(&request);

    printf("%s\n",ans);
}


int main(int argc, char *argv[]) {


    std::signal(SIGUSR1,send_recv);
    std::signal(SIGHUP,client_off);
    std::array<void*,3> contexts;


    int act = 0;
    char ans[256];
    int k;
    printf("Enter the numder of servers <4\n");
    scanf("%d", &k);
    for(int i = 0; i<k; i++){
        char portin [5];
        char *port;

        contexts.at(i) = zmq_ctx_new();
        servers.at(i) = zmq_socket(contexts[i], ZMQ_REQ);

        printf("Enter the port\n");
        scanf("%s",portin);
        ans[0] = '\0';
        strcat(ans, "tcp://localhost:");
        port = (char *) portin;
        strcat(ans, port);
        int rc = zmq_connect(servers[i], ans);
        if (rc != 0) {
            perror("zmq_connect");
            zmq_close(servers[i]);
            zmq_ctx_destroy(contexts[i]);
            exit(1);
        }
    }
    
    printf("Enter client name:\n");
    scanf("%s", message.clientName);
    message.client_proc_id = getpid();
    std::cout << "client pid = " << message.client_proc_id << std::endl;
    message.action = INIT;
    printf("1-Chose server to work\n");
    printf("2-Cont this server\n");
    printf("0-exit-Cont this server\n");
    int state=1;
    int serv;
    while(state!=0){
        printf("Enter the action with servers\n");
        menu1();
        scanf("%d",&state);
        if (state == 1){
            scanf("%d",&serv);
        }
        else if (state == 2){
            zmq_msg_t clientReq;
             zmq_msg_init_size(&clientReq, sizeof(Message));
            memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));
            zmq_msg_send(&clientReq, servers[serv], ZMQ_DONTWAIT);
            zmq_msg_close(&clientReq);

            zmq_msg_t reply;
            zmq_msg_init(&reply);
            zmq_msg_recv(&reply, servers[serv], 0);
            strcpy(ans, (char *) zmq_msg_data(&reply));

            if (strcmp(ans, "OK") == 0) {
                printf("\nWelcome~\n");
            } else if (strcmp(ans, "ERROR") == 0) {
                printf("Sorry! Server returned error, try again\n");
                exit(1);
            } else {
                printf("\nWelcome~\n");
                 printf("%s\n", ans);
            }
            zmq_msg_close(&reply);

            for (;;) {
                menu();
                if (scanf("%d", &act) == EOF) {
                    act = 0;
                }


                if (act == 1) {
                    message.action = SEND_MESSAGE;
                    printf("Type users name <name name ...>:\n");
                    std::string str;
                    getchar();
                    std::getline(std::cin, str);

                    std::istringstream iss(str);
                    int count = 0;
                    for(std::string s; iss >> s; ) {
                        strcpy(message.names[count].Client, s.c_str());
                        ++count;
                    }

                    message.sizeOfNames = count;
                    printf("Type message:\n");
                    std::getline(std::cin, str);

                    strcpy(message.text, str.c_str());
                    zmq_msg_init_size(&clientReq, sizeof(Message));
                    memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

                } 
                else if (act == 2) {
                    message.action = SHOW_HISTORY;

                    zmq_msg_init_size(&clientReq, sizeof(Message));
                    memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

                }
                else if (act == 0) {
                    message.action = CLIENT_OFF;
                    zmq_msg_init_size(&clientReq, sizeof(Message));
                    memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

                    printf("Sending...\n");
                    zmq_msg_send(&clientReq, servers[serv], 0);
                    zmq_msg_close(&clientReq);

                    break;
                }
                else if (act == -1){
                    break;
                }
                else {
                    printf("Try more...\n");
                    continue;
                }

                printf("Answer:\n");
                zmq_msg_init_size(&clientReq, sizeof(Message));
                memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));
                zmq_msg_send(&clientReq, servers[serv], 0);
                zmq_msg_close(&clientReq);

                zmq_msg_init(&reply);
                zmq_msg_recv(&reply, servers[serv], 0);
                strcpy(ans, (char *) zmq_msg_data(&reply));

                if (strcmp(ans, "ERROR") == 0) {
                    printf("Error occured\n");
                }
                else {
                    printf("%s\n", ans);
                }
                zmq_msg_close(&reply);
            }
        }
        else if (state == 0){
            break;
        }
    }
    for(int i = 0; i<k; i++){
    zmq_close(servers[i]);
    zmq_ctx_destroy(contexts[i]);
    }
    return 0;
    }
