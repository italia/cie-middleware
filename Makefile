CXX = clang++
MACOSX_DEPLOYMENT_TARGET ?= 14.0
export MACOSX_DEPLOYMENT_TARGET
CXXFLAGS = -std=c++17 -fPIC -O2 -Wall -Wno-unused-variable -Wno-unused-function \
           -Wno-reorder-ctor -Wno-format-security -Wno-deprecated-declarations \
           -Wno-unused-private-field -Wno-sign-compare -Wno-tautological-compare \
           -Wno-nonportable-include-path \
           -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)

OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl@3)
LIBXML_PREFIX := $(shell brew --prefix libxml2 2>/dev/null || echo /opt/homebrew/opt/libxml2)
FREETYPE_PREFIX := $(shell brew --prefix freetype 2>/dev/null || echo /opt/homebrew/opt/freetype)
PNG_PREFIX := $(shell brew --prefix libpng 2>/dev/null || echo /opt/homebrew/opt/libpng)
FONTCONFIG_PREFIX := $(shell brew --prefix fontconfig 2>/dev/null || echo /opt/homebrew/opt/fontconfig)
CURL_PREFIX := $(shell brew --prefix curl 2>/dev/null || echo /opt/homebrew/opt/curl)
CRYPTOPP_PREFIX := $(shell brew --prefix cryptopp 2>/dev/null || echo /opt/homebrew/opt/cryptopp)
ICONV_PREFIX := $(shell brew --prefix libiconv 2>/dev/null || echo /usr)
BZIP2_PREFIX := $(shell brew --prefix bzip2 2>/dev/null || echo /usr)
JPEG_PREFIX := $(shell brew --prefix jpeg 2>/dev/null || echo /opt/homebrew/opt/jpeg)
TIFF_PREFIX := $(shell brew --prefix libtiff 2>/dev/null || echo /opt/homebrew/opt/libtiff)
IDN_PREFIX := $(shell brew --prefix libidn 2>/dev/null || echo /opt/homebrew/opt/libidn)
UNISTRING_PREFIX := $(shell brew --prefix libunistring 2>/dev/null || echo /opt/homebrew/opt/libunistring)
PREFIX_DIR := third_party/prefix
SIGN_SDK_DIR := cie_sign_sdk_macos
SIGN_SDK_LIB := $(SIGN_SDK_DIR)/build/libcie_sign_sdk.a

INCLUDES = -I. -ICSP -IPKCS11 \
           -I$(OPENSSL_PREFIX)/include

LDFLAGS = -shared -framework PCSC \
          -L$(OPENSSL_PREFIX)/lib \
          -lssl -lcrypto

TARGET = libciepki.dylib
FIRMA_TARGET = libciefirma.dylib

SRCS = \
    CSP/PKCS11/PKCS11Functions.cpp \
    CSP/PKCS11/Slot.cpp \
    CSP/PKCS11/session.cpp \
    CSP/PKCS11/P11Object.cpp \
    CSP/PKCS11/Mechanism.cpp \
    CSP/PKCS11/CIEP11Template.cpp \
    CSP/PKCS11/CardContext.cpp \
    CSP/PKCS11/CardTemplate.cpp \
    CSP/PKCS11/initP11.cpp \
    CSP/PCSC/PCSC.cpp \
    CSP/PCSC/Token.cpp \
    CSP/PCSC/APDU.cpp \
    CSP/PCSC/CardLocker.cpp \
    CSP/CSP/IAS.cpp \
    CSP/CSP/ATR.cpp \
    CSP/CSP/ExtAuthKey.cpp \
    CSP/CSP/PINManager.cpp \
    CSP/Crypto/AES.cpp \
    CSP/Crypto/ASNParser.cpp \
    CSP/Crypto/Base64.cpp \
    CSP/Crypto/DES3.cpp \
    CSP/Crypto/MAC.cpp \
    CSP/Crypto/MD5.cpp \
    CSP/Crypto/RSA.cpp \
    CSP/Crypto/SHA1.cpp \
    CSP/Crypto/SHA256.cpp \
    CSP/Crypto/SHA512.cpp \
    CSP/LOGGER/Logger.cpp \
    CSP/Util/Array.cpp \
    CSP/Util/IniSettings.cpp \
    CSP/Util/ModuleInfo.cpp \
    CSP/Util/SyncroEvent.cpp \
    CSP/Util/SyncroMutex.cpp \
    CSP/Util/TLV.cpp \
    CSP/Util/Thread.cpp \
    CSP/Util/UtilException.cpp \
    CSP/Util/funccallinfo.cpp \
    CSP/Util/log.cpp \
    CSP/Util/util.cpp

OBJS = $(SRCS:.cpp=.o)

FIRMA_SRCS = $(SIGN_SDK_DIR)/src/FirmaConCIE.cpp $(SIGN_SDK_DIR)/src/CIESign.cpp $(SIGN_SDK_DIR)/src/FirmaStubs.cpp
FIRMA_OBJS = $(SIGN_SDK_DIR)/src/FirmaConCIE.o $(SIGN_SDK_DIR)/src/CIESign.o $(SIGN_SDK_DIR)/src/FirmaStubs.o

FIRMA_INCLUDES = \
    -I$(SIGN_SDK_DIR)/include \
    -I$(SIGN_SDK_DIR)/src \
    -I$(SIGN_SDK_DIR)/src/CSP \
    -I$(SIGN_SDK_DIR)/src/PCSC \
    -I$(SIGN_SDK_DIR)/src/Util \
    -I$(SIGN_SDK_DIR)/src/Crypto \
    -I$(SIGN_SDK_DIR)/src/ASN1 \
    -I$(SIGN_SDK_DIR)/src/cryptopp \
    -IPKCS11 \
    -I$(PREFIX_DIR)/include \
    -I$(OPENSSL_PREFIX)/include \
    -I$(LIBXML_PREFIX)/include/libxml2 \
    -I$(FREETYPE_PREFIX)/include/freetype2

FIRMA_LDFLAGS = -shared -framework PCSC -framework CoreFoundation -framework Security \
    -Wl,-force_load,$(SIGN_SDK_LIB) \
    -L$(PREFIX_DIR)/lib -lpodofo \
    -L$(OPENSSL_PREFIX)/lib -lssl -lcrypto \
    -L$(LIBXML_PREFIX)/lib -lxml2 \
    -L$(FREETYPE_PREFIX)/lib -lfreetype \
    -L$(PNG_PREFIX)/lib -lpng \
    -L$(FONTCONFIG_PREFIX)/lib -lfontconfig \
    -L$(CURL_PREFIX)/lib -lcurl \
    -L$(CRYPTOPP_PREFIX)/lib -lcryptopp \
    -L$(IDN_PREFIX)/lib -lidn \
    -L$(UNISTRING_PREFIX)/lib -lunistring \
    -liconv -lbz2 \
    -L$(JPEG_PREFIX)/lib -ljpeg \
    -L$(TIFF_PREFIX)/lib -ltiff \
    -lz

all: $(TARGET) $(FIRMA_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^
	install_name_tool -id @rpath/$@ $@

$(SIGN_SDK_LIB):
	@mkdir -p $(SIGN_SDK_DIR)/build
	cd $(SIGN_SDK_DIR)/build && cmake .. -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_OSX_ARCHITECTURES=arm64 \
		-DOPENSSL_ROOT_DIR=$(OPENSSL_PREFIX)
	cmake --build $(SIGN_SDK_DIR)/build -j$$(sysctl -n hw.ncpu)

$(SIGN_SDK_DIR)/src/%.o: $(SIGN_SDK_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) $(FIRMA_INCLUDES) -c $< -o $@

$(FIRMA_TARGET): $(SIGN_SDK_LIB) $(FIRMA_OBJS)
	$(CXX) $(FIRMA_LDFLAGS) -o $@ $(FIRMA_OBJS)
	install_name_tool -id @rpath/$@ $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

app: all
	bash CIEID-mac/build-app.sh

clean:
	rm -f $(OBJS) $(FIRMA_OBJS) $(TARGET) $(FIRMA_TARGET)
	rm -rf CIEID-mac/CIEID.app
	rm -rf $(SIGN_SDK_DIR)/build

.PHONY: all clean app
