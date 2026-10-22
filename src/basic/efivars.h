/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "sd-id128.h"

#അന്തര്ഭാവ "../fundamental/efivars.h"        /* IWYU pragma: export */

#define EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#define EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#define EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#define EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#define EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#define EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#define EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#define EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#define EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#define EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#define EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#define EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#define EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#define EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#define EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#define EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#define EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#define EFIVAR_PATH(variable) "/sys/firmware/efi/efivars/" variable
#define EFIVAR_CACHE_PATH(variable) "/run/systemd/efivars/" variable

#എങ്കിൽ ENABLE_EFI

പൂർണ്ണസംഖ്യ efi_get_variable(സ്ഥിരം അക്ഷരം *variable, uint32_t *attribute, ശൂന്യം **ret_മൂല്യം, size_t *ret_size);
പൂർണ്ണസംഖ്യ efi_get_variable_string(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ efi_get_variable_path(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ efi_set_variable(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size) _nonnull_if_nonzero_(2, 3);
പൂർണ്ണസംഖ്യ efi_set_variable_string(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം അക്ഷരം *മൂല്യം);

bool set_efi_boot(bool b);
bool is_efi_boot(ശൂന്യം);
bool is_efi_secure_boot(ശൂന്യം);
SecureBootMode efi_get_secure_boot_mode(ശൂന്യം);

#അല്ലെങ്കിൽ

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ efi_get_variable(സ്ഥിരം അക്ഷരം *variable, uint32_t *attribute, ശൂന്യം **മൂല്യം, size_t *size) {
        തിരികെ -EOPNOTSUPP;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ efi_get_variable_string(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret) {
        തിരികെ -EOPNOTSUPP;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ efi_get_variable_path(സ്ഥിരം അക്ഷരം *variable, അക്ഷരം **ret) {
        തിരികെ -EOPNOTSUPP;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ efi_set_variable(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size) {
        തിരികെ -EOPNOTSUPP;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ efi_set_variable_string(സ്ഥിരം അക്ഷരം *variable, സ്ഥിരം അക്ഷരം *p) {
        തിരികെ -EOPNOTSUPP;
}

സ്ഥിതം വരിയിൽ bool set_efi_boot(bool b) {
        തിരികെ false;
}

സ്ഥിതം വരിയിൽ bool is_efi_boot(ശൂന്യം) {
        തിരികെ false;
}

സ്ഥിതം വരിയിൽ bool is_efi_secure_boot(ശൂന്യം) {
        തിരികെ false;
}

സ്ഥിതം വരിയിൽ SecureBootMode efi_get_secure_boot_mode(ശൂന്യം) {
        തിരികെ SECURE_BOOT_UNKNOWN;
}
#endif

അക്ഷരം *efi_tilt_backslashes(അക്ഷരം *s);
