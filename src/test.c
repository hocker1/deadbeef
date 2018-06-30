#include <stdio.h>
#include "utils.h"
#include "types.h"

#define TEST(name, pass) { printf("%s ... ", name); if (pass) { printf("ok\n"); } else { printf("fail\n"); return -1; } }
#define TEST_EQ(name, x, y) TEST(name, x == y)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// collisions testing
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define RANGE_IDENT(rs, rd) { rd.p1 = rs.p1; rd.p2 = rs.p2; }
#define RANGE_INVER(rs, rd) { rd.p1 = rs.p2; rd.p2 = rs.p1; }

#define RANGES_VAR_0(rs1, rs2, rd1, rd2) { RANGE_IDENT(rs1, rd1); RANGE_IDENT(rs2, rd2); printf("+pp: "); }
#define RANGES_VAR_1(rs1, rs2, rd1, rd2) { RANGE_IDENT(rs1, rd1); RANGE_INVER(rs2, rd2); printf("+pn: "); }
#define RANGES_VAR_2(rs1, rs2, rd1, rd2) { RANGE_INVER(rs1, rd1); RANGE_IDENT(rs2, rd2); printf("+np: "); }
#define RANGES_VAR_3(rs1, rs2, rd1, rd2) { RANGE_INVER(rs1, rd1); RANGE_INVER(rs2, rd2); printf("+nn: "); }
#define RANGES_VAR_4(rs1, rs2, rd1, rd2) { RANGE_IDENT(rs2, rd1); RANGE_IDENT(rs1, rd2); printf("-pp: "); }
#define RANGES_VAR_5(rs1, rs2, rd1, rd2) { RANGE_IDENT(rs2, rd1); RANGE_INVER(rs1, rd2); printf("-pn: "); }
#define RANGES_VAR_6(rs1, rs2, rd1, rd2) { RANGE_INVER(rs2, rd1); RANGE_IDENT(rs1, rd2); printf("-np: "); }
#define RANGES_VAR_7(rs1, rs2, rd1, rd2) { RANGE_INVER(rs2, rd1); RANGE_INVER(rs1, rd2); printf("-nn: "); }

#define RANGES_TEST_EQ(name, call, value) { \
    RANGES_VAR_0(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_1(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_2(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_3(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_4(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_5(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_6(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
    RANGES_VAR_7(rs1, rs2, rd1, rd2); TEST_EQ(name, call, value); \
}

#define RANGES_MAKE(rs1, p11, p12, rs2, p21, p22) { rs1.p1 = p11; rs1.p2 = p12; rs2.p1 = p21; rs2.p2 = p22; }

int test_coll_ranges(void) {

    const struct sRangesTest {
        sPhyRange   r1;
        sPhyRange   r2;
        bool_t      result;
        char        *name;
    }
    tests[] = {
        { { 0, 1 },     { 2, 3 },       FALSE,      "outside" },
        { { -1, 1 },    { 2, 3 },       FALSE,      "outside" },
        { { -1, 1 },    { -2, -3 },     FALSE,      "outside" },
        { { -1, -2 },   { -3, -4 },     FALSE,      "outside" },
        { { 1, 2 },     { 2, 3 },       TRUE,       "edge overlap" },
        { { 1, -2 },    { -2, -3 },     TRUE,       "edge overlap" },
        { { -1, -2 },   { -2, -3 },     TRUE,       "edge overlap" },
        { { 1, 3 },     { 2, 4 },       TRUE,       "solid overlap" },
        { { -1, 3 },    { 2, 4 },       TRUE,       "solid overlap" },
        { { -1, -3 },   { -2, 4 },      TRUE,       "solid overlap" },
        { { -1, -3 },   { -2, -4 },     TRUE,       "solid overlap" },
    };
      
    sPhyRange rs1, rs2, rd1, rd2;
    char name[64];
    int i;
        
    for (i = 0; i < dimof(tests); i++) {
        
        sprintf(name, "test #%d: %s", i, tests[i].name);
        RANGE_IDENT(tests[i].r1, rs1);
        RANGE_IDENT(tests[i].r2, rs2);
        RANGES_TEST_EQ(name, coll_ranges(&rd1, &rd2), tests[i].result);
    }

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// collision movements
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int test_coll_movements(void) {

    const struct sMovesTest {
        sPhyRange           rm;
        physpace_pos_t      ym;
        sPhyVector          vm;
        sPhyRange           rs;
        physpace_pos_t      ys;
        bool_t              result;
        char                *name;
    }
    tests[] = {
        { { 0, 10 }, 0, { 10, 10 }, { 5, 15 }, 5, TRUE, NULL },
        { { 0, 10 }, 0, { 10, 10 }, { 20, 30 }, 5, FALSE, NULL }
    };
    
    int i;
    unsigned int fraction;

    for (i = 0; i < dimof(tests); i++) {

        TEST_EQ("movement", coll_hline_vec_hline((hsPhyRange)&tests[i].rm, tests[i].ym, (hsPhyVector)&tests[i].vm, (hsPhyRange)&tests[i].rs, tests[i].ys, &fraction), tests[i].result);
    }
    
    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(void) {
    
//    if (test_coll_ranges())
//        return -1;
    if (test_coll_movements())
        return -1;
    
    return 0;
}
