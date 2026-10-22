/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dlopen-note.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Compression {
        COMPRESSION_NONE,
        COMPRESSION_XZ,
        COMPRESSION_LZ4,
        COMPRESSION_ZSTD,
        COMPRESSION_GZIP,
        COMPRESSION_BZIP2,
        _COMPRESSION_MAX,
        _COMPRESSION_INVALID = -EINVAL,
} Compression;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(compression, Compression);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(compression_uppercase, Compression);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(compression_extension, Compression);

/* Try the lowercase string table first, fall back to the uppercase one. Useful for parsing user input
 * where both forms (e.g. "xz" and "XZ") have historically been accepted. */
Compression compression_from_string_harder(ਅਚਲ ਅੱਖਰ *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ਅਚਲ ਅੱਖਰ *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPRESS_PIPE_ਬਫਰ_ਮਾਪ (128U*1024U)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPRESSION_MAGIC_ਬਾਈਟਾਂ_MAX 6U
Compression compression_detect_from_magic(ਅਚਲ uint8_t ਅੰਕੜੇ[ਸਥਿਰ COMPRESSION_MAGIC_ਬਾਈਟਾਂ_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Compressor Compressor;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ Compressor Decompressor;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*DecompressorCallback)(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ, ਖਾਲੀ *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

ਪੂਰਨ compressor_new(Compressor **ret, Compression type);
ਪੂਰਨ compressor_ਸ਼ੁਰੂ(Compressor *c, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ, ਖਾਲੀ **ਬਫਰ, size_t *ਬਫਰ_ਮਾਪ, size_t *ਬਫਰ_allocated);
ਪੂਰਨ compressor_ਅੰਤ(Compressor *c, ਖਾਲੀ **ਬਫਰ, size_t *ਬਫਰ_ਮਾਪ, size_t *ਬਫਰ_allocated);

ਪੂਰਨ decompressor_detect(Decompressor **decompressor, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ);
ਪੂਰਨ decompressor_force(Decompressor **decompressor, Compression type);
ਪੂਰਨ decompressor_force_off(Decompressor **decompressor);
ਪੂਰਨ decompressor_ਧੱਕੋ(Decompressor *c, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ, DecompressorCallback callback, ਖਾਲੀ *userdata);

Compression compressor_type(ਅਚਲ Compressor *c);

/* Blob compression/decompression */

ਪੂਰਨ compress_blob_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level);
ਪੂਰਨ compress_blob(Compression compression,
                  ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                  ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level);
ਪੂਰਨ decompress_blob_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst, size_t *dst_ਮਾਪ, size_t dst_max);
ਪੂਰਨ decompress_blob(Compression compression,
                    ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                    ਖਾਲੀ **dst, size_t *dst_ਮਾਪ, size_t dst_max);

ਪੂਰਨ decompress_blob_ਮੰਜ਼ਿਲ_fd_journal(
                Compression type,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ਮਾਪ);
ਪੂਰਨ decompress_blob_ਮੰਜ਼ਿਲ_fd(
                Compression type,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ਮਾਪ);

ਪੂਰਨ decompress_zlib_raw(ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                        ਖਾਲੀ *dst, size_t dst_ਮਾਪ, ਪੂਰਨ wbits);

ਪੂਰਨ decompress_startswith_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ, size_t ਅਗੇਤਰ_len,
                uint8_t extra);
ਪੂਰਨ decompress_startswith(Compression compression,
                          ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                          ਖਾਲੀ **ਬਫਰ,
                          ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ, size_t ਅਗੇਤਰ_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

ਪੂਰਨ compress_ਧਾਰਾ(Compression type, ਪੂਰਨ fdf, ਪੂਰਨ fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_ਮਾਪ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

ਪੂਰਨ decompress_ਧਾਰਾ_ਭਰਿਆ(Compression type, ਪੂਰਨ fdf, ਪੂਰਨ fdt, uint64_t max_bytes, DecompressFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ decompress_ਧਾਰਾ(Compression type, ਪੂਰਨ fdf, ਪੂਰਨ fdt, uint64_t max_bytes) {
        ਵਾਪਸ decompress_ਧਾਰਾ_ਭਰਿਆ(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
ਪੂਰਨ decompress_ਧਾਰਾ_by_filename(ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ fdf, ਪੂਰਨ fdt, uint64_t max_bytes);

ਪੂਰਨ dlopen_xz(ਪੂਰਨ log_level) _dlopen_loader_;
ਪੂਰਨ dlopen_lz4(ਪੂਰਨ log_level) _dlopen_loader_;
ਪੂਰਨ dlopen_zstd(ਪੂਰਨ log_level) _dlopen_loader_;
ਪੂਰਨ dlopen_zlib(ਪੂਰਨ log_level) _dlopen_loader_;
ਪੂਰਨ dlopen_bzip2(ਪੂਰਨ log_level) _dlopen_loader_;

ਪੂਰਨ dlopen_compress(Compression c, ਪੂਰਨ log_level);
ਪੂਰਨ dlopen_compress_journal(Compression c, ਪੂਰਨ log_level);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ dlopen_compress_default(ਪੂਰਨ log_level) {
        ਵਾਪਸ dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* default_compression_extension(ਖਾਲੀ) {
        ਵਾਪਸ compression_extension_ਮੰਜ਼ਿਲ_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_ਬਾਹਰਲਾ_ਮਾਪ(ਖਾਲੀ);
