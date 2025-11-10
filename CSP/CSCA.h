#ifndef CSCA_H
#define CSCA_H

#include <vector>
#include <string>
#include <memory>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <windows.h>
#include <urlmon.h>

class CSCA {
public:
	CSCA();
	~CSCA();

	bool initialize();
	bool loadFromFiles(const std::vector<std::string>& certFiles);
	bool verifyCertificate(const std::string& certPath);
	bool verifyCertificate(const unsigned char* certData, size_t certSize);

	std::vector<X509*> getCertificateChain(const std::string& certPath);

	void cleanup();

	void setLogging(bool enabled) { loggingEnabled_ = enabled; }

	bool isInitialized() const { return initialized_; }

	size_t getCertificateCount() const { return cscaCertificates_.size(); }

private:
	static const std::vector<std::string> CSCA_URLS;

	X509_STORE* trustStore_;

	std::vector<X509*> cscaCertificates_;

	bool loggingEnabled_;
	bool initialized_;

	std::string extractHashFromUrl(const std::string& url);
	std::string generateTempFilename(const std::string& hash);
	bool downloadCertificate(const std::string& url, const std::string& filename);
	X509* loadCertificateFromData(const unsigned char* data, size_t size);
	X509* loadCertificateFromFile(const std::string& filename);
	bool isSelfSigned(X509* cert);
	bool verifyInternal(X509* certDS);
};

#endif // CSCA_H