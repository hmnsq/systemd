/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#ضمن "sd-id128.h"

#ضمن "../fundamental/efivars.h"        /* IWYU pragma: export */

#وصف_ڪريو EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#وصف_ڪريو EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#وصف_ڪريو EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#وصف_ڪريو EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#وصف_ڪريو EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#وصف_ڪريو EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#وصف_ڪريو EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#وصف_ڪريو EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#وصف_ڪريو EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#وصف_ڪريو EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#وصف_ڪريو EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#وصف_ڪريو EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#وصف_ڪريو EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#وصف_ڪريو EFI_VARIABLE_وقت_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#وصف_ڪريو EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#وصف_ڪريو EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#وصف_ڪريو EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#وصف_ڪريو EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#وصف_ڪريو EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#وصف_ڪريو EFIVAR_رستو(variable) "/sys/firmware/efi/efivars/" variable
#وصف_ڪريو EFIVAR_CACHE_رستو(variable) "/run/systemd/efivars/" variable

#إذا ENABLE_EFI

صحيح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, فراغ **ret_قدر, size_t *ret_سائيز);
صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret);
صحيح efi_get_variable_رستو(ثابت حرف *variable, حرف **ret);
صحيح efi_سيٽ_variable(ثابت حرف *variable, ثابت فراغ *قدر, size_t سائيز) _nonnull_if_nonzero_(2, 3);
صحيح efi_سيٽ_variable_string(ثابت حرف *variable, ثابت حرف *قدر);

bool سيٽ_efi_boot(bool b);
bool is_efi_boot(فراغ);
bool is_efi_secure_boot(فراغ);
Secure_Boot_گهڻائي efi_get_secure_boot_گهڻائي(فراغ);

#وإلا

ساكن مضمن صحيح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, فراغ **قدر, size_t *سائيز) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_get_variable_رستو(ثابت حرف *variable, حرف **ret) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_سيٽ_variable(ثابت حرف *variable, ثابت فراغ *قدر, size_t سائيز) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_سيٽ_variable_string(ثابت حرف *variable, ثابت حرف *p) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن bool سيٽ_efi_boot(bool b) {
        أرجع false;
}

ساكن مضمن bool is_efi_boot(فراغ) {
        أرجع false;
}

ساكن مضمن bool is_efi_secure_boot(فراغ) {
        أرجع false;
}

ساكن مضمن Secure_Boot_گهڻائي efi_get_secure_boot_گهڻائي(فراغ) {
        أرجع SECURE_BOOT_UNKNOWN;
}
#جيڪڏهن_پورو

حرف *efi_tilt_backslashes(حرف *s);
