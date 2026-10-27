#include "VFS_Precompile.h"

#include "PhoenixProtectedFile.h"

#include <algorithm>
#include <bcrypt.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <windows.h>

#if defined(__has_include)
#  if __has_include(<zlib.h>)
#    include <zlib.h>
#  else
#    include "MZLib/zlib.h"
#  endif
#else
#  include <zlib.h>
#endif

namespace
{
	const unsigned char kPhoenixMagic[8] = { 0xC7, 0x31, 0x9E, 0x4A, 0x11, 0xB8, 0x52, 0xD6 };
	const uint32_t kPhoenixVersion = 2;
	const uint32_t kFlagCompressed = 0x00000001;
	const uint32_t kMaxProtectedSize = 256u * 1024u * 1024u;
	const size_t kAesKeySize = 32;
	const size_t kSaltSize = 16;
	const size_t kNonceSize = 12;
	const size_t kTagSize = 16;

#pragma pack(push, 1)
	struct PhoenixProtectedAad
	{
		unsigned char Magic[8];
		uint32_t Version;
		uint32_t HeaderSize;
		uint32_t Flags;
		uint32_t RawSize;
		uint32_t PayloadSize;
		uint32_t PathHash;
		unsigned char Salt[16];
		unsigned char Nonce[12];
		uint32_t RawCrc32;
	};

	struct PhoenixProtectedHeader
	{
		PhoenixProtectedAad Aad;
		unsigned char Tag[16];
	};
#pragma pack(pop)

	struct BCryptAlgHandle
	{
		BCRYPT_ALG_HANDLE Handle;
		BCryptAlgHandle() : Handle(NULL) {}
		~BCryptAlgHandle() { if(Handle) BCryptCloseAlgorithmProvider(Handle, 0); }
	};

	struct BCryptKeyHandle
	{
		BCRYPT_KEY_HANDLE Handle;
		BCryptKeyHandle() : Handle(NULL) {}
		~BCryptKeyHandle() { if(Handle) BCryptDestroyKey(Handle); }
	};

	bool NtOk(NTSTATUS status)
	{
		return status >= 0;
	}

	std::string NormalizePathSlashesAndCase(const char* path)
	{
		std::string result;
		if(!path)
			return result;

		bool lastWasSlash = false;
		for(const char* p = path; *p; ++p)
		{
			char c = *p;
			const bool isSlash = c == '/' || c == '\\';
			if(isSlash && lastWasSlash)
				continue;

			if(isSlash)
			{
				result.push_back('/');
				lastWasSlash = true;
			}
			else
			{
				if(c >= 'A' && c <= 'Z')
					c = static_cast<char>(c - 'A' + 'a');
				result.push_back(c);
				lastWasSlash = false;
			}
		}

		return result;
	}

	void StripProtectedExtension(std::string& result)
	{
		const char* ext = PhoenixProtectedFile::Extension();
		const size_t extLen = strlen(ext);
		if(result.size() > extLen &&
		   result.compare(result.size() - extLen, extLen, ext) == 0)
		{
			result.resize(result.size() - extLen);
		}
	}

	void StripPathPrefix(std::string& path, const std::string& prefix)
	{
		if(path.empty() || prefix.empty())
			return;

		if(path == prefix)
		{
			path.clear();
			return;
		}

		std::string root = prefix;
		if(!root.empty() && root[root.size() - 1] != '/')
			root += '/';

		if(path.rfind(root, 0) == 0)
			path.erase(0, root.size());
	}

	void StripKnownRuntimeRoot(std::string& path)
	{
		char currentDirectory[MAX_PATH] = {0};
		if(GetCurrentDirectoryA(MAX_PATH, currentDirectory) > 0)
			StripPathPrefix(path, NormalizePathSlashesAndCase(currentDirectory));

		const std::string gameDataMarker = "/gamedata/";
		const size_t gameDataPos = path.find(gameDataMarker);
		if(gameDataPos != std::string::npos)
			path.erase(0, gameDataPos + gameDataMarker.size());

		while(path.rfind("./", 0) == 0)
			path.erase(0, 2);
	}

	std::string NormalizePath(const char* path)
	{
		std::string result = NormalizePathSlashesAndCase(path);
		StripProtectedExtension(result);
		StripKnownRuntimeRoot(result);

		return result;
	}

	uint32_t HashPath(const char* path)
	{
		const std::string normalized = NormalizePath(path);
		uint32_t hash = 2166136261u;
		for(size_t i = 0; i < normalized.size(); ++i)
		{
			hash ^= static_cast<unsigned char>(normalized[i]);
			hash *= 16777619u;
		}
		return hash;
	}

	void BuildBaseSecret(unsigned char out[kAesKeySize])
	{
		static const unsigned char a[kAesKeySize] = {
			0x8D, 0x2F, 0x64, 0xA7, 0x19, 0xCE, 0x43, 0x55,
			0xD1, 0x7B, 0x30, 0xE8, 0x92, 0x0C, 0xFA, 0x6E,
			0x48, 0xB5, 0x01, 0x3D, 0xC6, 0x7A, 0xEF, 0x20,
			0x13, 0x9B, 0xDA, 0x57, 0x84, 0x3A, 0x6C, 0xF1
		};
		static const unsigned char b[kAesKeySize] = {
			0x24, 0x91, 0xE0, 0x0A, 0x73, 0x5F, 0xC8, 0x1D,
			0x67, 0xBA, 0x0E, 0x44, 0x28, 0xD2, 0x35, 0x9C,
			0xFE, 0x10, 0xAB, 0x76, 0x49, 0x81, 0x03, 0xCD,
			0x5E, 0xB7, 0x22, 0xF9, 0x31, 0xC4, 0x08, 0xA6
		};
		static const unsigned char c[kAesKeySize] = {
			0xC1, 0x73, 0x9A, 0x58, 0x2D, 0xB0, 0x1E, 0xF4,
			0xA5, 0x06, 0xD9, 0x3C, 0xE7, 0x41, 0x88, 0x12,
			0x6B, 0xCF, 0x37, 0xA0, 0x15, 0xEE, 0x54, 0x79,
			0xB2, 0x04, 0x6D, 0x8F, 0xDA, 0x23, 0x5C, 0x90
		};

		for(size_t i = 0; i < kAesKeySize; ++i)
			out[i] = static_cast<unsigned char>((a[i] ^ b[(i + 11) & 31] ^ c[(i + 23) & 31]) + (i * 17));
	}

	bool RandomBytes(unsigned char* data, size_t size)
	{
		if(size == 0)
			return true;

		if(NtOk(BCryptGenRandom(NULL, data, static_cast<ULONG>(size), BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
			return true;

		uint32_t state = GetTickCount() ^ 0xD47C5A19u;
		for(size_t i = 0; i < size; ++i)
		{
			state = state * 1664525u + 1013904223u;
			data[i] = static_cast<unsigned char>((state >> 16) & 0xFF);
		}
		return true;
	}

	bool Sha256(const unsigned char* data, size_t size, unsigned char out[kAesKeySize])
	{
		BCryptAlgHandle alg;
		if(!NtOk(BCryptOpenAlgorithmProvider(&alg.Handle, BCRYPT_SHA256_ALGORITHM, NULL, 0)))
			return false;

		DWORD objectLength = 0;
		DWORD resultLength = 0;
		if(!NtOk(BCryptGetProperty(alg.Handle, BCRYPT_OBJECT_LENGTH,
			reinterpret_cast<PUCHAR>(&objectLength), sizeof(objectLength), &resultLength, 0)))
		{
			return false;
		}

		std::vector<unsigned char> objectBuffer(objectLength);
		BCRYPT_HASH_HANDLE hash = NULL;
		if(!NtOk(BCryptCreateHash(alg.Handle, &hash, objectBuffer.empty() ? NULL : &objectBuffer[0], objectLength, NULL, 0, 0)))
			return false;

		bool ok = NtOk(BCryptHashData(hash, const_cast<PUCHAR>(data), static_cast<ULONG>(size), 0)) &&
				  NtOk(BCryptFinishHash(hash, out, static_cast<ULONG>(kAesKeySize), 0));
		BCryptDestroyHash(hash);
		return ok;
	}

	bool DeriveKey(const char* originalPath, const unsigned char salt[kSaltSize], unsigned char outKey[kAesKeySize])
	{
		unsigned char baseSecret[kAesKeySize];
		BuildBaseSecret(baseSecret);

		const std::string normalized = NormalizePath(originalPath);
		std::vector<unsigned char> material;
		material.insert(material.end(), baseSecret, baseSecret + kAesKeySize);
		material.insert(material.end(), normalized.begin(), normalized.end());
		material.insert(material.end(), salt, salt + kSaltSize);
		const unsigned char domain[] = { 0x66, 0x31, 0xA0, 0x5D, 0x99, 0x02, 0xC8, 0x7E };
		material.insert(material.end(), domain, domain + sizeof(domain));

		return Sha256(material.empty() ? NULL : &material[0], material.size(), outKey);
	}

	bool OpenAesGcm(BCryptAlgHandle& alg)
	{
		if(!NtOk(BCryptOpenAlgorithmProvider(&alg.Handle, BCRYPT_AES_ALGORITHM, NULL, 0)))
			return false;

		return NtOk(BCryptSetProperty(alg.Handle, BCRYPT_CHAINING_MODE,
			reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(BCRYPT_CHAIN_MODE_GCM)),
			static_cast<ULONG>((wcslen(BCRYPT_CHAIN_MODE_GCM) + 1) * sizeof(wchar_t)), 0));
	}

	bool GenerateAesKey(BCryptAlgHandle& alg, const unsigned char key[kAesKeySize],
		BCryptKeyHandle& keyHandle, std::vector<unsigned char>& objectBuffer)
	{
		DWORD objectLength = 0;
		DWORD resultLength = 0;
		if(!NtOk(BCryptGetProperty(alg.Handle, BCRYPT_OBJECT_LENGTH,
			reinterpret_cast<PUCHAR>(&objectLength), sizeof(objectLength), &resultLength, 0)))
		{
			return false;
		}

		objectBuffer.resize(objectLength);
		return NtOk(BCryptGenerateSymmetricKey(alg.Handle, &keyHandle.Handle,
			objectBuffer.empty() ? NULL : &objectBuffer[0], objectLength,
			const_cast<PUCHAR>(key), static_cast<ULONG>(kAesKeySize), 0));
	}

	bool AesGcmCrypt(bool encrypt, const unsigned char key[kAesKeySize],
		const PhoenixProtectedAad& aad,
		const unsigned char* input, size_t inputSize,
		std::vector<char>& output,
		unsigned char tag[kTagSize])
	{
		BCryptAlgHandle alg;
		if(!OpenAesGcm(alg))
			return false;

		BCryptKeyHandle keyHandle;
		std::vector<unsigned char> keyObject;
		if(!GenerateAesKey(alg, key, keyHandle, keyObject))
			return false;

		BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
		BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
		authInfo.pbNonce = const_cast<PUCHAR>(aad.Nonce);
		authInfo.cbNonce = static_cast<ULONG>(kNonceSize);
		authInfo.pbAuthData = reinterpret_cast<PUCHAR>(const_cast<PhoenixProtectedAad*>(&aad));
		authInfo.cbAuthData = sizeof(PhoenixProtectedAad);
		authInfo.pbTag = tag;
		authInfo.cbTag = static_cast<ULONG>(kTagSize);

		output.resize(inputSize);
		ULONG resultSize = 0;
		PUCHAR outData = output.empty() ? NULL : reinterpret_cast<PUCHAR>(&output[0]);
		PUCHAR inData = const_cast<PUCHAR>(input);

		NTSTATUS status = encrypt
			? BCryptEncrypt(keyHandle.Handle, inData, static_cast<ULONG>(inputSize), &authInfo,
				NULL, 0, outData, static_cast<ULONG>(output.size()), &resultSize, 0)
			: BCryptDecrypt(keyHandle.Handle, inData, static_cast<ULONG>(inputSize), &authInfo,
				NULL, 0, outData, static_cast<ULONG>(output.size()), &resultSize, 0);

		if(!NtOk(status) || resultSize != inputSize)
		{
			output.clear();
			return false;
		}

		return true;
	}

	bool ReadAll(const char* path, std::vector<char>& out)
	{
		std::ifstream file(path, std::ios::binary);
		if(!file.is_open())
			return false;

		file.seekg(0, std::ios::end);
		const std::streamoff size = file.tellg();
		if(size < 0 || static_cast<uint64_t>(size) > kMaxProtectedSize)
			return false;

		file.seekg(0, std::ios::beg);
		out.resize(static_cast<size_t>(size));
		if(!out.empty())
			file.read(&out[0], static_cast<std::streamsize>(out.size()));

		return file.good() || file.eof();
	}
}

namespace PhoenixProtectedFile
{
	const char* Extension()
	{
		return ".phx";
	}

	bool IsInfPath(const char* path)
	{
		if(!path)
			return false;

		std::string normalized = NormalizePath(path);
		const char* infExt = ".inf";
		const size_t extLen = strlen(infExt);
		return normalized.size() > extLen &&
			   normalized.compare(normalized.size() - extLen, extLen, infExt) == 0;
	}

	bool BuildProtectedPath(const char* sourcePath, std::string& protectedPath)
	{
		if(!sourcePath || !IsInfPath(sourcePath))
			return false;

		protectedPath = sourcePath;
		protectedPath += Extension();
		return true;
	}

	bool Load(const char* protectedPath, std::vector<char>& outData)
	{
		return LoadWithOriginalPath(protectedPath, protectedPath, outData);
	}

	bool LoadWithOriginalPath(const char* protectedPath, const char* originalPath, std::vector<char>& outData)
	{
		outData.clear();

		std::vector<char> fileData;
		if(!ReadAll(protectedPath, fileData))
			return false;

		if(fileData.size() < sizeof(PhoenixProtectedHeader))
			return false;

		PhoenixProtectedHeader header;
		memcpy(&header, &fileData[0], sizeof(header));

		if(memcmp(header.Aad.Magic, kPhoenixMagic, sizeof(kPhoenixMagic)) != 0 ||
		   header.Aad.Version != kPhoenixVersion ||
		   header.Aad.HeaderSize != sizeof(PhoenixProtectedHeader) ||
		   header.Aad.RawSize > kMaxProtectedSize ||
		   header.Aad.PayloadSize > kMaxProtectedSize ||
		   fileData.size() != sizeof(PhoenixProtectedHeader) + header.Aad.PayloadSize)
		{
			return false;
		}

		if(header.Aad.PathHash != HashPath(originalPath))
			return false;

		unsigned char key[kAesKeySize];
		if(!DeriveKey(originalPath, header.Aad.Salt, key))
			return false;

		std::vector<char> decrypted;
		const unsigned char* payload = reinterpret_cast<const unsigned char*>(&fileData[sizeof(PhoenixProtectedHeader)]);
		unsigned char tag[kTagSize];
		memcpy(tag, header.Tag, kTagSize);
		if(!AesGcmCrypt(false, key, header.Aad, payload, header.Aad.PayloadSize, decrypted, tag))
			return false;

		// Ciphertext artik gerekmez — peak ~3x yerine ~2x (zone/.inf OOM baskisi).
		{
			std::vector<char> drop;
			fileData.swap(drop);
		}

		if(header.Aad.Flags & kFlagCompressed)
		{
			outData.resize(header.Aad.RawSize);
			uLongf rawSize = header.Aad.RawSize;
			const int result = uncompress(
				reinterpret_cast<Bytef*>(outData.empty() ? NULL : &outData[0]),
				&rawSize,
				reinterpret_cast<const Bytef*>(decrypted.empty() ? NULL : &decrypted[0]),
				header.Aad.PayloadSize);

			{
				std::vector<char> drop;
				decrypted.swap(drop);
			}

			if(result != Z_OK || rawSize != header.Aad.RawSize)
			{
				outData.clear();
				return false;
			}
		}
		else if(header.Aad.RawSize > 0)
		{
			if(decrypted.size() != header.Aad.RawSize)
			{
				outData.clear();
				return false;
			}
			// Kopya yok — decrypt buffer'i dogrudan ciktiya tasir.
			outData.swap(decrypted);
		}

		const uint32_t crc = static_cast<uint32_t>(
			crc32(0, reinterpret_cast<const Bytef*>(outData.empty() ? NULL : &outData[0]), header.Aad.RawSize));
		if(crc != header.Aad.RawCrc32)
		{
			outData.clear();
			return false;
		}

		return true;
	}

	bool Save(const char* originalPath, const char* data, size_t dataSize, const char* protectedPath)
	{
		if(!originalPath || !protectedPath || (!data && dataSize > 0) || dataSize > kMaxProtectedSize)
			return false;

		std::vector<char> payload;
		uint32_t flags = 0;

		const uLongf bound = compressBound(static_cast<uLong>(dataSize));
		std::vector<char> compressed(static_cast<size_t>(bound));
		uLongf compressedSize = bound;

		const int compressResult = compress2(
			reinterpret_cast<Bytef*>(compressed.empty() ? NULL : &compressed[0]),
			&compressedSize,
			reinterpret_cast<const Bytef*>(data),
			static_cast<uLong>(dataSize),
			Z_DEFAULT_COMPRESSION);

		if(compressResult == Z_OK && compressedSize < dataSize)
		{
			flags |= kFlagCompressed;
			payload.assign(compressed.begin(), compressed.begin() + compressedSize);
		}
		else
		{
			payload.assign(data, data + dataSize);
		}

		PhoenixProtectedHeader header;
		memset(&header, 0, sizeof(header));
		memcpy(header.Aad.Magic, kPhoenixMagic, sizeof(kPhoenixMagic));
		header.Aad.Version = kPhoenixVersion;
		header.Aad.HeaderSize = sizeof(PhoenixProtectedHeader);
		header.Aad.Flags = flags;
		header.Aad.RawSize = static_cast<uint32_t>(dataSize);
		header.Aad.PayloadSize = static_cast<uint32_t>(payload.size());
		header.Aad.PathHash = HashPath(originalPath);
		RandomBytes(header.Aad.Salt, kSaltSize);
		RandomBytes(header.Aad.Nonce, kNonceSize);
		header.Aad.RawCrc32 = static_cast<uint32_t>(
			crc32(0, reinterpret_cast<const Bytef*>(data), static_cast<uInt>(dataSize)));

		unsigned char key[kAesKeySize];
		if(!DeriveKey(originalPath, header.Aad.Salt, key))
			return false;

		std::vector<char> encrypted;
		if(!AesGcmCrypt(true, key, header.Aad,
			reinterpret_cast<const unsigned char*>(payload.empty() ? NULL : &payload[0]),
			payload.size(), encrypted, header.Tag))
		{
			return false;
		}

		std::ofstream out(protectedPath, std::ios::binary);
		if(!out.is_open())
			return false;

		out.write(reinterpret_cast<const char*>(&header), sizeof(header));
		if(!encrypted.empty())
			out.write(&encrypted[0], static_cast<std::streamsize>(encrypted.size()));

		return out.good();
	}
}
