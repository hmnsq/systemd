/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-seq.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ଶୂନ୍ୟ ansi_seq_parser_done(AnsiSeqParser *p) {
        ପ୍ରତିଜ୍ଞା(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ ansi_seq_parser_begin_string(AnsiSeqParser *p, ଅକ୍ଷର introducer) {
        ପ୍ରତିଜ୍ଞା(p);

        p->string_len = 0;
        ଯଦି (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ansi_seq_parser_ଠେଲ_string(AnsiSeqParser *p, ଅକ୍ଷର c) {
        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ଯଦି (p->string_len >= ANSI_SEQ_STRING_MAX)
                ଫେରନ୍ତୁ 0; /* too long */

        ଯଦି (p->capture) {
                ଯଦି (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ଫେରନ୍ତୁ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ଫେରନ୍ତୁ 1; /* added */
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ansi_seq_parser_bad(AnsiSeqParser *p) {
        ପ୍ରତିଜ୍ଞା(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_ABORT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ansi_seq_parser_feed(AnsiSeqParser *p, ଅକ୍ଷର c) {
        uint8_t u = (uint8_t) c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);

        ବାଛ (p->state) {

        ସ୍ଥିତି ANSI_SEQ_STATE_GROUND:
                ଯଦି (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ଫେରନ୍ତୁ ANSI_SEQ_EVENT_TEXT;

        ସ୍ଥିତି ANSI_SEQ_STATE_ESC:
                ବାଛ (c) {

                ସ୍ଥିତି '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                ସ୍ଥିତି ']': /* OSC */
                ସ୍ଥିତି 'P': /* DCS */
                ସ୍ଥିତି 'X': /* SOS */
                ସ୍ଥିତି '^': /* PM */
                ସ୍ଥିତି '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                ସ୍ଥିତି 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                ସ୍ଥିତି 0x51 ... 0x57:
                ସ୍ଥିତି 0x59 ... 0x5a:
                ସ୍ଥିତି 0x5c:
                ସ୍ଥିତି 0x60 ... 0x7e: /* Fs sequence */
                ସ୍ଥିତି 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_END;

                ସ୍ଥିତି 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ଫେରନ୍ତୁ ansi_seq_parser_bad(p);
                }

        ସ୍ଥିତି ANSI_SEQ_STATE_CSI:
                ଯଦି (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ଠେଲ_string(p, c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_END;
                }

                ଯଦି (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ଠେଲ_string(p, c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r > 0)
                                ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

        ସ୍ଥିତି ANSI_SEQ_STATE_STRING:
                ଯଦି (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_END;
                }

                ଯଦି (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ଯଦି (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ଠେଲ_string(p, c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r > 0)
                                ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

        ସ୍ଥିତି ANSI_SEQ_STATE_STRING_ESC:
                ଯଦି (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_END;
                }

                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

        ସ୍ଥିତି ANSI_SEQ_STATE_NF:
                ଯଦି (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ଠେଲ_string(p, c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ଫେରନ୍ତୁ ANSI_SEQ_EVENT_END;
                }

                ଯଦି (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ଠେଲ_string(p, c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r > 0)
                                ଫେରନ୍ତୁ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ଫେରନ୍ତୁ ansi_seq_parser_bad(p);

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ansi_seq_parser_feed_harder(AnsiSeqParser *p, ଅକ୍ଷର c) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        କର {
                r = ansi_seq_parser_feed(p, c);
        } ଯେତେବେଳେ (r == ANSI_SEQ_EVENT_ABORT);

        ଫେରନ୍ତୁ r;
}

ଅଚଳ ଅକ୍ଷର* ansi_seq_parser_string(ଅଚଳ AnsiSeqParser *p) {
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (!p->capture || p->bad || p->introducer == 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ strempty(p->string);
}
