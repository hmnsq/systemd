/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "dlopen-note.h"

تعريفنوع تعداد Compression {
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
#تعریف_کرو COMPRESS_PIPE_بفر_ناپ (128U*1024U)

#تعریف_کرو COMPRESSION_MAGIC_بائٹاں_MAX 6U
Compression compression_detect_from_magic(ثابت uint8_t انکڑے[ساكن COMPRESSION_MAGIC_بائٹاں_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

تعريفنوع بنية Compressor Compressor;
تعريفنوع Compressor Decompressor;

تعريفنوع صحيح (*DecompressorCallback)(ثابت فراغ *انکڑے, size_t ناپ, فراغ *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

صحيح compressor_new(Compressor **ret, Compression type);
صحيح compressor_شروع(Compressor *c, ثابت فراغ *انکڑے, size_t ناپ, فراغ **بفر, size_t *بفر_ناپ, size_t *بفر_allocated);
صحيح compressor_انت(Compressor *c, فراغ **بفر, size_t *بفر_ناپ, size_t *بفر_allocated);

صحيح decompressor_detect(Decompressor **decompressor, ثابت فراغ *انکڑے, size_t ناپ);
صحيح decompressor_force(Decompressor **decompressor, Compression type);
صحيح decompressor_force_off(Decompressor **decompressor);
صحيح decompressor_ڈالو(Decompressor *c, ثابت فراغ *انکڑے, size_t ناپ, DecompressorCallback callback, فراغ *userdata);

Compression compressor_type(ثابت Compressor *c);

/* Blob compression/decompression */

صحيح compress_blob_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level);
صحيح compress_blob(Compression compression,
                  ثابت فراغ *src, uint64_t src_ناپ,
                  فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level);
صحيح decompress_blob_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ **dst, size_t *dst_ناپ, size_t dst_max);
صحيح decompress_blob(Compression compression,
                    ثابت فراغ *src, uint64_t src_ناپ,
                    فراغ **dst, size_t *dst_ناپ, size_t dst_max);

صحيح decompress_blob_منزل_fd_journal(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ناپ);
صحيح decompress_blob_منزل_fd(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ناپ);

صحيح decompress_zlib_raw(ثابت فراغ *src, uint64_t src_ناپ,
                        فراغ *dst, size_t dst_ناپ, صحيح wbits);

صحيح decompress_startswith_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر, size_t اگیتر_len,
                uint8_t extra);
صحيح decompress_startswith(Compression compression,
                          ثابت فراغ *src, uint64_t src_ناپ,
                          فراغ **بفر,
                          ثابت فراغ *اگیتر, size_t اگیتر_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

صحيح compress_دھارا(Compression type, صحيح fdf, صحيح fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_ناپ);
تعريفنوع تعداد DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

صحيح decompress_دھارا_بھریا(Compression type, صحيح fdf, صحيح fdt, uint64_t max_bytes, DecompressFlags flags);
ساكن مضمن صحيح decompress_دھارا(Compression type, صحيح fdf, صحيح fdt, uint64_t max_bytes) {
        أرجع decompress_دھارا_بھریا(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
صحيح decompress_دھارا_by_filename(ثابت حرف *filename, صحيح fdf, صحيح fdt, uint64_t max_bytes);

صحيح dlopen_xz(صحيح log_level) _dlopen_loader_;
صحيح dlopen_lz4(صحيح log_level) _dlopen_loader_;
صحيح dlopen_zstd(صحيح log_level) _dlopen_loader_;
صحيح dlopen_zlib(صحيح log_level) _dlopen_loader_;
صحيح dlopen_bzip2(صحيح log_level) _dlopen_loader_;

صحيح dlopen_compress(Compression c, صحيح log_level);
صحيح dlopen_compress_journal(Compression c, صحيح log_level);
ساكن مضمن صحيح dlopen_compress_default(صحيح log_level) {
        أرجع dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ساكن مضمن ثابت حرف* default_compression_extension(فراغ) {
        أرجع compression_extension_منزل_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_باہرلا_ناپ(فراغ);
