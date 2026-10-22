/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
గణన pcapng_ఖండం_types {
        PCAPNG_INTERFACE_ఖండం = 1,
        PCAPNG_PACKET_ఖండం, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_ఖండం,
        PCAPNG_NAME_RESOLUTION_ఖండం,
        PCAPNG_INTERFACE_STATS_ఖండం,
        PCAPNG_ENHANCED_PACKET_ఖండం,

        PCAPNG_SECTION_ఖండం   = 0x0A0D0D0A,
};

నిర్మాణం pcapng_option {
        uint16_t సంకేతం_2;
        uint16_t length;
        uint8_t దత్తాంశం[];
};

#నిర్వచించు PCAPNG_BYTE_క్రమం_MAGIC 0x1A2B3C4D
#నిర్వచించు PCAPNG_MAJOR_VERS 1
#నిర్వచించు PCAPNG_MINOR_VERS 0

గణన pcapng_opt {
        PCAPNG_OPT_చివర     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

నిర్మాణం pcapng_section {
        uint32_t ఖండం_type;
        uint32_t ఖండం_నిడివి;
        uint32_t byte_క్రమం_magic;
        uint16_t major_సంస్కరణ;
        uint16_t minor_సంస్కరణ;
        uint64_t section_నిడివి;
};

గణన pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

నిర్మాణం pcapng_interface_ఖండం {
        uint32_t ఖండం_type;   /* 1 */
        uint32_t ఖండం_నిడివి;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

గణన pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_వేగం,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

నిర్మాణం pcapng_enhance_packet_ఖండం {
        uint32_t ఖండం_type; /* 6 */
        uint32_t ఖండం_నిడివి;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_నిడివి;
        uint32_t original_నిడివి;
};

/* Flags values */
#నిర్వచించు PCAPNG_IFB_INBOUND   0b01
#నిర్వచించు PCAPNG_IFB_OUTBOUND  0b10

గణన pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_వరుస,
        PCAPNG_EPB_VERDICT,
};

నిర్మాణం pcapng_statistics_ఖండం {
        uint32_t ఖండం_type; /* 5 */
        uint32_t ఖండం_నిడివి;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

గణన pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
