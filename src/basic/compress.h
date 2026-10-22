/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "dlopen-note.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Compression {
        COMPRESSION_NONE,
        COMPRESSION_XZ,
        COMPRESSION_LZ4,
        COMPRESSION_ZSTD,
        COMPRESSION_GZIP,
        COMPRESSION_BZIP2,
        _COMPRESSION_MAX,
        _COMPRESSION_INVALID = -EINVAL,
} Compression;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression, Compression);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression_uppercase, Compression);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression_extension, Compression);

/* Try the lowercase string table first, fall back to the uppercase one. Useful for parsing user input
 * where both forms (e.g. "xz" and "XZ") have historically been accepted. */
Compression compression_from_string_harder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#ವ್ಯಾಖ್ಯಾನಿಸು COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ (128U*1024U)

#ವ್ಯಾಖ್ಯಾನಿಸು COMPRESSION_MAGIC_ಬೈಟುಗಳು_MAX 6U
Compression compression_detect_from_magic(ಸ್ಥಿರಾಂಕ uint8_t ದತ್ತಾಂಶ[ಸ್ಥಿರ COMPRESSION_MAGIC_ಬೈಟುಗಳು_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Compressor Compressor;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Compressor Decompressor;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*DecompressorCallback)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

ಪೂರ್ಣಾಂಕ compressor_new(Compressor **ret, Compression type);
ಪೂರ್ಣಾಂಕ compressor_ಆರಂಭ(Compressor *c, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ **ಬಫರ್, size_t *ಬಫರ್_ಗಾತ್ರ, size_t *ಬಫರ್_allocated);
ಪೂರ್ಣಾಂಕ compressor_ಮುಕ್ತಾಯ(Compressor *c, ಶೂನ್ಯ **ಬಫರ್, size_t *ಬಫರ್_ಗಾತ್ರ, size_t *ಬಫರ್_allocated);

ಪೂರ್ಣಾಂಕ decompressor_detect(Decompressor **decompressor, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2);
ಪೂರ್ಣಾಂಕ decompressor_force(Decompressor **decompressor, Compression type);
ಪೂರ್ಣಾಂಕ decompressor_force_off(Decompressor **decompressor);
ಪೂರ್ಣಾಂಕ decompressor_ತಳ್ಳು(Decompressor *c, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, DecompressorCallback callback, ಶೂನ್ಯ *userdata);

Compression compressor_type(ಸ್ಥಿರಾಂಕ Compressor *c);

/* Blob compression/decompression */

ಪೂರ್ಣಾಂಕ compress_blob_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level);
ಪೂರ್ಣಾಂಕ compress_blob(Compression compression,
                  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                  ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level);
ಪೂರ್ಣಾಂಕ decompress_blob_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst, size_t *dst_ಗಾತ್ರ, size_t dst_max);
ಪೂರ್ಣಾಂಕ decompress_blob(Compression compression,
                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                    ಶೂನ್ಯ **dst, size_t *dst_ಗಾತ್ರ, size_t dst_max);

ಪೂರ್ಣಾಂಕ decompress_blob_ಗಮ್ಯ_fd_journal(
                Compression type,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ಗಾತ್ರ);
ಪೂರ್ಣಾಂಕ decompress_blob_ಗಮ್ಯ_fd(
                Compression type,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ಗಾತ್ರ);

ಪೂರ್ಣಾಂಕ decompress_zlib_raw(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                        ಶೂನ್ಯ *dst, size_t dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ wbits);

ಪೂರ್ಣಾಂಕ decompress_startswith_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ, size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra);
ಪೂರ್ಣಾಂಕ decompress_startswith(Compression compression,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                          ಶೂನ್ಯ **ಬಫರ್,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ, size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

ಪೂರ್ಣಾಂಕ compress_ಹರಿವು(Compression type, ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_ಗಾತ್ರ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು_ತುಂಬಿದ(Compression type, ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt, uint64_t max_bytes, DecompressFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು(Compression type, ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt, uint64_t max_bytes) {
        ಹಿಂತಿರುಗಿ decompress_ಹರಿವು_ತುಂಬಿದ(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು_by_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt, uint64_t max_bytes);

ಪೂರ್ಣಾಂಕ dlopen_xz(ಪೂರ್ಣಾಂಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಂಕ dlopen_lz4(ಪೂರ್ಣಾಂಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಂಕ dlopen_zstd(ಪೂರ್ಣಾಂಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಂಕ dlopen_zlib(ಪೂರ್ಣಾಂಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಂಕ dlopen_bzip2(ಪೂರ್ಣಾಂಕ log_level) _dlopen_loader_;

ಪೂರ್ಣಾಂಕ dlopen_compress(Compression c, ಪೂರ್ಣಾಂಕ log_level);
ಪೂರ್ಣಾಂಕ dlopen_compress_journal(Compression c, ಪೂರ್ಣಾಂಕ log_level);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ dlopen_compress_default(ಪೂರ್ಣಾಂಕ log_level) {
        ಹಿಂತಿರುಗಿ dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_compression_extension(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ compression_extension_ಗಮ್ಯ_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_ಹೊರಹೋಗು_ಗಾತ್ರ(ಶೂನ್ಯ);
