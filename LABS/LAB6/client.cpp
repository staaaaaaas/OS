#include <string.h>
#include <map>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <zmq.h>
#include <iostream>

int main () {
    void* context = zmq_ctx_new();
    printf("Client Starting….\n");

    void* request = zmq_socket(context, ZMQ_REQ);
    zmq_connect(request, "tcp://localhost:4040");

    char r [32];
    r[31] = '\0';
    r[0] = 'r';
  
    while(true) {
        printf("Enter your name: ");
        std::string name;
        
        std::cin >> name;
        name = name + "/0";

        for (int i = 1; i < 11; ++i)
            r[i] = name[i - 1];
        
        zmq_msg_t req;
        zmq_msg_init_size(&req, strlen(r));
        memcpy(zmq_msg_data(&req), r, strlen(r));
        zmq_msg_send(&req, request, 0);
        zmq_msg_close(&req);

        char ans [13];
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, request, 0);
        memcpy(ans, zmq_msg_data(&reply), 13);
        zmq_msg_close(&reply);
        if (ans[1] == 'r') {
            if (ans[0] == '1') {
//                printf("You've been registrated\n");
                break;
            } else {
                printf("Bad name\n");
            }
        }
        //printf("You've been registrated\n");
    }
    
    while(true) {
        char mod;
        scanf("%c", &mod);
        while (mod == '\n' || mod == ' ' || mod == '\t')
            scanf("%c", &mod);
        r[0] = mod;
        
        char s[10];
        if (mod == 'e') {
        	zmq_msg_t req;
        	zmq_msg_init_size(&req, 32);
        	memcpy(zmq_msg_data(&req), r, 32);
        	zmq_msg_send(&req, request, 0);
        	zmq_msg_close(&req);
            break;
        } else if (mod == 'a' || mod == 'g') {
            scanf("%s", s);
            if (strlen(s) < 10) s[strlen(s)] = '\0';
            for (int i = 11; i < 21; ++i)
                r[i] = s[i - 11]; 
        } else if (mod == 't') {
            scanf("%s", s);
            if (strlen(s) < 10) s[strlen(s)] = '\0';
            for (int i = 11; i < 21; ++i)
                r[i] = s[i - 11];
            scanf("%s", s);
            if (strlen(s) < 10) s[strlen(s)] = '\0';
            for (int i = 21; i < 31; ++i)
                r[i] = s[i - 21]; 
        } else if (mod != 'v'){
            continue;
        }
        
        zmq_msg_t req;
        zmq_msg_init_size(&req, 32);
        memcpy(zmq_msg_data(&req), r, 32);
        zmq_msg_send(&req, request, 0);
        zmq_msg_close(&req);

        char ans [13];
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, request, 0);
        memcpy(ans, zmq_msg_data(&reply), 13);
        zmq_msg_close(&reply);

        
        if (ans[1] == 'v') {
            int sum = 0;
            for (int i = 2; i < 13; ++i)
                sum = sum * 10 + ans[i] - '0';
            printf("Your cash: %d\n", sum);
        }  else if (ans[1] == 'a') {
            printf("Add successful\n");
        } else if (ans[1] == 'g') {
            if (ans[0] == '1') printf("Get successful\n");
            else printf("Not enough money\n");
        } else {
            if (ans[0] == '1') {
                printf("Transfer successful\n");
            } else if (ans[12] == '1') {
                printf("Not enough money\n");
            } else {
                printf("No such client\n");
            }
        }
    }
    
    zmq_close(request);
    zmq_ctx_destroy(context);

    return 0;
}
