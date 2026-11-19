
#include "MD5.h"

#ifdef WIN32

class init_md5 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_md5() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_MD5_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo MD5");
	}
	~init_md5() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_md5;

CMD5::CMD5() : hash(nullptr) {
}

CMD5::~CMD5() {
	if (hash != nullptr)
		BCryptDestroyHash(hash);
}
void CMD5::Init() {
	if (hash != nullptr)
		throw logged_error("Un'operazione di hash � gi� in corso");
	if (BCryptCreateHash(algo_md5.algo, &hash, nullptr, 0, nullptr, 0, 0) != 0)
		throw logged_error("Errore nella creazione dell'hash SHA1");
}
void CMD5::Update(ByteArray data) {
	if (hash == nullptr)
		throw logged_error("Hash non inizializzato");
	if (BCryptHashData(hash, data.data(), (ULONG)data.size(), 0) != 0)
		throw logged_error("Errore nell'hash dei dati SHA1");
}
ByteDynArray CMD5::Final() {
	if (hash == nullptr)
		throw logged_error("Hash non inizializzato");
	ByteDynArray resp(MD5_DIGEST_LENGTH);
	if (BCryptFinishHash(hash, resp.data(), (ULONG)resp.size(), 0) != 0)
		throw logged_error("Errore nel calcolo dell'hash SHA1");

	BCryptDestroyHash(hash);
	hash = nullptr;

	return resp;
}

#else

CMD5::CMD5() : isInit(false) {
}

CMD5::~CMD5() {
}

void CMD5::Init() {
	//if (isInit)
	//	throw logged_error("Un'operazione di hash � gi� in corso");
	MD5_Init(&ctx);
	isInit = true;
}
void CMD5::Update(ByteArray data) {
	if (!isInit)
		throw logged_error("Hash non inizializzato");
	MD5_Update(&ctx, data.data(), data.size());
}
ByteDynArray CMD5::Final() {
	if (!isInit)
		throw logged_error("Hash non inizializzato");
	ByteDynArray resp(MD5_DIGEST_LENGTH);
	MD5_Final(resp.data(), &ctx);
	isInit = false;

	return resp;
}
#endif

ByteDynArray CMD5::Digest(ByteArray data)
{
	Init();
	Update(data);
	return Final();
}
