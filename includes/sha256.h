#ifndef SHA256_H
#define SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct sha256_buff {
  uint64_t data_size;
  uint32_t h[8];
  uint8_t last_chunk[64];
  uint8_t chunk_size;
};

/* Initialization, must be called before any further use */
void sha256_init(struct sha256_buff *buff);

/* Process block of data of arbitary length, can be used on data streams (files,
 * etc) */
void sha256_update(struct sha256_buff *buff, const void *data, size_t size);

/* Produces final hash values (digest) to be read
   If the buffer is reused later, init must be called again */
void sha256_finalize(struct sha256_buff *buff);

/* Read digest into 32-byte binary array */
void sha256_read(const struct sha256_buff *buff, uint8_t *hash);

/* Read digest into 64-char string as hex (without null-byte) */
void sha256_read_hex(const struct sha256_buff *buff, char *hex);

/* Hashes single contiguous block of data and reads digest into 32-byte binary
 * array */
void sha256_easy_hash(const void *data, size_t size, uint8_t *hash);

/* Hashes single contiguous block of data and reads digest into 64-char string
 * (without null-byte) */
void sha256_easy_hash_hex(const void *data, size_t size, char *hex);

#ifdef __cplusplus
}

#include <string>

class SHA256 {
private:
  struct sha256_buff buff;

public:
  SHA256() { sha256_init(&buff); }

  void update(const void *data, std::size_t size) {
    sha256_update(&buff, data, size);
  }

  std::string hash() {
    char hash[64];
    sha256_finalize(&buff);
    sha256_read_hex(&buff, hash);
    sha256_init(&buff);
    return std::string(hash, 64);
  }

  static std::string hashString(const std::string &str) {
    char hash[64];
    sha256_easy_hash_hex(str.c_str(), str.length(), hash);
    return std::string(hash, 64);
  }
};

#endif

#endif
