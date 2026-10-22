/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
गणना pcapng_खण्डम्_types {
        PCAPNG_INTERFACE_खण्डम् = 1,
        PCAPNG_PACKET_खण्डम्, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_खण्डम्,
        PCAPNG_NAME_RESOLUTION_खण्डम्,
        PCAPNG_INTERFACE_STATS_खण्डम्,
        PCAPNG_ENHANCED_PACKET_खण्डम्,

        PCAPNG_SECTION_खण्डम्   = 0x0A0D0D0A,
};

संरचना pcapng_option {
        uint16_t संहिता;
        uint16_t length;
        uint8_t दत्त[];
};

#परिभाषय PCAPNG_BYTE_क्रमः_MAGIC 0x1A2B3C4D
#परिभाषय PCAPNG_MAJOR_VERS 1
#परिभाषय PCAPNG_MINOR_VERS 0

गणना pcapng_opt {
        PCAPNG_OPT_अन्तः     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

संरचना pcapng_section {
        uint32_t खण्डम्_type;
        uint32_t खण्डम्_दीर्घता;
        uint32_t byte_क्रमः_magic;
        uint16_t major_संस्करणम्;
        uint16_t minor_संस्करणम्;
        uint64_t section_दीर्घता;
};

गणना pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

संरचना pcapng_interface_खण्डम् {
        uint32_t खण्डम्_type;   /* 1 */
        uint32_t खण्डम्_दीर्घता;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

गणना pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_जवः,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

संरचना pcapng_enhance_packet_खण्डम् {
        uint32_t खण्डम्_type; /* 6 */
        uint32_t खण्डम्_दीर्घता;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_दीर्घता;
        uint32_t original_दीर्घता;
};

/* Flags values */
#परिभाषय PCAPNG_IFB_INBOUND   0b01
#परिभाषय PCAPNG_IFB_OUTBOUND  0b10

गणना pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_QUEUE,
        PCAPNG_EPB_VERDICT,
};

संरचना pcapng_statistics_खण्डम् {
        uint32_t खण्डम्_type; /* 5 */
        uint32_t खण्डम्_दीर्घता;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

गणना pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
