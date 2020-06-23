#include "data.h"
#include <zmq.h>
#include <map>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <signal.h>
#include <fstream>


char ans_publ[256];
int END = 0;
int NODE = 1;


int main(int argc, char *argv[]) {

    int countOfClients = 0;
    std::vector<std::string> history;
    std::vector<std::pair<std::vector<std::string>, std::string> > debtors; // users, message
    std::map<std::string, pid_t> tree; // map with clients <name, pid
    // if pid == 0, client is offline
    Message *message;
    zmq_msg_t reply;
    zmq_msg_t request;
    char ans[256];


    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    ans[0] = '\0';
    strcat(ans, "tcp://*:");
    char *port;
    if (argc == 2) {
        port = argv[1];

    } else {
        port = (char *) "4040";
    }
    strcat(ans, port);

    std::ifstream in("clients.txt", std::ios::in);
    if (in.is_open()) {
        while (true) {

            int ctmp;
            in >> ctmp;

            if (ctmp == END) {
                break;
            }


            std::string str;
            size_t size;
            in >> size;
            in >> str;

            pid_t status;

            in >> status;
            tree[str] = status;

        }
    }

   

    
    int rc = zmq_bind(responder, ans);
    if (rc != 0) {
        perror("zmq_bind");
        zmq_close(responder);
        zmq_ctx_destroy(context);
        exit(1);
    }


    printf("Server initialized...\n");
    for (;;) {
        zmq_msg_init(&request);
        zmq_msg_recv(&request, responder, 0);
        message = (Message *) zmq_msg_data(&request);
        printf("Recieved message from %s action: %d \n", message->clientName, message->action);
        ans[0] = '\0';
        std::string clientName(message->clientName);

        std::map<std::string, pid_t>::iterator it;
        it = tree.find(clientName);
        if (it == tree.end()) {
            //register new id
            printf("Client %s added successfully\n", message->clientName);
            tree[clientName] = message->client_proc_id;
        } else if (it->second != message->client_proc_id) {
            it->second = message->client_proc_id;
            for (auto &i : debtors) {
                for (auto &j : i.first) {
                    if (clientName == j) {


                        strcat(ans, i.second.c_str());
                        strcat(ans, "\n");

                        std::vector<std::string>::iterator it_vec;
                        it_vec = find(i.first.begin(), i.first.end(), j);
                        i.first.erase(it_vec);
                        break;
                    }
                }
            }
        }
        if (INIT == message->action) {
            it = tree.find(message->clientName);
            if (it != tree.end()) {
                strcat(ans, "OK");
                tree[message->clientName] = message->client_proc_id;
                countOfClients++;
            } else
                sprintf(ans, "ERROR");
        } else if (CLIENT_OFF == message->action) {
            countOfClients--;
            tree[message->clientName] = 0;
            if (countOfClients <= 0) {
                printf("all clients are offline, server was down\n");
                break;
            }
        } else if (SEND_MESSAGE == message->action) {

            ans_publ[0] = '\0';

            strcat(ans_publ, message->clientName);
            strcat(ans_publ, ": ");
            strcat(ans_publ, message->text);

            std::string _clientName(message->clientName);
            std::string mess(ans_publ);

            history.push_back(mess);

            std::vector<std::string> users;
            bool clientExist;
            for (int j = 0; j < message->sizeOfNames; ++j) {
                std::string client(message->names[j].Client);
                //message->names[i]; // проход по получателям
                clientExist = false;
                for (auto &i : tree) { // проход по пользователям
                    if (i.first == client) { // нашли пользователя
                        clientExist = true;

                        if (i.second == 0) {//оффлайн
                            
                            users.push_back(i.first);
                        } else if (i.second != message->client_proc_id) {
                            
                            kill(i.second, SIGUSR1);
                        }
                    }
                }
                if (!clientExist) { //клиент не существует
                    strcat(ans, "Client ");
                    strcat(ans, client.c_str());
                    strcat(ans, " doesn't exists\n");
                }

                strcat(ans, "OK\n");
            }

            if (!users.empty()) {
                std::string _message(ans_publ);
                std::pair<std::vector<std::string>, std::string> debtor = make_pair(users, _message);
                debtors.push_back(debtor);
            }


        } else if (SHOW_HISTORY == message->action) {
            ans[0] = '\0';
            for (auto &i : history) {

                strcat(ans, i.c_str());
                strcat(ans, "\n");
                std::cout << i << std::endl;

            }

        } else if (SEND_TO_SUBS == message->action) {

            strcat(ans, ans_publ);


        } else {
            sprintf(ans, "ERROR: Wrong request");
        }

        printf("Send answer to client: [%s]\n", ans);
        zmq_msg_close(&request);
        zmq_msg_init_size(&reply, strlen(ans) + 1);
        memcpy(zmq_msg_data(&reply), ans, strlen(ans) + 1);
        zmq_msg_send(&reply, responder, 0);
        zmq_msg_close(&reply);
    }


    //tree
    std::ofstream out("clients.txt", std::ios::out);
    for (auto &kv : tree) {
        out << NODE << " ";
        size_t size = kv.first.size();
        out << size << " ";
        out.write(&kv.first[0], size);
        out << " ";
        out << kv.second;
        out << "\n";
    }
    out << END;
    out.flush();
    out.close();

  
    std::ofstream out_history("history.txt", std::ios::out);
    for (auto &v : history) {
        out_history << NODE << " ";
        out_history << v << "\n";
    }
    out_history << END;
    out_history.flush();
    out_history.close();

    
    std::ofstream out_debtors("debtors.txt", std::ios::out);
    for (auto &kv : debtors) {
        out_debtors << NODE << " ";
        out_debtors << kv.second << "\n";
        for (auto &user : kv.first) {
            out_debtors << user << " ";
        }
        out_debtors << "-1";
        out_debtors << "\n";
    }
    out_debtors << END;
    out_debtors.flush();
    out_debtors.close();

    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}