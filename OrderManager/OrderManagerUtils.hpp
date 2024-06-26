#ifndef MAIN_ORDERMANAGERUTILS_H
#define MAIN_ORDERMANAGERUTILS_H

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <rapidjson/document.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

/* Global SSL context */
SSL_CTX *ctx;

#define RX_DEFAULT_BUF_SIZE 8192
#define ARBITRAGE_BATCH_SIZE 3
#define EVP_MAX_MD_SIZE 64

char *generateBitmexApiSignature(const char *decodedKey, int decodedKeyLen, const char *msg, int msgLen) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;

    HMAC(
            EVP_sha256(),
            decodedKey,
            decodedKeyLen,
            (unsigned char *)msg,
            msgLen,
            hash,
            &hashLen
    );

    char *result = (char *) malloc(hashLen * 2 + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < (int) hashLen; i++) {
        sprintf(result + (i * 2), "%02x", hash[i]);
    }
    result[hashLen * 2] = '\0';

    return result;
}

std::string generateNonce() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    long long microsecondsSinceEpoch = duration.count();
    return std::to_string(microsecondsSinceEpoch);
}

std::vector<unsigned char> base64Decode(const std::string &in) {
    BIO *bio, *b64;
    int decodeLen = in.size();
    std::vector<unsigned char> out(decodeLen);

    bio = BIO_new_mem_buf(in.data(), in.size());
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    decodeLen = BIO_read(bio, out.data(), in.size());
    out.resize(decodeLen);

    BIO_free_all(bio);
    return out;
}

std::vector<unsigned char> sha256(const std::string &data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH);
}

std::vector<unsigned char> hmacSha512(const std::vector<unsigned char> &key, const std::string &data) {
    unsigned char* hmac = HMAC(EVP_sha512(), key.data(), key.size(),
                               reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), NULL, NULL);

    return std::vector<unsigned char>(hmac, hmac + SHA512_DIGEST_LENGTH);
}

std::string base64Encode(const std::vector<unsigned char> &data) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string base64encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return base64encoded;
}

std::string generateKrakenApiSignature(const std::string &uriPath, const std::string &nonce, const std::string &postData, const std::string &secretKey) {
    std::string noncePostData = nonce + postData;
    std::vector<unsigned char> sha256Hash = sha256(noncePostData);
    std::string data = uriPath + std::string(reinterpret_cast<char*>(sha256Hash.data()), sha256Hash.size());
    std::vector<unsigned char> decoded_key = base64Decode(secretKey);
    std::vector<unsigned char> hmac = hmacSha512(decoded_key, data);
    return base64Encode(hmac);
}


void handle_error(const char *file, int lineno, const char *msg) {
    fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
    ERR_print_errors_fp(stderr);
    exit(-1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)

void die(const char *msg) {
    perror(msg);
    exit(1);
}

void print_unencrypted_data(char *buf, size_t len) {
    printf("%.*s", (int)len, buf);
}

/* An instance of this object is created each time a client connection is
 * accepted. It stores the client file descriptor, the SSL objects, and data
 * which is waiting to be either written to socket or encrypted. */
struct OrderManagerClient
{
    int sockfd;

    SSL *ssl;

    BIO *rbio; /* SSL reads from, we write to. */
    BIO *wbio; /* SSL writes to, we read from. */

    /* Bytes waiting to be written to socket. This is data that has been generated
     * by the SSL object, either due to encryption of user input, or, writes
     * requires due to peer-requested SSL renegotiation. */
    char* writeBuffer;
    size_t writeLen;

    /* Bytes waiting to be encrypted by the SSL object. */
    char* encrypt_buf;
    size_t encrypt_len;

    char response_buf[10000];

    /* Store the previous state string */
    const char * last_state;

    /* Method to invoke when unencrypted bytes are available. */
    void (*io_on_read)(char *buf, size_t len);
};

/* This enum contols whether the SSL connection needs to initiate the SSL
 * handshake. */
enum ssl_mode { SSLMODE_SERVER, SSLMODE_CLIENT };


void ssl_client_init(struct OrderManagerClient *p,
                     int sockfd,
                     enum ssl_mode mode)
{
    memset(p, 0, sizeof(struct OrderManagerClient));

    p->sockfd = sockfd;

    p->rbio = BIO_new(BIO_s_mem());
    p->wbio = BIO_new(BIO_s_mem());
    p->ssl = SSL_new(ctx);

    if (mode == SSLMODE_SERVER)
        SSL_set_accept_state(p->ssl);  /* ssl server mode */
    else if (mode == SSLMODE_CLIENT)
        SSL_set_connect_state(p->ssl); /* ssl client mode */

    SSL_set_bio(p->ssl, p->rbio, p->wbio);

    p->io_on_read = print_unencrypted_data;
}


void ssl_client_cleanup(struct OrderManagerClient *p)
{
    SSL_free(p->ssl);   /* free the SSL object and its BIO's */
    free(p->writeBuffer);
    free(p->encrypt_buf);
}


int ssl_client_want_write(struct OrderManagerClient *cp) {
    return (cp->writeLen>0);
}


/* Obtain the return value of an SSL operation and convert into a simplified
 * error code, which is easier to examine for failure. */
enum sslstatus { SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL};


static enum sslstatus get_sslstatus(SSL* ssl, int n)
{
    switch (SSL_get_error(ssl, n))
    {
        case SSL_ERROR_NONE:
            return SSLSTATUS_OK;
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            return SSLSTATUS_WANT_IO;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        default:
            return SSLSTATUS_FAIL;
    }
}


/* Handle request to send unencrypted data to the SSL.  All we do here is just
 * queue the data into the encrypt_buf for later processing by the SSL
 * object. */
void send_unencrypted_bytes(struct OrderManagerClient *client, const char *buf, size_t len)
{
    client->encrypt_buf = (char*)realloc(client->encrypt_buf, client->encrypt_len + len);
    memcpy(client->encrypt_buf+client->encrypt_len, buf, len);
    client->encrypt_len += len;
}


/* Queue encrypted bytes. Should only be used when the SSL object has requested a
 * write operation. */
void queue_encrypted_bytes(struct OrderManagerClient *client, const char *buf, size_t len)
{
    client->writeBuffer = (char*)realloc(client->writeBuffer, client->writeLen + len);
    memcpy(client->writeBuffer+client->writeLen, buf, len);
    client->writeLen += len;
}


void print_ssl_state(struct OrderManagerClient *client)
{
    const char * current_state = SSL_state_string_long(client->ssl);
    if (current_state != client->last_state) {
        if (current_state)
            printf("SSL-STATE: %s\n", current_state);
        client->last_state = current_state;
    }
}


void print_ssl_error()
{
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);
    char *buf;
    size_t len = BIO_get_mem_data(bio, &buf);
    if (len > 0)
        printf("SSL-ERROR: %s", buf);
    BIO_free(bio);
}


enum sslstatus do_ssl_handshake(struct OrderManagerClient *client)
{
    printf("DOING SSL HANDSHAKE\n");
    char buf[RX_DEFAULT_BUF_SIZE];
    enum sslstatus status;

    print_ssl_state(client);
    int n = SSL_do_handshake(client->ssl);
    print_ssl_state(client);    
    status = get_sslstatus(client->ssl, n);

    /* Did SSL request to write bytes? */
    if (status == SSLSTATUS_WANT_IO)
        do {
            n = BIO_read(client->wbio, buf, sizeof(buf));
            if (n > 0)
                queue_encrypted_bytes(client, buf, n);
            else if (!BIO_should_retry(client->wbio))
                return SSLSTATUS_FAIL;
        } while (n>0);

    return status;
}

/* Process SSL bytes received from the peer. The data needs to be fed into the
   SSL object to be unencrypted.  On success, returns 0, on SSL error -1. */
int on_read_cb(struct OrderManagerClient *client, char* src, size_t len, bool is_handshake)
{
    // printf("£££££££££££££££££££££££££££££");
    char buf[RX_DEFAULT_BUF_SIZE];
    enum sslstatus status;
    int n;

    while (len > 0) {
        // printf("###################################");
        n = BIO_write(client->rbio, src, len);

        if (n<=0)
            return -1; /* assume bio write failure is unrecoverable */

        src += n;
        len -= n;

        if (!SSL_is_init_finished(client->ssl)) {
            if (do_ssl_handshake(client) == SSLSTATUS_FAIL)
                return -1;
            if (!SSL_is_init_finished(client->ssl))
                return 0;
        }

        /* The encrypted data is now in the input bio so now we can perform actual
         * read of unencrypted data. */
        size_t total_bytes_read = strlen(client->response_buf);
        do {
            n = SSL_read(client->ssl, buf, sizeof(buf));
            if (n > 0) {
                if (!is_handshake) {
                    // Append the data read from buf to response_buf
                    memcpy(client->response_buf + total_bytes_read, buf, n);
                    total_bytes_read += n; // Update the total bytes read
                }
            }
        } while (n > 0);

        client->response_buf[total_bytes_read] = '\0';

        status = get_sslstatus(client->ssl, n);

        /* Did SSL request to write bytes? This can happen if peer has requested SSL
         * renegotiation. */
        if (status == SSLSTATUS_WANT_IO)
            do {
                n = BIO_read(client->wbio, buf, sizeof(buf));
                if (n > 0)
                    queue_encrypted_bytes(client, buf, n);
                else if (!BIO_should_retry(client->wbio))
                    return -1;
            } while (n>0);

        if (status == SSLSTATUS_FAIL)
            return -1;
    }

    return 0;
}

/* Process outbound unencrypted data that is waiting to be encrypted.  The
 * waiting data resides in encrypt_buf.  It needs to be passed into the SSL
 * object for encryption, which in turn generates the encrypted bytes that then
 * will be queued for later socket write. */
int do_encrypt(struct OrderManagerClient *client)
{
    char buf[RX_DEFAULT_BUF_SIZE];
    enum sslstatus status;

    if (!SSL_is_init_finished(client->ssl))
        return 0;

    while (client->encrypt_len>0) {
        int n = SSL_write(client->ssl, client->encrypt_buf, client->encrypt_len);
        status = get_sslstatus(client->ssl, n);

        if (n>0) {
            /* consume the waiting bytes that have been used by SSL */
            if ((size_t)n<client->encrypt_len)
                memmove(client->encrypt_buf, client->encrypt_buf+n, client->encrypt_len-n);
            client->encrypt_len -= n;
            client->encrypt_buf = (char*)realloc(client->encrypt_buf, client->encrypt_len);

            /* take the output of the SSL object and queue it for socket write */
            do {
                n = BIO_read(client->wbio, buf, sizeof(buf));
                if (n > 0)
                    queue_encrypted_bytes(client, buf, n);
                else if (!BIO_should_retry(client->wbio))
                    return -1;
            } while (n>0);
        }

        if (status == SSLSTATUS_FAIL)
            return -1;

        if (n==0)
            break;
    }
    return 0;
}

/* Read encrypted bytes from socket. */
int do_sock_read(struct OrderManagerClient *client, bool is_handshake)
{
    char buf[RX_DEFAULT_BUF_SIZE];
    ssize_t n = read(client->sockfd, buf, sizeof(buf));

    if (n>0)
        return on_read_cb(client, buf, (size_t)n, is_handshake);
    else
        return -1;
}

int do_sock_write(struct OrderManagerClient *client)
{
    ssize_t n = write(client->sockfd, client->writeBuffer, client->writeLen);

    if (n>0) {
        if ((size_t)n<client->writeLen)
            memmove(client->writeBuffer, client->writeBuffer+n, client->writeLen-n);
        client->writeLen -= n;
        client->writeBuffer = (char*)realloc(client->writeBuffer, client->writeLen);
        return 0;
    }
    else
        return -1;
}

void ssl_init(const char * certfile, const char* keyfile)
{
    /* SSL library initialisation */

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
#if OPENSSL_VERSION_MAJOR < 3
    ERR_load_BIO_strings(); // deprecated since OpenSSL 3.0
#endif
    ERR_load_crypto_strings();

    /* create the SSL server context */
    ctx = SSL_CTX_new(TLS_method());
    if (!ctx)
        die("SSL_CTX_new()");

    /* Load certificate and private key files, and check consistency */
    if (certfile && keyfile) {
        if (SSL_CTX_use_certificate_file(ctx, certfile,  SSL_FILETYPE_PEM) != 1)
            int_error("SSL_CTX_use_certificate_file failed");

        if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) != 1)
            int_error("SSL_CTX_use_PrivateKey_file failed");

        /* Make sure the key and certificate file match. */
        if (SSL_CTX_check_private_key(ctx) != 1)
            int_error("SSL_CTX_check_private_key failed");
        else
            printf("certificate and private key loaded and verified\n");
    }

    /* Recommended to avoid SSLv2 & SSLv3 */
    SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    // Enable session caching
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_NO_INTERNAL);
}

using namespace rapidjson;

Document extract_json(const std::string& response) {
    size_t json_start = response.find("{");

    if (json_start != std::string::npos) {
        // Extract the JSON string from the response
        std::string json_str = response.substr(json_start);

        Document doc;
        doc.Parse(json_str.c_str(), json_str.length());

        if (doc.HasParseError()) {
            std::cerr << "Error parsing JSON: " << doc.GetParseError() << std::endl;
            return Document();
        }

        return doc;
    } else {
        return Document();
    }
}

std::string getCurrentTime(std::chrono::system_clock::time_point now) {
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    // Format time
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%dT%H:%M:%S");

    // Get milliseconds
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Convert milliseconds to string and append to result
    oss << "." << std::setfill('0') << std::setw(3) << milliseconds.count() << "Z";

    return oss.str();
}



#endif //MAIN_ORDERMANAGERUTILS_H
