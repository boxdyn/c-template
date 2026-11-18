#include "str.h"

i32 utf8_pop_first(str mut self) {
    i32 out = slice_pop_first(*self, -1);
    if (out < 0) return pop_utf8_END;

    i32 cb = 0;
    switch (out) {
        case 0xf8 ... 0xff: return pop_utf8_OUT_OF_RANGE;
        case 0xf0 ... 0xf7: cb = 3, out &= 0x07; break;
        case 0xe0 ... 0xef: cb = 2, out &= 0x0f; break;
        case 0xc2 ... 0xdf: cb = 1, out &= 0x1f; break;
        case 0x80 ... 0xc1: return pop_utf8_MISSING_HEAD;
        default: return out;
    }

    for (; cb > 0; cb--) {
        i32 next = slice_pop_first(*self, -1);
        if (next < 0) return pop_utf8_MISSING_TAIL;
        if (out == 0 && ((cb == 3 && next < 0x90) || (cb == 2 && next < 0xa0))) {
            return pop_utf8_BAD_CONTINUE;
        }
        out = out << 6 | (next & 0x3f);
    }

    return out;
}

i32 utf8_pop_last(str mut self) {
    // back up until we find the start (or encounter too many CBs)
    for (int i = 1; i <= 4 && self->len; ++i) {
        let c = self->buf[--self->len];
        // then use utf8_pop_first to parse the codepoint
        if (c < 0x80 || c > 0xbf) {
            let rest = (str) {.buf = self->buf + self->len, .len = i};
            return utf8_pop_first(&rest);
        }
    }
    return pop_utf8_MISSING_HEAD;
}

bool str_is_utf8(str self) {
    // clang-format off
    enum CB { START, CB1, CB2, CB3, CBE, CBF } cb = START;
    foreach (c, self) switch (cb) {
        case START: switch (*c) {
            case 0x00 ... 0x7f: break;
            case 0xc2 ... 0xdf: cb = CB1; break;
            case 0xe0:          cb = CBE; break;
            case 0xe1 ... 0xef: cb = CB2; break;
            case 0xf0:          cb = CBF; break;
            case 0xf1 ... 0xf7: cb = CB3; break;
            default: return false; break;
        } break;
        case CBE: cb = CB1; if (*c < 0xa0 || *c >= 0xc0) return false; else break;
        case CBF: cb = CB2; if (*c < 0x90 || *c >= 0xc0) return false; else break;
        default:  cb -= 1;  if (*c < 0x80 || *c >= 0xc0) return false; else break;
    }
    // clang-format on

    return cb == START;
}

InlineSlice(u8, 4) utf8_encode(u32 code) {
    InlineSlice(u8, 4) out = {.buf = {0x00, 0x80, 0x80, 0x80}};

    switch (code) {
        case 0x10000 ... 0x10ffff: out.buf[0] = 0xf0, out.len = 4; break;
        case 0x01000 ... 0x00ffff: out.buf[0] = 0xe0, out.len = 3; break;
        case 0x00080 ... 0x000fff: out.buf[0] = 0xc0, out.len = 2; break;
        case 0x00000 ... 0x00007f: out.buf[0] = code, out.len = 1; return out;
    }

    for (usize i = 0; i < out.len; ++i) {
        out.buf[i] |= code >> 6 * (out.len - i - 1) & 0x3f;
    }

    return out;
}