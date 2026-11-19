
#include "SHA1.h"



#ifdef WIN32

static char *szCompiledFile=__FILE__;

class init_sha1 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_sha1() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_SHA1_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo SHA1");
	}
	~init_sha1() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_sha1;

CSHA1::CSHA1() : hash(nullptr) {
}

CSHA1::~CSHA1() {
	if (hash!=nullptr)
		BCryptDestroyHash(hash);
}
void CSHA1::Init() {
	if (hash != nullptr)
		throw logged_error("Un'operazione di hash � gi� in corso");
	if (BCryptCreateHash(algo_sha1.algo, &hash, nullptr, 0, nullptr, 0, 0) != 0)
		throw logged_error("Errore nella creazione dell'hash SHA1");
}
void CSHA1::Update(ByteArray data) {
	if (hash == nullptr)
		throw logged_error("Hash non inizializzato");
	if (BCryptHashData(hash, data.data(), (ULONG)data.size(), 0) != 0)
		throw logged_error("Errore nell'hash dei dati SHA1");
}
ByteDynArray CSHA1::Final() {
	if (hash == nullptr)
		throw logged_error("Hash non inizializzato");
	ByteDynArray resp(SHA_DIGEST_LENGTH);
	if (BCryptFinishHash(hash, resp.data(), (ULONG)resp.size(), 0) != 0)
		throw logged_error("Errore nel calcolo dell'hash SHA1");

	BCryptDestroyHash(hash);
	hash = nullptr;

	return resp;
}

#else

CSHA1::CSHA1() : isInit(false) {
}

CSHA1::~CSHA1() {
}

void CSHA1::Init() {
//    if (isInit)
//        throw logged_error("Un'operazione di hash � gi� in corso");
	SHA1_Init(&ctx);
	isInit = true;
}
void CSHA1::Update(ByteArray data) {
	if (!isInit)
		throw logged_error("Hash non inizializzato");
	SHA1_Update(&ctx, data.data(), data.size());
}
ByteDynArray CSHA1::Final() {
	if (!isInit)
		throw logged_error("Hash non inizializzato");
	ByteDynArray resp(SHA_DIGEST_LENGTH);
	SHA1_Final(resp.data(), &ctx);
	isInit = false;

	return resp;
}
#endif

ByteDynArray CSHA1::Digest(ByteArray data)
{
	Init();
	Update(data);
	return Final();
}
