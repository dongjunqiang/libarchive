/*-
* Copyright (c) 2014 Michihiro NAKAJIMA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __LIBARCHIVE_BUILD
#error This header is only to be used internally to libarchive.
#endif

#ifndef ARCHIVE_CRYPTOR_PRIVATE_H_INCLUDED
#define ARCHIVE_CRYPTOR_PRIVATE_H_INCLUDED

#ifdef __APPLE__
#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#define AES_BLOCK_SIZE	16
#define AES_MAX_KEY_SIZE kCCKeySizeAES256

typedef struct {
	CCCryptorRef	ctx;
	uint8_t		key[AES_MAX_KEY_SIZE];
	unsigned	key_len;
	uint8_t		nonce[AES_BLOCK_SIZE];
	uint8_t		encr_buf[AES_BLOCK_SIZE];
	unsigned	encr_pos;
} archive_crypto_ctx;

#elif defined(HAVE_LIBNETTLE)
#include <nettle/pbkdf2.h>
#include <nettle/aes.h>

typedef struct {
	struct aes_ctx	ctx;
	uint8_t		key[AES_MAX_KEY_SIZE];
	unsigned	key_len;
	uint8_t		nonce[AES_BLOCK_SIZE];
	uint8_t		encr_buf[AES_BLOCK_SIZE];
	unsigned	encr_pos;
} archive_crypto_ctx;

#elif defined(HAVE_LIBCRYPTO)
#include <openssl/evp.h>
#define AES_BLOCK_SIZE	16
#define AES_MAX_KEY_SIZE 32

typedef struct {
	EVP_CIPHER_CTX	ctx;
	const EVP_CIPHER *type;
	uint8_t		key[AES_MAX_KEY_SIZE];
	unsigned	key_len;
	uint8_t		nonce[AES_BLOCK_SIZE];
	uint8_t		encr_buf[AES_BLOCK_SIZE];
	unsigned	encr_pos;
} archive_crypto_ctx;

#endif

/* defines */
#define archive_pbkdf2_sha1(pw, pw_len, salt, salt_len, rounds, dk, dk_len)\
  __archive_cryptor.pbkdf2sha1(pw, pw_len, salt, salt_len, rounds, dk, dk_len)

#define archive_decrypto_aes_ctr_init(ctx, key, key_len) \
  __archive_cryptor.decrypto_aes_ctr_init(ctx, key, key_len)
#define archive_decrypto_aes_ctr_update(ctx, in, in_len, out, out_len) \
  __archive_cryptor.decrypto_aes_ctr_update(ctx, in, in_len, out, out_len)
#define archive_decrypto_aes_ctr_release(ctx) \
  __archive_cryptor.decrypto_aes_ctr_release(ctx)

#define archive_encrypto_aes_ctr_init(ctx, key, key_len) \
  __archive_cryptor.encrypto_aes_ctr_init(ctx, key, key_len)
#define archive_encrypto_aes_ctr_update(ctx, in, in_len, out, out_len) \
  __archive_cryptor.encrypto_aes_ctr_update(ctx, in, in_len, out, out_len)
#define archive_encrypto_aes_ctr_release(ctx) \
  __archive_cryptor.encrypto_aes_ctr_release(ctx)

/* Minimal interface to cryptographic functionality for internal use in
 * libarchive */
struct archive_cryptor
{
  /* PKCS5 PBKDF2 HMAC-SHA1 */
  int (*pbkdf2sha1)(const char *pw, size_t pw_len, const uint8_t *salt,
    size_t salt_len, unsigned rounds, uint8_t *derived_key,
    size_t derived_key_len);
  /* AES CTR mode(little endian version) */
  int (*decrypto_aes_ctr_init)(archive_crypto_ctx *, const uint8_t *, size_t);
  int (*decrypto_aes_ctr_update)(archive_crypto_ctx *, const uint8_t *,
    size_t, uint8_t *, size_t *);
  int (*decrypto_aes_ctr_release)(archive_crypto_ctx *);
  int (*encrypto_aes_ctr_init)(archive_crypto_ctx *, const uint8_t *, size_t);
  int (*encrypto_aes_ctr_update)(archive_crypto_ctx *, const uint8_t *,
    size_t, uint8_t *, size_t *);
  int (*encrypto_aes_ctr_release)(archive_crypto_ctx *);
};

extern const struct archive_cryptor __archive_cryptor;

#endif
