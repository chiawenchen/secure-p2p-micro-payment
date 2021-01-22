#include "SSL.h"

SSL_CTX* InitClientCTX()
{
    SSL_CTX *ctx;
    // Initialize SSL
    SSL_library_init();
    // load all SSL Algorithms
    OpenSSL_add_all_algorithms();
    // load all SSL error messages
    SSL_load_error_strings();
    // Use SSL V2 & V3 standard to generate SSL_CTX, which is SSL Content Text 
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        abort();
    }
    return ctx;
}


SSL_CTX* InitServerCTX()
{
    SSL_CTX *ctx;
    // Initialize SSL
    SSL_library_init();
    // load all SSL Algorithms
    OpenSSL_add_all_algorithms();
    // load all SSL error messages
    SSL_load_error_strings();
    // Use SSL V2 & V3 standard to generate SSL_CTX, which is SSL Content Text 
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        abort();
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx)
{
    //Auto generate certificate
    EVP_PKEY * pkey;
    pkey = EVP_PKEY_new();
    RSA * rsa;
    rsa = RSA_generate_key(
        2048,   /* number of bits for the key - 2048 is a sensible value */
        RSA_F4, /* exponent - RSA_F4 is defined as 0x10001L */
        NULL,   /* callback - can be NULL if we aren't displaying progress */
        NULL    /* callback argument - not needed in this case */
    );
    EVP_PKEY_assign_RSA(pkey, rsa);

    X509 * x509;
    x509 = X509_new();

    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);

    X509_set_pubkey(x509, pkey);

    X509_NAME * name;
    name = X509_get_subject_name(x509);

    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"TW", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"NTUIM.", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"Joan", -1, -1, 0);

    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha1());
    //End Auto generate certificate

    // load client's digital certificate. The certificate is used to sending to client. The certificate include public key
    if ( SSL_CTX_use_certificate(ctx, x509) <= 0 ) 
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    // load user's private key
    if ( SSL_CTX_use_PrivateKey(ctx, pkey) <= 0 ) 
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    //Check the correctness of client's private key
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL)
    {
        printf("Digital certificate information:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Certificate: %s\n", line);

        free(line);

        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);

        free(line);
        X509_free(cert);
    }
    else
        printf("No certificate information！\n");
}



