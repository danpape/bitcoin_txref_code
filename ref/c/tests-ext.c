#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "segwit_addr.h"
#include "txrefext_code.h"

struct test_vector {
    const char magic;
    const char *hrp;
    const char* txref_encoded;
    int blockheight;
    int pos;
    int utxo_index;
    int enc_fail; //0 == must not fail, 1 == can fail, 2 == can fail and continue with next test, 3 == skip
    int dec_fail;
    int non_std;
};

static struct test_vector test_vectors[] = {
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-rjk0-u5ng-qqq8-lsnk3",
        466793,
        2205,
        0,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-rjk0-u5ng-2qqn-tcg6h",
        466793,
        2205,
        10,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-rjk0-u5ng-abcd-lsnk3", /* error correct test >abcd< instead of >qqq8<*/
        466793,
        2205,
        0,
        1,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-rqqq-qqqq-qqqu-au7hl",
        0,
        0,
        0,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-rzqq-qqqq-qqql-4ym2c",
        1,
        0,
        0,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-r7ll-lrar-qqqw-jmhax",
        2097151, /* last valid block height with current enc/dec version is 0x1FFFFF*/
        1000,
        0,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-r7ll-lrar-gqq7-zpt2e",
        2097151, /* last valid block height with current enc/dec version is 0x1FFFFF*/
        1000,
        8,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-r7ll-llll-qqqn-sr5q8",
        2097151, /* last valid block height with current enc/dec version is 0x1FFFFF*/
        8191, /* last valid tx pos is 0x1FFF */
        0,
        0,0,0
    },
    {
        TXREF_MAGIC_BTC_MAINNET,
        TXREF_BECH32_HRP_MAINNET,
        "tx1-r7ll-llll-ll8y-5yj2q",
        2097151, /* last valid block height with current enc/dec version is 0x1FFFFF*/
        8191, /* last valid tx pos is 0x1FFF */
        8191, /* last valid utxo_index is 0x1FFF */
        0,0,0
    },

            ///////////////////////////

    {
        TXREF_MAGIC_BTC_TESTNET,
        TXREF_BECH32_HRP_TESTNET,
        "txtest1-xqqq-qqqq-qqyr-qtc39q4",
        0,
        0,
        100,
        0,0,1
    },
    {
        TXREF_MAGIC_BTC_TESTNET,
        TXREF_BECH32_HRP_TESTNET,
        "txtest1-xqqq-qqqq-qqll-89622gr",
        0,
        0,
        8191,
        0,0,1
    },
    {
        TXREF_MAGIC_BTC_TESTNET,
        TXREF_BECH32_HRP_TESTNET,
        "txtest1-xjk0-uq5n-gq2q-qjtrcwr",
        466793,
        2205,
        10,
        0,0,1
    },
    {
        TXREF_MAGIC_BTC_TESTNET,
        TXREF_BECH32_HRP_TESTNET,
        "txtest1-xjk0-uq5n-gqll-83mv9gz",
        466793,
        2205,
        8191,
        0,0,1
    },

};

int main(void) {
    int fail = 0;
    int fail_in = 0;
    size_t i;
    char magic;
    int bh_check;
    int pos_check;
    int utxo_index_check;
    int res;
    char encoded_txref[32] = {};

    char hrpbuf[1024];
    for (i = 0; i < sizeof(test_vectors) / sizeof(test_vectors[0]); ++i) {
        fail_in = 0;
        memset(encoded_txref, 0, sizeof(encoded_txref));
        memset(hrpbuf, 0, sizeof(hrpbuf));
        if (test_vectors[i].enc_fail != 3) {
            res = btc_txrefext_encode(encoded_txref, test_vectors[i].hrp, test_vectors[i].magic, test_vectors[i].blockheight, test_vectors[i].pos, test_vectors[i].utxo_index, test_vectors[i].non_std);
            if (strcmp(test_vectors[i].txref_encoded, encoded_txref) != 0 && !test_vectors[i].enc_fail) {
                fail++;
                fail_in = 1;
            }
            if (test_vectors[i].enc_fail == 2) {
                if (fail_in) {
                  printf("Failed encoding in %s (%d)\n", encoded_txref, (int)i);
                }
                continue;
            }
            res = btc_txrefext_decode(encoded_txref, hrpbuf, &magic, &bh_check, &pos_check, &utxo_index_check);
        }
        else {
            res = btc_txrefext_decode(test_vectors[i].txref_encoded, hrpbuf, &magic, &bh_check, &pos_check, &utxo_index_check);
        }
        if (res!=1 && !test_vectors[i].dec_fail) {
            fail++;
        }
        if (test_vectors[i].dec_fail == 2) {
            if (fail_in) {
              printf("Failed decoding in %s (%d)\n", encoded_txref, (int)i);
            }
            continue;
        }
        /* make sure the human readable part matches */
        if (strncmp(hrpbuf, test_vectors[i].hrp, strlen(test_vectors[i].hrp)) != 0) {
            fail++;
        }
        if (test_vectors[i].utxo_index != utxo_index_check) {
            fail++;
        }
        if (test_vectors[i].pos != pos_check) {
            fail++;
        }
        if (test_vectors[i].blockheight != bh_check) {
            fail++;
        }
        if (magic != test_vectors[i].magic) {
            fail++;
        }

        if (fail_in) {
          printf("Failed decoding in %s (%d)\n", encoded_txref, (int)i);
        }
    }

    printf("%i failures\n", fail);
    return fail != 0;
}
