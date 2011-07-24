#ifdef WIN32
#include <winsock2.h>
#define socklen_t	int
#endif
#include "IServer.h"
#include "Cipher.h"
#include "KSG_EncodeDecode.h"

#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_SIGNAL	16
#define	SIG_SENDDATA	SIGRTMIN + MAX_SIGNAL + 1

static unsigned gs_holdrand = time(NULL);

static inline unsigned _Rand() {
    gs_holdrand = gs_holdrand * 214013L + 2531011L;
    return gs_holdrand;
}

static inline void RandMemSet(int nSize, unsigned char *pbyBuffer) {
    while (nSize--) {
        *pbyBuffer++ = (unsigned char)_Rand();
    }
}

int do_sendData(IServer* server, client_info* client);
void JustDoIt(IServer* pIServer, client_info* client)
{
	if (client->recv_flag)
	{
		client->recv_flag = false;
		pIServer->receiveData(client);
	}
	if (client->state == STATE_SENDING)
	{
		do_sendData(pIServer, client);
	}
}

void DoEveryThing(IServer* pIServer)
{
	for (int i = 0; i < pIServer->max_client; i++)
	{
		client_info& client = pIServer->clients[i];
		if (client.sock != INVALID_SOCKET)
			JustDoIt(pIServer, &client);
	}
}

int ZListenThread::action() {
	int index;
	while(true) {
//响应请求-----------------
		struct sockaddr client_addr;
		socklen_t length = sizeof(client_addr);
		SOCKET s = accept(parent->listen_socket, &client_addr, &length);
		if(s == INVALID_SOCKET) continue;
//分配一个连接的资源
		index = parent->getConnection(s);
		if(index== -1) {
			printf("server max connection reached, client attempt fail.\n");
			closesocket(s);
		}
		else {
//平台相关,创建完成端口或者事件
#ifdef WIN32
			CreateIoCompletionPort((HANDLE)s, parent->CompletionPort, (DWORD)&parent->clients[index], 0);
#else
			fcntl(s, F_SETSIG, SIGRTMIN);
			fcntl(s, F_SETOWN, parent->pid);
			int flags = fcntl(s, F_GETFL);
			flags |= O_NONBLOCK | O_ASYNC;
			fcntl(s, F_SETFL, flags);
			fcntl(s, F_SETAUXFL, O_ONESIGFD);
#endif
//发送key值到客户端
			client_info* client = &parent->clients[index];
			//client->mutex.lock();
			memcpy(&client->addr, &client_addr, length);
			client->server_key = _Rand();
			client->client_key = _Rand();

#pragma pack(1)
			struct {
				unsigned short uSize;
				ACCOUNT_BEGIN ab;
			}account_begin;
#pragma pack()

            RandMemSet(sizeof(account_begin), (unsigned char *)&account_begin);
			account_begin.uSize = sizeof(ACCOUNT_BEGIN);
			account_begin.ab.ProtocolType = CIPHER_PROTOCOL_TYPE;
			account_begin.ab.Mode         = 0;
			account_begin.ab.ServerKey    = ~client->server_key;
			account_begin.ab.ClientKey    = ~client->client_key;
			if (sizeof(account_begin) != send(client->sock, &account_begin, sizeof(account_begin), 0))
			{
				closesocket(client->sock);
				client->state = STATE_IDLE;
				printf("client [%d] %d create failed.\n", index, client->sock);
				client->sock = INVALID_SOCKET;
			}
			else
			{
#ifdef WIN32
				client->recv_flag = true;
#endif
				client->conn_flag = true;
				printf("client [%d] %d created. (%u, %u)\n", index, client->sock, client->server_key, client->client_key);
			}
			//client->mutex.unlock();
		}
	}
	return 0;
}


#ifndef WIN32
// flying add this functino
// Data shoule be sent in a thread, so that the main-process will not be blocked.
int do_sendData(IServer* server, client_info* client) {
	if (client->state == STATE_CLOSING)
		return -1;
	int data_length;
	char *data_buffer = client->buffer->getSendData(data_length);
	if(!data_buffer) {
		client->state = STATE_IDLE;
		return -1;
	}

	int nRetVal = send(client->sock, data_buffer, data_length, 0);
	if(nRetVal != -1) {
		client->buffer->sendData(nRetVal);
		if(nRetVal != data_length) {				//发送的数据不够再次发送
			server->sendData(client);
		}
		else client->state = STATE_IDLE;
		return 1;
	}
	else {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			client->state = STATE_BLOCKED;
		}
		else {
			printf("sending failed %d....close connection [%d].\n", errno, client->index);
			client->state = STATE_CLOSING;
			return 0;
		}
	}
	return 1;
}
#endif //WIN32

//实际的工作线程,平台相关
#ifdef WIN32
DWORD WINAPI sig_thread(LPVOID id) {
#else
void *sig_thread(LPVOID id) {
#endif
	IServer *parent = (IServer *)id;
#ifdef WIN32
	DWORD transferred;
	LPOVERLAPPED overlapped;
	DWORD flags;
	DWORD dwIoSize;
	client_info *client;
	while(true) {
		bool bIORet = GetQueuedCompletionStatus(parent->CompletionPort, &dwIoSize, (LPDWORD) &client, &overlapped, INFINITE);
		DWORD dwIOError = GetLastError();
		if(!bIORet && dwIOError != WAIT_TIMEOUT ) {
			closesocket(client->sock);
			client->buffer->clear();
			client->sock = INVALID_SOCKET;
			if(parent->call_back)
				parent->call_back(parent->parameter, client->index, enumClientConnectClose);		//notify
			continue;
		}
		if(overlapped == &client->write_overlapped) {
			if(dwIoSize > 0) {
				client->buffer->sendData(dwIoSize);
			}
			parent->sendData(client);
		}
		else {
			if(dwIoSize > 0) {
				client->buffer->receiveData(dwIoSize);
			}
			parent->receiveData(client);
		}
	}
#else
  siginfo_t siginfo;
  sigset_t sset;
  parent->pid = getpid();
  printf("signal thread start\n");

	sigemptyset(&sset);
	sigaddset(&sset, SIGIO);
	sigaddset(&sset, SIGRTMIN);
	pthread_sigmask(SIG_BLOCK, &sset, NULL);

  while(true) {
    int signum = sigwaitinfo(&sset, &siginfo);
    if(signum == -1)
	{
		printf("fatal signum in signal thread ...errno = %d\n", errno);
		continue;
	}
    if(signum == SIGIO) {
	  //client_info* client = (client_info*)siginfo.si_value.sival_ptr;
      printf("overflow .... \n");
	  exit(-2);
    }
	/*if (signum == SIG_SENDDATA) {
		client_info* client = (client_info*)siginfo.si_value.sival_ptr;
		if(client->state == STATE_SENDING) {
			do_sendData(parent, client);
		}
	}
    else*/ if(signum == SIGRTMIN) {
		client_info *client;
		int index = 0;
		while(index < parent->max_client) {
			client = &parent->clients[index];
			if(client->sock == siginfo.si_fd) {
				//client->mutex.lock();
				if (client->state != STATE_CLOSING)
				{
				if (siginfo.si_band & 0x01) {			//有等待读入的数据
					client->recv_flag = true;
					//parent->receiveData(client);
				}
				if (siginfo.si_band & 0x04) {
					if(client->state == STATE_BLOCKED) {
						client->state = STATE_IDLE;//STATE_SENDING;
						//parent->sendData(client);
					}
				}
				else if (!(siginfo.si_band & 0x01))
				{
					printf("signal notify close connection %d\n", client->index);
					client->state = STATE_CLOSING;
				}
				}
				//client->mutex.unlock();
				break;
			}
			index++;
		}
    }
  }
	sigprocmask(SIG_UNBLOCK, &sset, NULL);
#endif
}

void IServer::closeConnection(client_info *client) {
	if(client->sock == INVALID_SOCKET) return;
	closesocket(client->sock);
	client->buffer->clear();
	client->state = STATE_IDLE;
	client->conn_flag = false;
	client->recv_flag = false;
	client->sock = INVALID_SOCKET;
}

bool IServer::sendData(client_info *client) {
	//int nRetryTime = 0;
//平台相关代码,实际发出重叠的IO请求,发送数据---------------------------------------
	int size;
#ifdef WIN32
	client->m_wsaOutBuffer.buf = client->buffer->getSendData((int &)client->m_wsaOutBuffer.len);
	if(client->m_wsaOutBuffer.buf) {
		ULONG ulFlags = MSG_PARTIAL;
		ZeroMemory(&client->write_overlapped, sizeof(OVERLAPPED));
		int nRetVal = WSASend(client->sock, &client->m_wsaOutBuffer, 1, &client->m_wsaOutBuffer.len, 
			ulFlags, &client->write_overlapped, NULL);
		if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING ) {		//出现错误
			closeConnection(client);
		}
		else return true;
	}
#else
	if(client->state == STATE_IDLE) {
/*		union sigval info;
		info.sival_ptr = (void*)client;
		int nRet = 0;*/
		client->state = STATE_SENDING;
		/*if (0 != (nRet = sigqueue(pids[client->index % max_thread], SIG_SENDDATA, info))) {
			printf("sigqueue failed...%d\n", nRet);
		}*/
	}
#endif
	return false;
}

bool IServer::receiveData(client_info *client) {
//平台相关代码,实际发出重叠的IO请求,接收数据---------------------------------------
#ifdef WIN32
	ULONG ulFlags = MSG_PARTIAL;
	DWORD dwIoSize;
	int bufsize = 0;
	client->m_wsaInBuffer.buf = (char *)client->buffer->getReceiveBuffer(bufsize);
	client->m_wsaInBuffer.len = bufsize;
	if (!client->m_wsaInBuffer.buf)
	{
		printf("receive buffer full...close connection [%d].\n", client->index);
		closeConnection(client);
		return false;
	}
	UINT nRetVal = WSARecv(client->sock, &client->m_wsaInBuffer, 1, &dwIoSize, &ulFlags, &client->read_overlapped, NULL);
	if(nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
	    printf("receive data pending? close connection [%d].\n", client->index);
		closeConnection(client);
		return false;
	}
#else
  int bufsize = 0;
  if (client->state == STATE_CLOSING)
	  return false;
  char* recv_buffer = client->buffer->getReceiveBuffer(bufsize);
	if (!recv_buffer)
	{
		printf("receive buffer full...close connection [%d].\n", client->index);
		client->state = STATE_CLOSING;
		return false;
	}
  int ret = recv(client->sock, recv_buffer, bufsize, 0);
  if(ret <= 0) {
	if (errno != EAGAIN)
	{
	    printf("receive data pending? close connection [%d].\n", client->index);
		client->state = STATE_CLOSING;
	}
    return false;
  }
  client->buffer->receiveData(ret);
#endif
  return true;
}

IServer::IServer(int number, int thread_number, int max_send, int max_receive) {
	clients = new client_info[number];
	max_client = number;
	int index;
	for(index = 0; index < max_client; index++) {
		clients[index].buffer = new ZBuffer(max_send, max_receive);
		clients[index].buffer->startPerf(&recvPerf, &sendPerf);
		clients[index].sock = INVALID_SOCKET;
		clients[index].index = index;
	}
	call_back = NULL;
	max_thread = thread_number;
#ifndef WIN32
	//current_thread = 0;
	//pids = new int[max_thread];
	//sigsets = new sigset_t[max_thread];
	//send_threads = new ZSendThread[max_thread];
	//recv_threads = new ZRecvThread[max_thread];
	create_callback_thread = new ZCreateCallBackThread(this);
	close_callback_thread = new ZCloseCallBackThread(this);
	for (index = 0; index < max_thread; index++)
	{
		//send_threads[index].parent = this;
		//send_threads[index].index = index;

		//recv_threads[index].parent = this;
		//recv_threads[index].index = index;
	}
#endif //WIN32
	listen_thread = new ZListenThread(this);
}

IServer::~IServer() {
	printf("destruct IServer....\n");
	listen_thread->stop();
	int index;
	for (index = 0; index < max_thread; index++)
	{
		//send_threads[index].stop();
		//recv_threads[index].stop();
	}
	create_callback_thread->stop();
	close_callback_thread->stop();
#ifndef WIN32
	//delete[] pids;
	//delete[] sigsets;
	//delete send_threads;
	//delete recv_threads;
	delete create_callback_thread;
	delete close_callback_thread;
#endif //WIN32
	delete listen_thread;
	delete[] clients;
}

int IServer::getConnection(SOCKET sock) {
	int index;
	for(index = 0; index < max_client; index++) {
		//clients[index].mutex.lock();
		if(clients[index].sock == INVALID_SOCKET) {
			clients[index].sock = sock;
			//clients[index].mutex.unlock();
			return index;
		}
		//clients[index].mutex.unlock();
	}
	return -1;
}

int init_socket = 0;
int IServer::Startup() {
	if(!init_socket) {
#ifdef WIN32
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1, 1), &wsa_data);
/*#else
	int index;
	int signal_number;
	for(index = 0; index < max_thread; index++) {
		sigemptyset(&sigsets[index]);
		sigaddset(&sigsets[index], SIGIO);
		//sigaddset(&sigsets[index], SIG_SENDDATA);
		signal_number = SIGRTMIN + index;
		while(signal_number < SIGRTMIN + MAX_SIGNAL) {
			sigaddset(&sigsets[index], signal_number);
			signal_number += max_thread;
		}
		pthread_sigmask(SIG_BLOCK, &sigsets[index], NULL);
	}*/
#endif
		init_socket++;
	}
	return 0;
}

int IServer::Cleanup() {
	if(init_socket--) {
#ifdef WIN32
		WSACleanup();
#endif
	}
	return 0;
}

int IServer::Release() {
	return 0;
}

int IServer::Open(const unsigned long &ulnAddressToListenOn, const unsigned short &usnPortToListenOn) {
//在此创建套接字,绑定,启动侦听线程
#ifdef WIN32
	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	int index;
	DWORD thread_id;
	for(index = 0; index < max_thread; index++) {
		HANDLE hThread;
		hThread = CreateThread(NULL, 0, sig_thread, this, 0, &thread_id);
		CloseHandle(hThread);
	}
	listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    pthread_t work;
	int index;
	//for(index = 0; index < max_thread; index++)
	{
		int ret = pthread_create(&work, NULL, sig_thread, this);
		if (ret == 0)
			pthread_detach(work);
		else
			return 0;
	}
#endif
	if(listen_socket == INVALID_SOCKET) return 0;
	struct sockaddr_in addr;
	
	memset((void*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(usnPortToListenOn);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(listen_socket, (const sockaddr *)&addr, sizeof(addr))) {
		return 0;
	}
	if(listen(listen_socket, 5)) return 0;
	listen_thread->start();
	create_callback_thread->start();
	close_callback_thread->start();
#ifndef WIN32
	for (index = 0; index < max_thread; index++)
	{
		//send_threads[index].start();
		//recv_threads[index].start();
	}
#endif
	printf("GameServer on Port %d started....\n", usnPortToListenOn);
	return 1;
}

int IServer::CloseService() {
	closesocket(listen_socket);

	#ifdef _COND_SEND_
	pthread_cond_destroy(&cond_send_data);
	#endif

	return 0;
}

int IServer::RegisterMsgFilter(void * lpParam, CALLBACK_SERVER_EVENT pfnEventNotify) {
	call_back = pfnEventNotify;
	parameter = lpParam;
	return 0;
}

int IServer::PreparePackSink() {		//准备开始数据封装
	return 0;
}

int IServer::PackDataToClient(const unsigned long &ulnClientID, const void * pData,	unsigned long datalength) {
	if (ulnClientID < 0 || ulnClientID >= max_client)
		return -1;

	if(clients[ulnClientID].sock != INVALID_SOCKET) {
		if (clients[ulnClientID].state != STATE_CLOSING &&
			!clients[ulnClientID].buffer->packData((const char *)pData, datalength))
		{
			printf("sending buffer exceed....close connection [%d].\n", clients[ulnClientID].index);
			clients[ulnClientID].state = STATE_CLOSING;
			return -1;
		}
	}
	JustDoIt(this, &clients[ulnClientID]);
	return 0;
}

int IServer::SendPackToClient() {
	int index;
	int nTotalLength = 0;
	for(index = 0; index < max_client; index++) {
		client_info& client = clients[index];
		//clients[index].mutex.lock();
		if (client.sock != INVALID_SOCKET) {
			JustDoIt(this, &client);
			int length = 0;
			char *data = client.buffer->completePacket(length);
			if(data && client.server_key) {
				KSG_EncodeBuf(length, (unsigned char *)data, (unsigned int *)&client.server_key);
				client.buffer->sendPacket();
				//printf("_<%08X>_", data);
				{
					//if (client.sock != INVALID_SOCKET && client.state == STATE_SENDING)
					{
						//client.mutex.lock();
						sendData(&client);
						//client.mutex.unlock();
					}
				}
			}
			nTotalLength += length;
		}
		JustDoIt(this, &client);
		//clients[index].mutex.unlock();
	}
	return nTotalLength;
}

int IServer::SendPackToClient(int index) {
	if (index < 0 || index >= max_client)
	{
		return SendPackToClient();
	}
	//clients[index].mutex.lock();
	client_info& client = clients[index];
	if(client.sock != INVALID_SOCKET) {
		int length;
		char *data = client.buffer->completePacket(length);
		if(data) {
			KSG_EncodeBuf(length, (unsigned char *)data, (unsigned int *)&client.server_key);
			client.buffer->sendPacket();
			sendData(&client);
		}
		//clients[index].mutex.unlock();
		JustDoIt(this, &client);
		return length;
	}
	//clients[index].mutex.unlock();
	return 0;
}

int IServer::SendData(const unsigned long &ulnClientID,	const void * pData, unsigned long datalength) {
	PackDataToClient(ulnClientID, pData, datalength);
	SendPackToClient(ulnClientID);
//发送一个消息
	return 0;
}
	
const void *IServer::GetPackFromClient(unsigned long ulnClientID, unsigned int &datalength) {
	if (ulnClientID < 0 || ulnClientID >= max_client)
		return NULL;
	client_info& client = clients[ulnClientID];
	JustDoIt(this, &client);
	if(client.sock != INVALID_SOCKET) {
		//clients[ulnClientID].mutex.lock();
		char *data = (char*)client.buffer->getPacket((int &)datalength);
		if(data) {
			KSG_DecodeBuf(datalength, (unsigned char *)data, (unsigned int *)&client.client_key);
			//client.mutex.unlock();
			return data;
		}
		//clients[ulnClientID].mutex.unlock();
	}
	return NULL; 
}

int IServer::ShutdownClient(const unsigned long &ulnClientID) { 
	if (ulnClientID < 0 || ulnClientID >= max_client)
		return NULL;

	//clients[ulnClientID].mutex.lock();
	if(clients[ulnClientID].sock != INVALID_SOCKET) {
		clients[ulnClientID].state = STATE_CLOSING;
	}
	//clients[ulnClientID].mutex.unlock();
	return 0; 
}

const char * IServer::GetClientInfo(const unsigned long &ulnClientID)
{
	return inet_ntoa(((sockaddr_in*)&(clients[ulnClientID].addr))->sin_addr);
}

#ifndef WIN32
int ZCloseCallBackThread::action()
{
	int i = 0;
	printf("Close CallBack Thread...start\n");
	while (1) {
		usleep(5000);
		for (i = 0; i < parent->max_client; i++)
		{
			client_info& client = parent->clients[i];
			{
				if (client.state == STATE_CLOSING) {
					//client.mutex.lock();
					parent->closeConnection(&client);
					//client.mutex.unlock();
					if(parent->call_back) {
						//printf("close call_back %d\n", i);
						parent->call_back(parent->parameter, i, enumClientConnectClose);
						usleep(200000);
					}
				}
			}
		}
	}
	return 0;
}

int ZCreateCallBackThread::action()
{
	int i = 0;
	printf("Create CallBack Thread...start\n");
	while (1) {
		usleep(5000);
		for (i = 0; i < parent->max_client; i++)
		{
			client_info& client = parent->clients[i];
			{
				if (client.conn_flag)
				{
					//client.mutex.lock();
					client.conn_flag = false;
					//client.mutex.unlock();
					if(parent->call_back) {
						//printf("create call_back %d\n", i);
						parent->call_back(parent->parameter, i, enumClientConnectCreate);		//notify
					}
				}
			}
		}
	}
	return 0;
}

int ZRecvThread::action()
{
	int i = 0;
	printf("receiving thread %d...start\n", index);
	while (1) {
		usleep(5000);
		for (i = index; i < parent->max_client; i += parent->max_thread)
		{
			client_info& client = parent->clients[i];
			{
				if (client.sock != INVALID_SOCKET && client.recv_flag)
				{
					//client.mutex.lock();
					parent->receiveData(&client);
					client.recv_flag = false;
					//client.mutex.unlock();
				}
			}
		}
	}
	printf("receiving thread...end\n");
	return 0;
}

int ZSendThread::action()
{
	int i = 0;
	printf("sending thread %d...start\n", index);
	while (1) {
		usleep(5000);
		for (i = index; i < parent->max_client; i += parent->max_thread)
		{
			client_info& client = parent->clients[i];
			{
				if (client.sock != INVALID_SOCKET && client.state == STATE_SENDING)
				{
					//client.mutex.lock();
					do_sendData(parent, &client);
					//client.mutex.unlock();
				}
			}
		}
	}
	printf("sending thread...end\n");
	return 0;
}
#endif