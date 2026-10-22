/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "sd-id128.h"

#અન્તર્ભાવ "../fundamental/efivars.h"        /* IWYU pragma: export */

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

#જો ENABLE_EFI

પૂર્ણાંક efi_get_variable(અચલ અક્ષર *variable, uint32_t *attribute, શૂન્ય **ret_કિંમત, size_t *ret_size);
પૂર્ણાંક efi_get_variable_string(અચલ અક્ષર *variable, અક્ષર **ret);
પૂર્ણાંક efi_get_variable_path(અચલ અક્ષર *variable, અક્ષર **ret);
પૂર્ણાંક efi_set_variable(અચલ અક્ષર *variable, અચલ શૂન્ય *કિંમત, size_t size) _nonnull_if_nonzero_(2, 3);
પૂર્ણાંક efi_set_variable_string(અચલ અક્ષર *variable, અચલ અક્ષર *કિંમત);

bool set_efi_boot(bool b);
bool is_efi_boot(શૂન્ય);
bool is_efi_secure_boot(શૂન્ય);
SecureBootMode efi_get_secure_boot_mode(શૂન્ય);

#નહીંતર

સ્થિર અંતઃરેખ પૂર્ણાંક efi_get_variable(અચલ અક્ષર *variable, uint32_t *attribute, શૂન્ય **કિંમત, size_t *size) {
        પાછા -EOPNOTSUPP;
}

સ્થિર અંતઃરેખ પૂર્ણાંક efi_get_variable_string(અચલ અક્ષર *variable, અક્ષર **ret) {
        પાછા -EOPNOTSUPP;
}

સ્થિર અંતઃરેખ પૂર્ણાંક efi_get_variable_path(અચલ અક્ષર *variable, અક્ષર **ret) {
        પાછા -EOPNOTSUPP;
}

સ્થિર અંતઃરેખ પૂર્ણાંક efi_set_variable(અચલ અક્ષર *variable, અચલ શૂન્ય *કિંમત, size_t size) {
        પાછા -EOPNOTSUPP;
}

સ્થિર અંતઃરેખ પૂર્ણાંક efi_set_variable_string(અચલ અક્ષર *variable, અચલ અક્ષર *p) {
        પાછા -EOPNOTSUPP;
}

સ્થિર અંતઃરેખ bool set_efi_boot(bool b) {
        પાછા false;
}

સ્થિર અંતઃરેખ bool is_efi_boot(શૂન્ય) {
        પાછા false;
}

સ્થિર અંતઃરેખ bool is_efi_secure_boot(શૂન્ય) {
        પાછા false;
}

સ્થિર અંતઃરેખ SecureBootMode efi_get_secure_boot_mode(શૂન્ય) {
        પાછા SECURE_BOOT_UNKNOWN;
}
#endif

અક્ષર *efi_tilt_backslashes(અક્ષર *s);
