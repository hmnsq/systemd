/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-id128.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/efivars.h"        /* IWYU pragma: export */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VARIABLE_ਸਮਾਂ_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFIVAR_ਰਸਤਾ(variable) "/sys/firmware/efi/efivars/" variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFIVAR_CACHE_ਰਸਤਾ(variable) "/run/systemd/efivars/" variable

#ਜੇ ENABLE_EFI

ਪੂਰਨ efi_get_variable(ਅਚਲ ਅੱਖਰ *variable, uint32_t *attribute, ਖਾਲੀ **ret_ਮੁੱਲ, size_t *ret_ਮਾਪ);
ਪੂਰਨ efi_get_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret);
ਪੂਰਨ efi_get_variable_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret);
ਪੂਰਨ efi_ਸੈੱਟ_variable(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ) _nonnull_if_nonzero_(2, 3);
ਪੂਰਨ efi_ਸੈੱਟ_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

bool ਸੈੱਟ_efi_boot(bool b);
bool is_efi_boot(ਖਾਲੀ);
bool is_efi_secure_boot(ਖਾਲੀ);
Secure_Boot_ਬਹੁਲਕ efi_get_secure_boot_ਬਹੁਲਕ(ਖਾਲੀ);

#ਨਹੀਂਤਾਂ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ efi_get_variable(ਅਚਲ ਅੱਖਰ *variable, uint32_t *attribute, ਖਾਲੀ **ਮੁੱਲ, size_t *ਮਾਪ) {
        ਵਾਪਸ -EOPNOTSUPP;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ efi_get_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret) {
        ਵਾਪਸ -EOPNOTSUPP;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ efi_get_variable_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *variable, ਅੱਖਰ **ret) {
        ਵਾਪਸ -EOPNOTSUPP;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ efi_ਸੈੱਟ_variable(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ) {
        ਵਾਪਸ -EOPNOTSUPP;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ efi_ਸੈੱਟ_variable_string(ਅਚਲ ਅੱਖਰ *variable, ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ -EOPNOTSUPP;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸੈੱਟ_efi_boot(bool b) {
        ਵਾਪਸ false;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_efi_boot(ਖਾਲੀ) {
        ਵਾਪਸ false;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_efi_secure_boot(ਖਾਲੀ) {
        ਵਾਪਸ false;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ Secure_Boot_ਬਹੁਲਕ efi_get_secure_boot_ਬਹੁਲਕ(ਖਾਲੀ) {
        ਵਾਪਸ SECURE_BOOT_UNKNOWN;
}
#ਜੇ_ਅੰਤ

ਅੱਖਰ *efi_tilt_backslashes(ਅੱਖਰ *s);
