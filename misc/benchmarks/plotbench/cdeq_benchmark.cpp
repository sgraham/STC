#include <stdio.h>
#include <time.h>
#define i_static
#include "stc/crand.h"

#ifdef __cplusplus
#include <deque>
#include <algorithm>
#endif

enum {INSERT, ERASE, FIND, ITER, DESTRUCT, N_TESTS};
const char* operations[] = {"insert", "erase", "access", "iter", "destruct"};
typedef struct { time_t t1, t2; uint64_t sum; float fac; } Range;
typedef struct { const char* name; Range test[N_TESTS]; } Sample;
enum {SAMPLES = 2, N = 60000000, R = 4};
uint64_t seed = 1, mask1 = 0xfffffff, mask2 = 0xffff;

static float secs(Range s) { return (float)(s.t2 - s.t1) / CLOCKS_PER_SEC; }

#define i_tag x
#define i_val size_t
#include "stc/cdeq.h"

#ifdef __cplusplus
Sample test_std_deque() {
    typedef std::deque<size_t> container;
    Sample s = {"std,deque"};
    {
        s.test[INSERT].t1 = clock();
        container con;
        csrand(seed);
        c_forrange (N/3) con.push_front(crand() & mask1);
        c_forrange (N/3) {con.push_back(crand() & mask1); con.pop_front();}
        c_forrange (N/3) con.push_back(crand() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = con.size();
        s.test[ERASE].t1 = clock();
        c_forrange (con.size()/2) { con.pop_front(); con.pop_back(); }
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = con.size();
     }{
        container con;
        csrand(seed);
        c_forrange (N) con.push_back(crand() & mask2);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        c_forrange (R) c_forrange (i, N) sum += con[i];
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) for (const auto i: con) sum += i;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}
#else
Sample test_std_deque() { Sample s = {"std-deque"}; return s;}
#endif


Sample test_stc_deque() {
    typedef cdeq_x container;
    Sample s = {"STC,deque"};
    {
        s.test[INSERT].t1 = clock();
        container con = cdeq_x_init();
        //cdeq_x_reserve(&con, N);
        csrand(seed);
        c_forrange (N/3) cdeq_x_push_front(&con, crand() & mask1);
        c_forrange (N/3) {cdeq_x_push_back(&con, crand() & mask1); cdeq_x_pop_front(&con);}
        c_forrange (N/3) cdeq_x_push_back(&con, crand() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = cdeq_x_size(&con);
        s.test[ERASE].t1 = clock();
        c_forrange (cdeq_x_size(&con)/2) { cdeq_x_pop_front(&con); cdeq_x_pop_back(&con); }
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = cdeq_x_size(&con);
        cdeq_x_drop(&con);
     }{
        csrand(seed);
        container con = cdeq_x_init();
        c_forrange (N) cdeq_x_push_back(&con, crand() & mask2);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        c_forrange (R) c_forrange (i, N) sum += *cdeq_x_at(&con, i);
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) c_foreach (i, cdeq_x, con) sum += *i.ref;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
        cdeq_x_drop(&con);
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}

int main(int argc, char* argv[])
{
    Sample std_s[SAMPLES + 1], stc_s[SAMPLES + 1];
    c_forrange (i, SAMPLES) {
        std_s[i] = test_std_deque();
        stc_s[i] = test_stc_deque();
        if (i > 0) c_forrange (j, N_TESTS) {
            if (secs(std_s[i].test[j]) < secs(std_s[0].test[j])) std_s[0].test[j] = std_s[i].test[j];
            if (secs(stc_s[i].test[j]) < secs(stc_s[0].test[j])) stc_s[0].test[j] = stc_s[i].test[j];
            if (stc_s[i].test[j].sum != stc_s[0].test[j].sum) printf("Error in sum: test %lld, sample %lld\n", i, j);
        }
    }
    const char* comp = argc > 1 ? argv[1] : "test";
    bool header = (argc > 2 && argv[2][0] == '1');
    float std_sum = 0, stc_sum = 0;

    c_forrange (j, N_TESTS) {
        std_sum += secs(std_s[0].test[j]);
        stc_sum += secs(stc_s[0].test[j]);
    }
    if (header) printf("Compiler,Library,C,Method,Seconds,Ratio\n");

    c_forrange (j, N_TESTS)
        printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, std_s[0].name, N, operations[j], secs(std_s[0].test[j]), 1.0f);
    printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, std_s[0].name, N, "total", std_sum, 1.0f);

    c_forrange (j, N_TESTS)
        printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, stc_s[0].name, N, operations[j], secs(stc_s[0].test[j]), secs(std_s[0].test[j]) ? secs(stc_s[0].test[j])/secs(std_s[0].test[j]) : 1.0f);
    printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, stc_s[0].name, N, "total", stc_sum, stc_sum/std_sum);
}
