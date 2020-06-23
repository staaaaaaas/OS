#include <string.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "mlist.h"
#include <zmq.h>

using namespace std;

mList* ClientBase = new mList();
map <string,int> Clients;
bool sl = false;

typedef struct {
    bool suc;
    int sum;
    char com;
} Ret;

Ret Execute(char s [32]) {
    char ch = s[0];
    Ret ans;
    ans.suc = true;
    ans.sum = 0;
    ans.com = ch;
    
    if (ch == 'r') {
        string newname = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            newname += s[i];
        }
        if(Clients.find(newname) == Clients.end() || Clients[newname] != 0){
	        
	        if (Clients.find(newname) == Clients.end())
	    	    ClientBase->Push(newname);
    	    Clients[newname] = 1;
    	}else ans.suc = false;
    } else if (ch == 'a') {
        string name = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            name += s[i];
        }
        int sum = 0;
        for (int i = 11; i <= 20; ++i) {
            if (s[i] == '\0') break;
            sum = sum * 10 + s[i] - '0';
        }
        mList_el* lucky = ClientBase->Find(name);
        if (lucky->credit < 1000){
			if (1000 - lucky->credit > sum){
				lucky->credit += sum;
			}
			else {
				lucky->deposit += sum - 1000 + lucky->credit;
				lucky->credit += sum;
			}
		}else lucky->deposit += sum;
   	 } else if (ch == 'g') {
        string name = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            name += s[i];
        }
        int sum = 0;
        for (int i = 11; i <= 20; ++i) {
            if (s[i] == '\0') break;
            sum = sum * 10 + s[i] - '0';
        }
        mList_el* lucky = ClientBase->Find(name);
        
        if (lucky->deposit >= sum) lucky->deposit -= sum;
        else if(lucky->deposit + lucky->credit >= sum){
        	lucky->credit -= sum - lucky->deposit;
        	lucky->deposit = 0;
        }else ans.suc = false;
        
    } else if (ch == 'v') {
        string name = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            name += s[i];
        }
        mList_el* lucky = ClientBase->Find(name);
        ans.sum = lucky->deposit;
    } else if (ch == 't') {
        string name1 = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            name1 += s[i];
        }
        string name2 = "";
        for (int i = 11; i <= 20; ++i) {
            if (s[i] == '\0') break;
            name2 += s[i];
        }
        int sum = 0;
        for (int i = 21; i <= 30; ++i) {
            if (s[i] == '\0') break;
            sum = sum * 10 + s[i] - '0';
        }
        mList_el* poor = ClientBase->Find(name1);
        mList_el* lucky = ClientBase->Find(name2);
        if (!lucky) {
            ans.suc = false;
        } else if (poor->deposit >= sum) {
            poor->deposit -= sum;
            lucky->deposit += sum;
        } else if (poor->deposit + poor->credit >= sum){
        	poor->credit -= sum - poor->deposit;
        	poor->deposit = 0;
        	lucky->deposit += sum;
        }else {
            ans.sum = 1;
            ans.suc = false;
        }
    } else if (ch == 'e'){
    	string name = "";
        for (int i = 1; i <= 10; ++i) {
            if (s[i] == '\0') break;
            name += s[i];
        }
        Clients[name] = 0;
    }
    
    return ans;
}

int main () {
    void* context = zmq_ctx_new();
    void* respond = zmq_socket(context, ZMQ_REP);

    zmq_bind(respond, "tcp://*:4040");
    printf("Starting…\n");

    while(true) {
        while (sl) sleep(1);
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, respond, 0);
        char s [32];
        memcpy(s, zmq_msg_data(&request), 32);
        zmq_msg_close(&request);
        
        Ret a = Execute(s);
        char ans [13];
        ans[12] = '\0';
        if (a.suc) ans[0] = '1';
        else ans[0] = '0';
        ans[1] = a.com;
        for (int i = 12; i >= 2; --i) {
            ans[i] = '0' + a.sum % 10;
            a.sum /= 10;
        }
        
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, 13);
        memcpy(zmq_msg_data(&reply), ans, 13);
        zmq_msg_send(&reply, respond, 0);
        zmq_msg_close(&reply);
    }
    zmq_close(respond);
    zmq_ctx_destroy(context);

    return 0;
}
