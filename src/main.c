#include <stdio.h>

#include "base/base.h"
#include "base/slice/heap_slice.h"
#include "base/str.h"

i32 main() {
    let s = STR("Hello, world! Δ\u{2021}\u{1f988}\u{10ffff}");

    let hs = (HeapSlice(i32)) {};
    defer hs_free(&hs);

    let s1 = slice_as(s, int);
    foreach (i, s1) {
        printf("%08x\n", *i);
        hs_push(&hs, *i);
    }

    for (let s2 = s; s2.len > 0;) {
        i32 c = utf8_pop_first(&s2);
        let enc = utf8_encode(c);

        printf("\"" STRFMT "\" '" STRFMT "'", STRARG(s2), STRARG(enc));
        printf(" = 0x%02x (%d)\n", c, c);
        hs_push(&hs, c);
    }

    for (let it = s; it.len > 0;) {
        i32 c = utf8_pop_last(&it);
        let enc = utf8_encode(c);

        printf("\"" STRFMT "\" '" STRFMT "'", STRARG(it), STRARG(enc));
        printf(" = 0x%02x (%d)\n", c, c);
        hs_push(&hs, c);
    }

    printf("HeapSlice(i32):\n");
    for (let it = slice_of(i32, hs); it.len;) {
        printf("    ");
        for (int i = 0; it.len && i < 6; ++i) {
            if (i) printf(", ");
            printf("%08x", slice_pop_first(it, 0x80000000));
        }
        printf(",\n");
    }

    for (int i = -1; i < (isize)s.len + 1; ++i) {
        printf("%2d: \"" STRFMT "\"", i, STRARG(slice_until(s, i)));
        printf(" \"" STRFMT "\"\n", STRARG(slice_after(s, i)));
    }
}
