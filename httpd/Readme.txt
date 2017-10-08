==Accessing certificate data PHP==

Always remember that you need to configure your web server to pass the variables to your PHP process.

When a client certificate is available, the$_SERVERvariable contains a bunch ofSSL_CLIENT_*variables .

$_SERVER['SSL_CLIENT_VERIFY'] is an important one. Don't use the certificate if it does not equal SUCCESS. When no certificate is passed, it is NONE.

All variables with SSL_CLIENT_I_* are about the issuer, that is the CA. SSL_CLIENT_S_* are about the "subject", the user that sent the client certificate.

As described in the techspecification [http://www.agid.gov.it/sites/default/files/documentazione/cie_3.0_-_specifiche_chip.pdf] the most usefull fields are the

    surname (Object ID: 2.5.4.42)
    givenName(Object ID: 2.5.4.4)
    commonName (Object ID: 2.5.4.3): the italian TID (fiscal code), the id number for services . The separator is (slash, ASCII 0x2F). eg: BNCLCU75M11B950T/AAAAA112345678





===========================================================================
#Reference Configuration for ssl and certificate required

#-------------------------------------------
#  apache SSL
#
# nomehost.hackdev17.it
#
LoadModule ssl_module modules/mod_ssl.so
Listen 443
AddType application/x-x509-ca-cert.crt
AddType application/x-pkcs7-crl.crl
SSLPassPhraseDialog  builtin
#----------------------------------------
# cache SSL tuning
#----------------------------------------
SSLSessionCache dbm:/var/cache/mod_ssl/cache_ssl
SSLSessionCache shm:/var/cache/mod_ssl/cache_ssl(512000)
SSLSessionCacheTimeout 300
SSLMutex default
SSLRandomSeed startup file:/dev/urandom  256
SSLRandomSeed connect builtin
SSLCryptoDevice builtin


#----------------------------------------
#  client auth required
#----------------------------------------
SSLVerifyClient require

#----------------------------------------
# 
SSLCertificateFile     /etc/pki/tls/certs/myhost.crt
SSLCertificateKeyFile  /etc/pki/tls/private/myhost.key

# ca contains the CIEca.
SSLCACertificateFile   /etc/pki/tls/certs/ca
#----------------------------------------
#

#----------------------------------------
#  PHP
#----------------------------------------
AddHandler php5-script .php
AddType text/html .php
#----------------------------------------
#  PHP
#----------------------------------------


#----------------------------------------
# Virtual Host myhost.hackdev17.it
#----------------------------------------
<VirtualHost  11.22.33.44:443>
ServerName    myhost.hackdev17.it:443
ErrorLog      logs/myhost.hackdev17.it_ssl_error_log
TransferLog   logs/myhost.hackdev17.it_ssl_access_log
LogLevel      info
ServerAlias   myhost.hackdev17.it

DocumentRoot  "/var/www/html/"

SSLEngine      on
SSLCipherSuite ALL:!ADH:!EXPORT:!SSLv2:RC4+RSA:+HIGH:+MEDIUM:+LOW

# important!!! to export cert data visisbility
SSLOptions      +StdEnvVars +ExportCertData +OptRenegotiate
SetEnvIf User-Agent ".*MSIE.*" \
         nokeepalive ssl-unclean-shutdown \
         downgrade-1.0 force-response-1.0

CustomLog logs/myhost.hackdev17.it_ssl_request_log \
          "%t %h [%{SSL_CLIENT_S_DN_CN}x] \"%r\" [%T] [%D]"

RewriteEngine On


<Location /optional location/>
         SSLVerifyClient optional
</Location>



</VirtualHost>

====================================================================================
The attached zip contains the full list Certification Authority for Italy 
