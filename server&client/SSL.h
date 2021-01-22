#include <iostream>
using namespace std;

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

SSL_CTX* InitClientCTX();//Use SSLv23_client_method() to initialize SSL Content Text
SSL_CTX* InitServerCTX();//Use SSLv23_server_method() to initialize SSL Content Text
void LoadCertificates(SSL_CTX* ctx);//load user's certificate & load user's private key & check correctness of private key
void ShowCerts(SSL *ssl);


