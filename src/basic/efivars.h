/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#अन्तर्भाव "sd-id128.h"

#अन्तर्भाव "../fundamental/efivars.h"        /* IWYU pragma: export */

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

#सचे ENABLE_EFI

पूर्णाङ्क efi_get_variable(ध्रुव वर्ण *variable, uint32_t *attribute, शून्य **ret_मानम्, size_t *ret_size);
पूर्णाङ्क efi_get_variable_string(ध्रुव वर्ण *variable, वर्ण **ret);
पूर्णाङ्क efi_get_variable_path(ध्रुव वर्ण *variable, वर्ण **ret);
पूर्णाङ्क efi_set_variable(ध्रुव वर्ण *variable, ध्रुव शून्य *मानम्, size_t size) _nonnull_if_nonzero_(2, 3);
पूर्णाङ्क efi_set_variable_string(ध्रुव वर्ण *variable, ध्रुव वर्ण *मानम्);

bool set_efi_boot(bool b);
bool is_efi_boot(शून्य);
bool is_efi_secure_boot(शून्य);
SecureBootMode efi_get_secure_boot_mode(शून्य);

#अञ्ञथा

स्थिर अन्तःस्थ पूर्णाङ्क efi_get_variable(ध्रुव वर्ण *variable, uint32_t *attribute, शून्य **मानम्, size_t *size) {
        पटिनिवत्त -EOPNOTSUPP;
}

स्थिर अन्तःस्थ पूर्णाङ्क efi_get_variable_string(ध्रुव वर्ण *variable, वर्ण **ret) {
        पटिनिवत्त -EOPNOTSUPP;
}

स्थिर अन्तःस्थ पूर्णाङ्क efi_get_variable_path(ध्रुव वर्ण *variable, वर्ण **ret) {
        पटिनिवत्त -EOPNOTSUPP;
}

स्थिर अन्तःस्थ पूर्णाङ्क efi_set_variable(ध्रुव वर्ण *variable, ध्रुव शून्य *मानम्, size_t size) {
        पटिनिवत्त -EOPNOTSUPP;
}

स्थिर अन्तःस्थ पूर्णाङ्क efi_set_variable_string(ध्रुव वर्ण *variable, ध्रुव वर्ण *p) {
        पटिनिवत्त -EOPNOTSUPP;
}

स्थिर अन्तःस्थ bool set_efi_boot(bool b) {
        पटिनिवत्त false;
}

स्थिर अन्तःस्थ bool is_efi_boot(शून्य) {
        पटिनिवत्त false;
}

स्थिर अन्तःस्थ bool is_efi_secure_boot(शून्य) {
        पटिनिवत्त false;
}

स्थिर अन्तःस्थ SecureBootMode efi_get_secure_boot_mode(शून्य) {
        पटिनिवत्त SECURE_BOOT_UNKNOWN;
}
#endif

वर्ण *efi_tilt_backslashes(वर्ण *s);
