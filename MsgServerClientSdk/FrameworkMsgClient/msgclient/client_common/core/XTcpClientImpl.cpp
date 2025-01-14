#include "core/XTcpClientImpl.h"

#include "webrtc/base/common.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/nethelpers.h"
#include "webrtc/base/stringutils.h"

#ifdef WIN32
#include "webrtc/base/win32socketserver.h"
#endif

#include <sys/time.h>

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XTcpClientImpl"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

const int kDefaultServerPort = 80;
const int kReconnectDelay = 2000;
const int kTryReconnectDelay = 3000;
const int kNotifyDelay = 1;
const int kBufferSizeInBytes = 2048;

rtc::AsyncSocket* CreateClientSocket(int family) {
#ifdef WIN32
	rtc::Win32Socket* sock = new rtc::Win32Socket();
	sock->CreateT(family, SOCK_STREAM);
	return sock;
#elif defined(POSIX)
	rtc::Thread* thread = rtc::Thread::Current();
	ASSERT(thread != NULL);
	return thread->socketserver()->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
}

XTcpClient* XTcpClient::Create(XTcpClientCallback&rCallback)
{
	return new XTcpClientImpl(rCallback);
}

XTcpClientImpl::XTcpClientImpl(XTcpClientCallback &rCallback)
: m_rCallback(rCallback)
, m_asynResolver(NULL)
, m_nState(NOT_CONNECTED)
, m_bAutoConnect(false)
, m_nBufLen(0)
, m_nBufOffset(0)
{
	XTcpTick::Open();
	m_nBufLen = kBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}


XTcpClientImpl::~XTcpClientImpl()
{
	XTcpTick::Close();
	delete[] m_pBuffer;
}

void XTcpClientImpl::Connect(const std::string& server, int port, bool bAutoConnect)
{
	ASSERT(!server.empty());

	m_bAutoConnect = bAutoConnect;
	if (m_nState != NOT_CONNECTED) {
		m_rCallback.OnServerConnectionFailure();
		return;
	}

	if (server.empty()) {
		m_rCallback.OnServerConnectionFailure();
		return;
	}

	if (port <= 0)
		port = kDefaultServerPort;

	m_svrSockAddr.SetIP(server);
	m_svrSockAddr.SetPort(port);

	if (m_svrSockAddr.IsUnresolvedIP()) {
		CurThread()->PostDelayed(RTC_FROM_HERE, 1, this, 1);
	}
	else {
		CurThread()->PostDelayed(RTC_FROM_HERE, 1, this, 0);
	}
}

void XTcpClientImpl::Disconnect()
{
	m_bAutoConnect = false;
	Close();
}

int XTcpClientImpl::SendMessageX(std::string&strMsg)
{
	return SendMessageX(strMsg.c_str(), (int)strMsg.length());
}

int XTcpClientImpl::SendMessageX(const char*pMsg, int nLen)
{
	if (pMsg == NULL || nLen == 0)
		return -1;

	rtc::CritScope l(&m_csBuf);
	while ((m_nBufOffset + nLen) > m_nBufLen)
	{
		int newLen = m_nBufLen + kBufferSizeInBytes;
		if (nLen > newLen)
			newLen = m_nBufLen + nLen;
		char* temp = new char[newLen];
		if (temp == NULL)
			continue;
		memcpy(temp, m_pBuffer, m_nBufLen);
		delete[] m_pBuffer;
		m_pBuffer = temp;
		m_nBufLen = newLen;
	}
	memcpy(m_pBuffer + m_nBufOffset, pMsg, nLen);
	m_nBufOffset += nLen;

	return nLen;
}

void XTcpClientImpl::OnTick()
{
	rtc::CritScope l(&m_csBuf);
	if (m_nBufOffset > 0 && m_nState == CONNECTED)
	{
		size_t sent = m_asynSock->Send(m_pBuffer, m_nBufOffset);

		if (sent > 0)
		{
			m_nBufOffset -= sent;
			if (m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer + sent, m_pBuffer, m_nBufOffset);
			}

			m_rCallback.OnMessageSent((int)sent);
		}
	}

	m_rCallback.OnTick();
}

void XTcpClientImpl::OnFinish(void)
{
	Close();
}

void XTcpClientImpl::OnMessage(rtc::Message* msg)
{
	if (msg->message_id == 0)
	{
		DoConnect();
	}
	else if (msg->message_id == 1)
	{
		DoResolver();
	}
    else if(msg->message_id == 1001)
    { //1001 try to reconnect
        if(Status()!=CONNECTED) {
#ifdef WEBRTC_ANDROID
            LOGI("XTcpClientImpl::OnMessage receive 1001, reconnect after 2s...\n");
#else
            LOG(WARNING) << "XTcpClientImpl::OnMessage receive 1001, reconnect after 2s...";
#endif
            rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kReconnectDelay, this, 0);
        }
    }
    else if (msg->message_id == 1002)
    {
        m_rCallback.OnClientSyncSeqn();
    }
    else if (msg->message_id == 1003)
    {
        if (msg->pdata) {
            RelayedMsgData* pd = static_cast< RelayedMsgData* >(msg->pdata);
            if (pd) {
                m_rCallback.OnClientSyncGroupSeqn(pd->data());
            }
            delete pd;
            pd = nullptr;
        } else {
            LOG(LERROR) << "XTcpClientImpl::OnMessage OnClientSyncGroupSeqn msg->pdata is null";
        }
    }
}

void XTcpClientImpl::DoResolver()
{
	m_nState = RESOLVING;
	if (!m_asynResolver)
	{
		m_asynResolver = new rtc::AsyncResolver();
		m_asynResolver->SignalDone.connect(this, &XTcpClientImpl::OnResolveResult);
		m_asynResolver->Start(m_svrSockAddr);
    } else {
    }
}

void XTcpClientImpl::DoConnect()
{
	m_nState = CONNECTTING;
	m_asynSock.reset(CreateClientSocket(m_svrSockAddr.ipaddr().family()));
    // 1024*1024*16 // 16M
    m_asynSock->SetOption(rtc::Socket::Option::OPT_RCVBUF, 1024*1024*16);
	InitSocketSignals();

	bool ret = ConnectControlSocket();
	if (!ret) {
#ifdef WEBRTC_ANDROID
        LOGI("XTcpClientImpl::DoConnect false, try to reconnect after 3s...\n");
#else
        LOG(WARNING) << "XTcpClientImpl::DoConnect false, try to reconnect after 3s...";
#endif
		m_rCallback.OnServerConnectionFailure();
        CurThread()->PostDelayed(RTC_FROM_HERE, kTryReconnectDelay, this, 1001);
    }
}
void XTcpClientImpl::Close()
{
    if (m_asynSock.get() != NULL) {
		m_asynSock->Close();
        m_asynSock.reset(nullptr);
    }
	if (m_asynResolver != NULL) {
		m_asynResolver->Destroy(false);
		m_asynResolver = NULL;
	}
	m_nState = NOT_CONNECTED;
}
void XTcpClientImpl::InitSocketSignals()
{
	ASSERT(m_asynSock.get() != NULL);
	m_asynSock->SignalCloseEvent.connect(this,
		&XTcpClientImpl::OnClose);
	m_asynSock->SignalConnectEvent.connect(this,
		&XTcpClientImpl::OnConnect);
	m_asynSock->SignalReadEvent.connect(this,
		&XTcpClientImpl::OnRead);
}
bool XTcpClientImpl::ConnectControlSocket()
{
	ASSERT(m_asynSock->GetState() == rtc::Socket::CS_CLOSED);
	int err = m_asynSock->Connect(m_svrSockAddr);
	if (err == SOCKET_ERROR) {
		Close();
		return false;
	}
	return true;
}

void XTcpClientImpl::NotifySyncSeqn()
{

    rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kNotifyDelay, this, 1002);
}

void XTcpClientImpl::NotifySyncGroupSeqn(const std::string& storeid)
{
    RelayedMsgData *pd = new RelayedMsgData(storeid);
    rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kNotifyDelay, this, 1003, pd);
}



void XTcpClientImpl::OnConnect(rtc::AsyncSocket* socket)
{
	m_nState = CONNECTED;
	m_rCallback.OnServerConnected();
}

void XTcpClientImpl::OnRead(rtc::AsyncSocket* socket)
{
	char buffer[1024*512];
    int64_t timestamp = 0;
    int i=0;

	do {
		int bytes = socket->Recv(buffer, sizeof(buffer), &timestamp);
		if (bytes <= 0)
			break;
		m_rCallback.OnMessageRecv(buffer, bytes);
	//} while (true);
	} while (i++ < 30);
}

void XTcpClientImpl::OnClose(rtc::AsyncSocket* socket, int err)
{
	socket->Close();

#ifdef WIN32
	if (err != WSAECONNREFUSED) {
#else
	if (err != ECONNREFUSED) {
#endif
	}

	if (socket == m_asynSock.get()) {
		{
			rtc::CritScope l(&m_csBuf);
			if (m_nBufOffset > 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
				m_nBufOffset = 0;
				m_rCallback.OnMessageSent(-1);
			}
		}

		if (m_nState == CONNECTED)
			m_rCallback.OnServerDisconnect();
		else
			m_rCallback.OnServerConnectionFailure();

		if (m_bAutoConnect)
		{// Auto Connect...
#ifdef WEBRTC_ANDROID
            LOGI("XTcpClientImpl::OnClose  Connection refused; retrying in 2 seconds\n");
#else
			LOG(WARNING) << "XTcpClientImpl::OnClose Connection refused; retrying in 2 seconds";
#endif
			rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kReconnectDelay, this, 0);
		}
		else
		{
			Close();
		}
	}
}

void XTcpClientImpl::OnResolveResult(rtc::AsyncResolverInterface* resolver)
{
#ifdef WEBRTC_ANDROID
    LOGI("XTcpClientImpl::OnResolverResult was called m_asynResolver->GetError:%d\n", m_asynResolver->GetError());
#else
    LOG(WARNING) << "XTcpClientImpl::OnResolverResult was called m_asynResolver->GetError:" << m_asynResolver->GetError();
#endif
    if (m_asynResolver->GetError() != 0) {
        m_rCallback.OnServerConnectionFailure();
        m_nState = NOT_CONNECTED;
    }
    else {
        if(m_asynResolver->GetResolvedAddress(AF_INET6, &m_svrSockAddr)
           || m_asynResolver->GetResolvedAddress(AF_INET, &m_svrSockAddr)) {
            DoConnect();
        } else {
            m_rCallback.OnServerConnectionFailure();
            m_nState = NOT_CONNECTED;
        }
    }
    m_asynResolver->Destroy(false);
    m_asynResolver = NULL;
}
