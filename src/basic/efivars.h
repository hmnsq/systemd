/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#समावेश "sd-id128.h"

#समावेश "../fundamental/efivars.h"        /* IWYU pragma: export */

#घोषणा EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#घोषणा EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#घोषणा EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#घोषणा EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#घोषणा EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#घोषणा EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#घोषणा EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#घोषणा EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#घोषणा EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#घोषणा EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#घोषणा EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#घोषणा EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#घोषणा EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#घोषणा EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#घोषणा EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#घोषणा EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#घोषणा EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#घोषणा EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#घोषणा EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#घोषणा EFIVAR_PATH(variable) "/sys/firmware/efi/efivars/" variable
#घोषणा EFIVAR_CACHE_PATH(variable) "/run/systemd/efivars/" variable

#अगर ENABLE_EFI

पूर्णांक efi_get_variable(स्थिर अक्षर *variable, uint32_t *attribute, व्योम **ret_मूल्य, माप_प्रकार *ret_आकार);
पूर्णांक efi_get_variable_string(स्थिर अक्षर *variable, अक्षर **ret);
पूर्णांक efi_get_variable_path(स्थिर अक्षर *variable, अक्षर **ret);
पूर्णांक efi_set_variable(स्थिर अक्षर *variable, स्थिर व्योम *मूल्य, माप_प्रकार आकार) _nonnull_if_nonzero_(2, 3);
पूर्णांक efi_set_variable_string(स्थिर अक्षर *variable, स्थिर अक्षर *मूल्य);

bool set_efi_boot(bool b);
bool is_efi_boot(व्योम);
bool is_efi_secure_boot(व्योम);
SecureBootMode efi_get_secure_boot_mode(व्योम);

#अन्यथा

अटल अंतरभूत पूर्णांक efi_get_variable(स्थिर अक्षर *variable, uint32_t *attribute, व्योम **मूल्य, माप_प्रकार *आकार) {
        वापस -EOPNOTSUPP;
}

अटल अंतरभूत पूर्णांक efi_get_variable_string(स्थिर अक्षर *variable, अक्षर **ret) {
        वापस -EOPNOTSUPP;
}

अटल अंतरभूत पूर्णांक efi_get_variable_path(स्थिर अक्षर *variable, अक्षर **ret) {
        वापस -EOPNOTSUPP;
}

अटल अंतरभूत पूर्णांक efi_set_variable(स्थिर अक्षर *variable, स्थिर व्योम *मूल्य, माप_प्रकार आकार) {
        वापस -EOPNOTSUPP;
}

अटल अंतरभूत पूर्णांक efi_set_variable_string(स्थिर अक्षर *variable, स्थिर अक्षर *p) {
        वापस -EOPNOTSUPP;
}

अटल अंतरभूत bool set_efi_boot(bool b) {
        वापस false;
}

अटल अंतरभूत bool is_efi_boot(व्योम) {
        वापस false;
}

अटल अंतरभूत bool is_efi_secure_boot(व्योम) {
        वापस false;
}

अटल अंतरभूत SecureBootMode efi_get_secure_boot_mode(व्योम) {
        वापस SECURE_BOOT_UNKNOWN;
}
#पूर्ण_अगर

अक्षर *efi_tilt_backslashes(अक्षर *s);
