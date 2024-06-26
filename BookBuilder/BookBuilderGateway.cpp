#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <string>
#include <signal.h>
#include <chrono>
#include <ev.h>
#include <liburing.h>
#include <fstream>
#include <sys/socket.h>
#include "../OrderBook/OrderBook.hpp"
#include "../SPSCQueue/SPSCQueue.hpp"
#include "../Utils/Utils.hpp"

#define CPU_CORE_INDEX_FOR_BOOK_BUILDER_GATEWAY_THREAD 1
#define CPU_CORE_INDEX_FOR_SQ_POLL_THREAD 0
#define NUMBER_OF_IO_URING_SQ_ENTRIES 256
#define WEBSOCKET_CLIENT_RX_BUFFER_SIZE 16378

using namespace std::chrono;

#if defined(USE_BITMEX_EXCHANGE) || defined(USE_BITMEX_MOCK_EXCHANGE) || defined(USE_BITMEX_TESTNET_EXCHANGE)
    #define NUMBER_OF_CONNECTIONS 3
    static const std::vector<std::string> currencyPairs_ = {"XBTUSDT", "XBTETH", "ETHUSDT"};
#elif defined(USE_KRAKEN_EXCHANGE) || defined(USE_KRAKEN_MOCK_EXCHANGE)  
  #if defined(USE_PORTFOLIO_122)
    #define NUMBER_OF_CONNECTIONS 122
    static const std::vector<std::string> currencyPairs_ = {"KSM/EUR","KSM/BTC","KSM/DOT","KSM/GBP","KSM/ETH","KSM/USD","GBP/USD","BTC/CAD","BTC/EUR","BTC/AUD","BTC/JPY","BTC/GBP","BTC/CHF","BTC/USDT","BTC/USD","BTC/USDC","LTC/EUR","LTC/BTC","LTC/AUD","LTC/JPY","LTC/GBP","LTC/ETH","LTC/USDT","LTC/USD","SOL/EUR","SOL/BTC","SOL/GBP","SOL/ETH","SOL/USDT","SOL/USD","DOT/EUR","DOT/BTC","DOT/JPY","DOT/GBP","DOT/ETH","DOT/USDT","DOT/USD","ETH/CAD","ETH/EUR","ETH/BTC","ETH/AUD","ETH/JPY","ETH/GBP","ETH/CHF","ETH/USDT","ETH/USD","ETH/USDC","LINK/EUR","LINK/BTC","LINK/AUD","LINK/JPY","LINK/GBP","LINK/ETH","LINK/USDT","LINK/USD","USDC/CAD","USDC/EUR","USDC/AUD","USDC/GBP","USDC/CHF","USDC/USDT","USDC/USD","ADA/EUR","ADA/BTC","ADA/AUD","ADA/GBP","ADA/ETH","ADA/USDT","ADA/USD","ATOM/EUR","ATOM/BTC","ATOM/GBP","ATOM/ETH","ATOM/USDT","ATOM/USD","USDT/EUR","USDT/AUD","USDT/JPY","USDT/GBP","USDT/CHF","USDT/USD","USDT/CAD","AUD/JPY","AUD/USD","XRP/CAD","XRP/EUR","XRP/BTC","XRP/AUD","XRP/GBP","XRP/ETH","XRP/USDT","XRP/USD","EUR/CAD","EUR/AUD","EUR/JPY","EUR/GBP","EUR/CHF","EUR/USD","BCH/EUR","BCH/BTC","BCH/AUD","BCH/JPY","BCH/GBP","BCH/ETH","BCH/USDT","BCH/USD","USD/CHF","USD/JPY","USD/CAD","ALGO/EUR","ALGO/BTC","ALGO/GBP","ALGO/ETH","ALGO/USDT","ALGO/USD", };
  #elif defined(USE_PORTFOLIO_92)
    #define NUMBER_OF_CONNECTIONS 92
    static const std::vector<std::string> currencyPairs_ = {"BCH/USD","BCH/BTC","BCH/EUR","BCH/AUD","BCH/GBP","BCH/ETH","BCH/USDT","BCH/JPY","BTC/USD","BTC/EUR","BTC/USDC","BTC/AUD","BTC/GBP","BTC/CAD","BTC/USDT","BTC/JPY","USD/CAD","USD/JPY","XRP/USD","XRP/BTC","XRP/EUR","XRP/AUD","XRP/GBP","XRP/ETH","XRP/CAD","XRP/USDT","EUR/USD","EUR/AUD","EUR/GBP","EUR/CAD","EUR/JPY","LTC/USD","LTC/EUR","LTC/BTC","LTC/AUD","LTC/GBP","LTC/ETH","LTC/USDT","LTC/JPY","ETH/USD","ETH/EUR","ETH/BTC","ETH/USDC","ETH/AUD","ETH/GBP","ETH/CAD","ETH/USDT","ETH/JPY","LINK/USD","LINK/BTC","LINK/EUR","LINK/AUD","LINK/GBP","LINK/ETH","LINK/USDT","LINK/JPY","ADA/USD","ADA/BTC","ADA/EUR","ADA/AUD","ADA/GBP","ADA/ETH","ADA/USDT","USDC/USD","USDC/EUR","USDC/AUD","USDC/GBP","USDC/CAD","USDC/USDT","GBP/USD","DOT/USD","DOT/BTC","DOT/EUR","DOT/GBP","DOT/ETH","DOT/USDT","DOT/JPY","USDT/USD","USDT/EUR","USDT/AUD","USDT/GBP","USDT/CAD","USDT/JPY","AUD/USD","AUD/JPY", };
  #elif defined(USE_PORTFOLIO_50)
    #define NUMBER_OF_CONNECTIONS 50
    static const std::vector<std::string> currencyPairs_ = {"BCH/JPY","BCH/ETH","BCH/GBP","BCH/AUD","BCH/BTC","BCH/USDT","BCH/EUR","BCH/USD","USDT/JPY","USDT/GBP","USDT/AUD","USDT/EUR","USDT/USD","BTC/JPY","BTC/GBP","BTC/AUD","BTC/USDT","BTC/EUR","BTC/USD","EUR/GBP","EUR/JPY","EUR/AUD","EUR/USD","ETH/JPY","ETH/EUR","ETH/AUD","ETH/BTC","ETH/USDT","ETH/GBP","ETH/USD","USD/JPY","LINK/JPY","LINK/ETH","LINK/EUR","LINK/AUD","LINK/BTC","LINK/USDT","LINK/GBP","LINK/USD","LTC/JPY","LTC/ETH","LTC/GBP","LTC/AUD","LTC/BTC","LTC/USDT","LTC/EUR","LTC/USD","GBP/USD","AUD/JPY","AUD/USD", };
  #elif defined(USE_PORTFOLIO_3)
    #define NUMBER_OF_CONNECTIONS 3
    static const std::vector<std::string> currencyPairs_ = {"USDT/USD", "SOL/USDT", "SOL/USD"};
  #endif
#endif

static SPSCQueue<BookBuilderGatewayToComponentQueueEntry>* bookBuilderGatewayToComponentQueue;

static int rxSeen, test;
static int interrupted[NUMBER_OF_CONNECTIONS];
static struct lws *clientWsis[NUMBER_OF_CONNECTIONS];

static struct ev_loop *loopEv; 
static ev_timer timeoutWatcher;

struct WebSocketClientEvContext
{
  ev_io socketWatcher;
  int sockfd;
  uint connectionIdx;
  char undecryptedReadBuffer[WEBSOCKET_CLIENT_RX_BUFFER_SIZE];	
  int undecryptedReadBufferSize = sizeof(undecryptedReadBuffer);
  char decryptedReadBuffer[WEBSOCKET_CLIENT_RX_BUFFER_SIZE];	
  int decryptedReadBufferSize = sizeof(decryptedReadBuffer);
  struct msghdr msg;
  struct iovec iov[1];
  struct timeval tv;
  char ctrlBuf[CMSG_SPACE(sizeof(tv))];
  struct cmsghdr* cmsg;
};

static struct WebSocketClientEvContext *wsClientsEvContexts[NUMBER_OF_CONNECTIONS];

struct WebSocketSubscriptionData {
    std::vector<std::string> currencyPairs;
    int connectionIdx;
};

static SSL *ssls[NUMBER_OF_CONNECTIONS];
static BIO *rbios[NUMBER_OF_CONNECTIONS];
static int sockfds[NUMBER_OF_CONNECTIONS];

static struct io_uring ring;
static struct io_uring_sqe *sqe;
static struct io_uring_cqe *cqe;

// static const struct lws_extension extensions[] = {
//         {
//                 "permessage-deflate",
//                 lws_extension_callback_pm_deflate,
//                       "permessage-deflate"
//                       "; client_no_context_takeover"
//                       "; client_max_window_bits"
//         },
//         { NULL, NULL, NULL /* terminator */ }
// };

static int
bookBuilderLwsCallback(struct lws *wsi, enum lws_callback_reasons reason,
	      void *user, void *in, size_t len)
{	
    int n, connectionIdx = (int)(intptr_t)lws_get_opaque_user_data(wsi);

	switch (reason) {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                in ? (char *)in : "(null)");
            clientWsis[connectionIdx] = NULL;
            break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED: {
            printf("LWS_CALLBACK_CLIENT_ESTABLISHED for %d\n", connectionIdx);
            lwsl_user("%s: established\n", __func__);
#ifndef USE_KRAKEN_MOCK_EXCHANGE
#ifndef USE_BITMEX_MOCK_EXCHANGE
    #if defined(USE_BITMEX_EXCHANGE) || defined(USE_BITMEX_TESTNET_EXCHANGE)
            std::string currencyPair = currencyPairs_[connectionIdx];
            std::string subscriptionMessage = "{\"op\":\"subscribe\",\"args\":[\"orderBookL2_25:" + currencyPair + "\"]}";
    #elif defined(USE_KRAKEN_EXCHANGE)
            std::string subscriptionMessage = R"({
                                                "method": "subscribe",
                                                "params": {
                                                    "channel": "book",
                                                    "depth": 10,
                                                    "snapshot": true,
                                                    "symbol": [)";
            subscriptionMessage += "\"" + currencyPairs_[connectionIdx] + "\"";
            subscriptionMessage += R"(]
                                        },
                                        "req_id": 1234567890
                                        }
                                    )";
    #endif
            // Allocate buffer with LWS_PRE bytes before the data
            unsigned char buf[LWS_PRE + subscriptionMessage.size()];

            // Copy subscription message to buffer after LWS_PRE bytes
            memcpy(&buf[LWS_PRE], subscriptionMessage.c_str(), subscriptionMessage.size());
                    
            // Send data using lws_write
            lws_write(wsi, &buf[LWS_PRE], subscriptionMessage.size(), LWS_WRITE_TEXT);
            
#endif
#endif
			interrupted[connectionIdx] = 1;
            break;
        }

        case LWS_CALLBACK_CLIENT_CLOSED:
            clientWsis[connectionIdx] = NULL;
            break;

        default:
            break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
        { "book-builder-lws-client", bookBuilderLwsCallback, 0, 0, 0, NULL, 0 },
        LWS_PROTOCOL_LIST_TERM
};

void handleSocketEvent (EV_P_ ev_io *w_, int revents) {
    // Cast the ev_io watcher pointer to our custom WebSocketClientEvContext structure
    struct WebSocketClientEvContext *w = (struct WebSocketClientEvContext *) w_;
    // Now we can access the custom data fields for the corresponding WebSocket client 
    uint connectionIdx = w->connectionIdx;

  	if (revents & EV_READ) {
        system_clock::time_point marketUpdatePollTimestamp = high_resolution_clock::now();
        // printf("SOCKET ready for reading for connection %d\n", connectionIdx);
        sqe = io_uring_get_sqe(&ring);

        if (!sqe) {
            perror("io_uring_get_sqe failed");
            io_uring_queue_exit(&ring);
            return;
        }

        // printf("Reading for sockfd %d\n", w->sockfd);
        io_uring_prep_recvmsg(sqe, connectionIdx, &w->msg, 0);  
        sqe->flags |= IOSQE_FIXED_FILE;

        if (io_uring_submit(&ring) < 0) {
            perror("io_uring_submit failed");
            io_uring_queue_exit(&ring);
            return;
        }

        int ret = io_uring_wait_cqe(&ring, &cqe);
        system_clock::time_point marketUpdateReadCompletionTimestamp = high_resolution_clock::now();
        
        if (ret < 0) {
            perror("Error waiting for completion: %s\n");
            return;
        }

        if (cqe->res < 0) {
            perror("recvmsg operation error");
            return;
        } 

        int undecryptedBytesRead = cqe->res;				
        int bytesBioWritten = BIO_write(rbios[connectionIdx], w->undecryptedReadBuffer, undecryptedBytesRead);
        int decryptedBytesRead = SSL_read(ssls[connectionIdx], w->decryptedReadBuffer, w->decryptedReadBufferSize);

        system_clock::time_point marketUpdateDecryptionCompletionTimestamp = high_resolution_clock::now();
        
        // printf("BYTES READ from ssl: %d\n", decryptedBytesRead);
        io_uring_cqe_seen(&ring, cqe);

        system_clock::time_point marketUpdateSocketRxTimestamp;
        w->msg.msg_control = w->ctrlBuf;
        w->msg.msg_controllen = sizeof(w->ctrlBuf);
        w->cmsg = CMSG_FIRSTHDR(&w->msg);

        if (w->cmsg->cmsg_level == SOL_SOCKET && w->cmsg->cmsg_type == SCM_TIMESTAMP) {
            memcpy(&w->tv, CMSG_DATA(w->cmsg), sizeof(w->tv));
            // std::cout << "Received packet at timestamp: " << w->tv.tv_sec << " seconds and " << w->tv.tv_usec << " microseconds" << std::endl;
            marketUpdateSocketRxTimestamp = std::chrono::system_clock::from_time_t((long)w->tv.tv_sec);
            marketUpdateSocketRxTimestamp += std::chrono::microseconds((long)w->tv.tv_usec);
        }

        struct BookBuilderGatewayToComponentQueueEntry queueEntry;
        memcpy(queueEntry.decryptedReadBuffer, w->decryptedReadBuffer, w->decryptedReadBufferSize);
        queueEntry.decryptedBytesRead = decryptedBytesRead;
        queueEntry.marketUpdateSocketRxTimestamp = marketUpdateSocketRxTimestamp;
        queueEntry.marketUpdatePollTimestamp = marketUpdatePollTimestamp;
        queueEntry.marketUpdateReadCompletionTimestamp = marketUpdateReadCompletionTimestamp;
        queueEntry.marketUpdateDecryptionCompletionTimestamp = marketUpdateDecryptionCompletionTimestamp;

        while (!bookBuilderGatewayToComponentQueue->push(queueEntry));  
    
        memset(w->undecryptedReadBuffer, 0, w->undecryptedReadBufferSize);
        memset(w->decryptedReadBuffer, 0, w->decryptedReadBufferSize);
        // memset(w->ctrlBuf, 0, sizeof(w->ctrlBuf));     
    }
}

// another callback, this time for a time-out
static void
timeout_cb (EV_P_ ev_timer *w, int revents)
{
  puts ("timeout");
  // this causes the innermost ev_run to stop iterating
  ev_break (EV_A_ EVBREAK_ONE);
}

void bookBuilderGateway(SPSCQueue<BookBuilderGatewayToComponentQueueEntry>& bookBuilderGatewayToComponentQueue_, std::vector<std::string> currencyPairs_, int orderManagerPipeEnd) {
    int numCores = std::thread::hardware_concurrency();
    
    if (numCores == 0) {
        std::cerr << "Error: Unable to determine the number of CPU cores." << std::endl;
        return;
    } else if (numCores < CPU_CORE_INDEX_FOR_BOOK_BUILDER_GATEWAY_THREAD) {
        std::cerr << "Error: Not enough cores to run the system." << std::endl;
        return;
    }

    int cpuCoreNumberForBookBuilderThread = CPU_CORE_INDEX_FOR_BOOK_BUILDER_GATEWAY_THREAD;
    setThreadAffinity(pthread_self(), cpuCoreNumberForBookBuilderThread);

    bookBuilderGatewayToComponentQueue = &bookBuilderGatewayToComponentQueue_;

    struct io_uring_params params;

    // Initialize io_uring
    // Use submission queue polling if user has root privileges
    if (geteuid()) {
        printf("You need root privileges to run the Order Manager with submission queue polling\n");
        int ret = io_uring_queue_init(NUMBER_OF_IO_URING_SQ_ENTRIES, &ring, 0);
        if (ret) {
            perror("io_uring_queue_init");
            return;
        }
    } else {
        printf("Running the Order Manager with submission queue polling\n");
        memset(&params, 0, sizeof(params));
        params.flags |= IORING_SETUP_SQPOLL;
        params.flags |= IORING_SETUP_SQ_AFF;
        params.sq_thread_idle = 200000;
        params.sq_thread_cpu = CPU_CORE_INDEX_FOR_SQ_POLL_THREAD;
        int ret = io_uring_queue_init_params(NUMBER_OF_IO_URING_SQ_ENTRIES, &ring, &params);
        
        if (ret) {
            perror("io_uring_queue_init");
            return;
        }
    
        int bookBuilderRingFd = ring.ring_fd;
        if (write(orderManagerPipeEnd, &bookBuilderRingFd, sizeof(bookBuilderRingFd)) != sizeof(bookBuilderRingFd)) {
            perror("Pipe write error in Book Builder");
            return;
        }

        printf("WEB SOCKET CLIENT RING FD: %d\n", bookBuilderRingFd);
    }
	
	const char *p;
	int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

	lws_set_log_level(logs, NULL);
	lwsl_user("LWS Book Builder ws client rx [-d <logs>] [--h2] [-t (test)]\n");

    loopEv = ev_default_loop(EVBACKEND_EPOLL);
    void *foreignLoops[1];
    foreignLoops[0] = loopEv;

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.foreign_loops = foreignLoops;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_WITH_LIBEV;
    info.port = CONTEXT_PORT_NO_LISTEN; 
    info.fd_limit_per_thread = 1 + NUMBER_OF_CONNECTIONS;
    info.protocols = protocols;

    struct lws_context *context;
    context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		return;
	}

    struct lws_client_connect_info i;
	memset(&i, 0, sizeof i);
	i.context = context;
#if defined(USE_KRAKEN_MOCK_EXCHANGE) || defined(USE_BITMEX_MOCK_EXCHANGE)
    i.port = 7681;
    i.address = "146.169.41.107";
    i.ssl_connection = LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK | LCCSCF_ALLOW_INSECURE | LWS_SERVER_OPTION_IGNORE_MISSING_CERT | LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED;
#elif defined(USE_KRAKEN_EXCHANGE)
    i.port = 443;
	i.address = "ws.kraken.com";
    i.path = "/v2";
#elif defined(USE_BITMEX_EXCHANGE)
    i.port = 443;
	i.address = "ws.bitmex.com";
    i.path = "/realtime";
#elif defined(USE_BITMEX_TESTNET_EXCHANGE)
    i.port = 443;
	i.address = "testnet.bitmex.com";
    i.path = "/realtime";
#endif
    i.ssl_connection = i.ssl_connection | LCCSCF_USE_SSL | LCCSCF_PRIORITIZE_READS;
	i.host = i.address;
	i.origin = i.address;
	i.protocol = NULL; 
    
    for (int m = 0; m < NUMBER_OF_CONNECTIONS; m++) {
        i.pwsi = &clientWsis[m];
        i.opaque_user_data = (void *)(intptr_t) m;
        lws_client_connect_via_info(&i);

        while (n >= 0 && clientWsis[m] && !interrupted[m]) {
            n = lws_service(context, 0);
        }
        ssls[m] = lws_get_ssl(clientWsis[m]);
        rbios[m] = BIO_new(BIO_s_mem());
	    SSL_set_bio(ssls[m], rbios[m], NULL);
        sockfds[m] = lws_get_socket_fd(clientWsis[m]);
        int timestamp_option = 1;
        if (setsockopt(sockfds[m], SOL_SOCKET, SO_TIMESTAMP, &timestamp_option, sizeof(timestamp_option)) < 0) {
            perror("setsockopt SO_TIMESTAMP failed");
            return;
        }
    }

    if (io_uring_register_files(&ring, sockfds, NUMBER_OF_CONNECTIONS) < 0) {
        perror("io_uring_register_files failed");
        return;
    }

	
    for (int connectionIdx = 0; connectionIdx < NUMBER_OF_CONNECTIONS; connectionIdx++) {
        wsClientsEvContexts[connectionIdx] = new WebSocketClientEvContext();
        wsClientsEvContexts[connectionIdx]->sockfd = sockfds[connectionIdx];
        wsClientsEvContexts[connectionIdx]->connectionIdx = connectionIdx;

        wsClientsEvContexts[connectionIdx]->iov[0].iov_base = wsClientsEvContexts[connectionIdx]->undecryptedReadBuffer;
        wsClientsEvContexts[connectionIdx]->iov[0].iov_len = wsClientsEvContexts[connectionIdx]->undecryptedReadBufferSize;

        wsClientsEvContexts[connectionIdx]->msg.msg_control = wsClientsEvContexts[connectionIdx]->ctrlBuf;
        wsClientsEvContexts[connectionIdx]->msg.msg_controllen = sizeof(wsClientsEvContexts[connectionIdx]->ctrlBuf);
        wsClientsEvContexts[connectionIdx]->msg.msg_iov = wsClientsEvContexts[connectionIdx]->iov;
        wsClientsEvContexts[connectionIdx]->msg.msg_iovlen = 1;

        ev_io_init(&wsClientsEvContexts[connectionIdx]->socketWatcher, handleSocketEvent, sockfds[connectionIdx], EV_READ);

        ev_io_start(loopEv, &wsClientsEvContexts[connectionIdx]->socketWatcher);
    }
    
    // initialise a timer watcher, then start it
    // simple non-repeating 5.5 second timeout
    ev_timer_init(&timeoutWatcher, timeout_cb, 600, 0.);
    ev_timer_start(loopEv, &timeoutWatcher); 

    while (true) {
        ev_run(loopEv, EVRUN_NOWAIT);
    }

	lws_context_destroy(context);
    close(orderManagerPipeEnd);    
}