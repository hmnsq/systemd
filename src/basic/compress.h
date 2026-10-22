/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "dlopen-note.h"

قسمتعریف شمار Compression {
        COMPRESSION_NONE,
        COMPRESSION_XZ,
        COMPRESSION_LZ4,
        COMPRESSION_ZSTD,
        COMPRESSION_GZIP,
        COMPRESSION_BZIP2,
        _COMPRESSION_MAX,
        _COMPRESSION_INVALID = -EINVAL,
} Compression;

DECLARE_STRING_جدول_LOOKUP(compression, Compression);
DECLARE_STRING_جدول_LOOKUP(compression_uppercase, Compression);
DECLARE_STRING_جدول_LOOKUP(compression_extension, Compression);

/* Try the lowercase string table first, fall back to the uppercase one. Useful for parsing user input
 * where both forms (e.g. "xz" and "XZ") have historically been accepted. */
Compression compression_from_string_harder(ثابت حرف *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ثابت حرف *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#تعریف_کرو COMPRESS_PIPE_بفر_سائز (128U*1024U)

#تعریف_کرو COMPRESSION_MAGIC_بائٹیں_MAX 6U
Compression compression_detect_from_magic(ثابت uint8_t ڈیٹا[ساکن COMPRESSION_MAGIC_بائٹیں_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

قسمتعریف ساخت Compressor Compressor;
قسمتعریف Compressor Decompressor;

قسمتعریف صحیح (*DecompressorCallback)(ثابت خالی *ڈیٹا, size_t سائز, خالی *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

صحیح compressor_new(Compressor **ret, Compression type);
صحیح compressor_آغاز(Compressor *c, ثابت خالی *ڈیٹا, size_t سائز, خالی **بفر, size_t *بفر_سائز, size_t *بفر_allocated);
صحیح compressor_اختتام(Compressor *c, خالی **بفر, size_t *بفر_سائز, size_t *بفر_allocated);

صحیح decompressor_detect(Decompressor **decompressor, ثابت خالی *ڈیٹا, size_t سائز);
صحیح decompressor_force(Decompressor **decompressor, Compression type);
صحیح decompressor_force_off(Decompressor **decompressor);
صحیح decompressor_ڈالو(Decompressor *c, ثابت خالی *ڈیٹا, size_t سائز, DecompressorCallback callback, خالی *userdata);

Compression compressor_type(ثابت Compressor *c);

/* Blob compression/decompression */

صحیح compress_blob_journal(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level);
صحیح compress_blob(Compression compression,
                  ثابت خالی *src, uint64_t src_سائز,
                  خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level);
صحیح decompress_blob_journal(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی **dst, size_t *dst_سائز, size_t dst_max);
صحیح decompress_blob(Compression compression,
                    ثابت خالی *src, uint64_t src_سائز,
                    خالی **dst, size_t *dst_سائز, size_t dst_max);

صحیح decompress_blob_مقصد_fd_journal(
                Compression type,
                ثابت خالی *src,
                uint64_t src_سائز,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_سائز);
صحیح decompress_blob_مقصد_fd(
                Compression type,
                ثابت خالی *src,
                uint64_t src_سائز,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_سائز);

صحیح decompress_zlib_raw(ثابت خالی *src, uint64_t src_سائز,
                        خالی *dst, size_t dst_سائز, صحیح wbits);

صحیح decompress_startswith_journal(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ, size_t سابقہ_len,
                uint8_t extra);
صحیح decompress_startswith(Compression compression,
                          ثابت خالی *src, uint64_t src_سائز,
                          خالی **بفر,
                          ثابت خالی *سابقہ, size_t سابقہ_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

صحیح compress_دھارا(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_سائز);
قسمتعریف شمار DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

صحیح decompress_دھارا_بھرا(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes, DecompressFlags flags);
ساکن اندرونی صحیح decompress_دھارا(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes) {
        واپس decompress_دھارا_بھرا(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
صحیح decompress_دھارا_by_filename(ثابت حرف *filename, صحیح fdf, صحیح fdt, uint64_t max_bytes);

صحیح dlopen_xz(صحیح log_level) _dlopen_loader_;
صحیح dlopen_lz4(صحیح log_level) _dlopen_loader_;
صحیح dlopen_zstd(صحیح log_level) _dlopen_loader_;
صحیح dlopen_zlib(صحیح log_level) _dlopen_loader_;
صحیح dlopen_bzip2(صحیح log_level) _dlopen_loader_;

صحیح dlopen_compress(Compression c, صحیح log_level);
صحیح dlopen_compress_journal(Compression c, صحیح log_level);
ساکن اندرونی صحیح dlopen_compress_default(صحیح log_level) {
        واپس dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ساکن اندرونی ثابت حرف* default_compression_extension(خالی) {
        واپس compression_extension_مقصد_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_برآمد_سائز(خالی);
