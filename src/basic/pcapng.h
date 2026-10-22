/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
شمار pcapng_بلاک_types {
        PCAPNG_INTERFACE_بلاک = 1,
        PCAPNG_PACKET_بلاک, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_بلاک,
        PCAPNG_NAME_RESOLUTION_بلاک,
        PCAPNG_INTERFACE_STATS_بلاک,
        PCAPNG_ENHANCED_PACKET_بلاک,

        PCAPNG_SECTION_بلاک   = 0x0A0D0D0A,
};

ساخت pcapng_option {
        uint16_t رمز;
        uint16_t length;
        uint8_t ڈیٹا[];
};

#تعریف_کرو PCAPNG_BYTE_ترتیب_MAGIC 0x1A2B3C4D
#تعریف_کرو PCAPNG_MAJOR_VERS 1
#تعریف_کرو PCAPNG_MINOR_VERS 0

شمار pcapng_opt {
        PCAPNG_OPT_انتہا     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

ساخت pcapng_section {
        uint32_t بلاک_type;
        uint32_t بلاک_لمبائی;
        uint32_t byte_ترتیب_magic;
        uint16_t major_نسخہ;
        uint16_t minor_نسخہ;
        uint64_t section_لمبائی;
};

شمار pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

ساخت pcapng_interface_بلاک {
        uint32_t بلاک_type;   /* 1 */
        uint32_t بلاک_لمبائی;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

شمار pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_رفتار,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

ساخت pcapng_enhance_packet_بلاک {
        uint32_t بلاک_type; /* 6 */
        uint32_t بلاک_لمبائی;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_لمبائی;
        uint32_t original_لمبائی;
};

/* Flags values */
#تعریف_کرو PCAPNG_IFB_INBOUND   0b01
#تعریف_کرو PCAPNG_IFB_OUTBOUND  0b10

شمار pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_قطار,
        PCAPNG_EPB_VERDICT,
};

ساخت pcapng_statistics_بلاک {
        uint32_t بلاک_type; /* 5 */
        uint32_t بلاک_لمبائی;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

شمار pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
