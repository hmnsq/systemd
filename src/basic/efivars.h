/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

#ضمن "sd-id128.h"

#ضمن "../fundamental/efivars.h"        /* IWYU pragma: export */

#تعریف_کړه EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#تعریف_کړه EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#تعریف_کړه EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#تعریف_کړه EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#تعریف_کړه EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#تعریف_کړه EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#تعریف_کړه EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#تعریف_کړه EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#تعریف_کړه EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#تعریف_کړه EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#تعریف_کړه EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#تعریف_کړه EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#تعریف_کړه EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#تعریف_کړه EFI_VARIABLE_وخت_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#تعریف_کړه EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#تعریف_کړه EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#تعریف_کړه EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#تعریف_کړه EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#تعریف_کړه EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#تعریف_کړه EFIVAR_لار(variable) "/sys/firmware/efi/efivars/" variable
#تعریف_کړه EFIVAR_CACHE_لار(variable) "/run/systemd/efivars/" variable

#إذا ENABLE_EFI

صحيح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, فراغ **ret_ارزښت, size_t *ret_اندازه);
صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret);
صحيح efi_get_variable_لار(ثابت حرف *variable, حرف **ret);
صحيح efi_سېټ_variable(ثابت حرف *variable, ثابت فراغ *ارزښت, size_t اندازه) _nonnull_if_nonzero_(2, 3);
صحيح efi_سېټ_variable_string(ثابت حرف *variable, ثابت حرف *ارزښت);

bool سېټ_efi_boot(bool b);
bool is_efi_boot(فراغ);
bool is_efi_secure_boot(فراغ);
Secure_Boot_ډېرپېښ efi_get_secure_boot_ډېرپېښ(فراغ);

#وإلا

ساكن مضمن صحيح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, فراغ **ارزښت, size_t *اندازه) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_get_variable_لار(ثابت حرف *variable, حرف **ret) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_سېټ_variable(ثابت حرف *variable, ثابت فراغ *ارزښت, size_t اندازه) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن صحيح efi_سېټ_variable_string(ثابت حرف *variable, ثابت حرف *p) {
        أرجع -EOPNOTSUPP;
}

ساكن مضمن bool سېټ_efi_boot(bool b) {
        أرجع false;
}

ساكن مضمن bool is_efi_boot(فراغ) {
        أرجع false;
}

ساكن مضمن bool is_efi_secure_boot(فراغ) {
        أرجع false;
}

ساكن مضمن Secure_Boot_ډېرپېښ efi_get_secure_boot_ډېرپېښ(فراغ) {
        أرجع SECURE_BOOT_UNKNOWN;
}
#د_که_پای

حرف *efi_tilt_backslashes(حرف *s);
