#define i_implement
#include "stc/cstr.h"

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

// Define Viking lookup struct with hash, cmp, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline int RViking_cmp(const RViking* rx, const RViking* ry) {
    int c = strcmp(rx->name, ry->name);
    return c ? c : strcmp(rx->country, ry->country);
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    return c_LITERAL(Viking){cstr_from(raw.name), cstr_from(raw.country)};
}

static inline RViking Viking_toraw(const Viking* vp) {
    return c_LITERAL(RViking){cstr_str(&vp->name), cstr_str(&vp->country)};
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_key_class Viking      // key type
#define i_raw_class RViking     // lookup type
#define i_keyfrom   Viking_from
#define i_val       int         // mapped type
#define i_opt       c_no_clone
#define i_hash(rp)  stc_hash_mix(stc_strhash(rp->name), stc_strhash(rp->country))
#include "stc/cmap.h"

int main(void)
{
    Vikings vikings = {0};
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Björn", "Sweden"}, 10);

    Vikings_value* v = Vikings_get_mut(&vikings, c_LITERAL(RViking){"Einar", "Norway"});
    v->second += 3; // add 3 hp points to Einar

    c_forpair (vk, hp, Vikings, vikings) {
        printf("%s of %s has %d hp\n", cstr_str(&_.vk->name), cstr_str(&_.vk->country), *_.hp);
    }
    Vikings_drop(&vikings);
}
