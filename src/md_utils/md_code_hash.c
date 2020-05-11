#include <assert.h>
#include <string.h>
#include <ZToolLib/ztl_utils.h>

#include "md_utils.h"
#include "md_code_hash.h"



// instrument flags
#define FLAG_None     0
#define FLAG_Call     1
#define FLAG_Put      2
#define FLAG_CallEx   3
#define FLAG_PutEx    4
#define FLAG_SP       5
#define FLAG_SPC      6
#define FLAG_SPD      7
static const char*    flag_name[] = { "", "C", "P", "-C-", "-P-", "SP ", "SPC ", "SPD " };

// market number
#define F_MKT_NONE    0
#define F_MKT_SH      1
#define F_MKT_SZ      2
#define F_MKT_HK      3
#define F_MKT_NEEQ    4
#define F_MKT_FUT     5
#define F_MKT_OTHER   7

#define STOCK_OPTION_SIZE   17      // currently only 17 bytes


#ifndef __cplusplus
#define inline
#endif//__cplusplus

typedef struct stSCode
{
    unsigned mkt      : 3;
    unsigned flag     : 3;
    unsigned code     : 20;
    unsigned adjust   : 5;
    unsigned date     : 14;
    unsigned px       : 15;
}SCode;/* 60bit */

typedef struct stFUnderlying
{
    unsigned v1       : 6;
    unsigned v2       : 6;
    unsigned date     : 14;
}FUnderlying;

typedef struct stFCode
{
    unsigned mkt      : 3;
    unsigned flag     : 3;
    unsigned code     : 26;   // FUnderlying 6+6+14
    union {
        unsigned code2: 26;
        unsigned px   : 15;
    };
}FCode;/* 58bit */


static uint32_t MurMurHash2(const void *key, int len)
{
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    const int seed = 97;
    unsigned int h = seed ^ len;
    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *)key;
    while (len >= 4)
    {
        unsigned int k = *(unsigned int *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }
    // Handle the last few bytes of the input array
    switch (len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

static inline bool IsNumericString(const char* str, int aLength)
{
    while (aLength-- > 0) 
    {
        if (*str >= '0' && *str <= '9')
        {
            ++str;
            continue;
        }
        else if (*str == '\0' || aLength == 0)
        {
            break;
        }

        return false;
    }

    return true;
}

static inline uint8_t CharToIndex(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 1;
    }
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 26 + 1;
    }
    return 0;
}

static inline char IndexToChar(uint8_t idx)
{
    if (idx == 0) {
        return 0;
    }
    if (idx <= 26) {
        return idx - 1 + 'A';
    }
    else if (idx <= 52) {
        return idx - 1 - 26 + 'a';
    }
    return 0;
}

static inline int GetUnderlyingLength(const char* apInstrID)
{
    if (apInstrID[5] >= '0' && apInstrID[5] <= '9') {
        return 6;
    }
    return 5;
}

static uint32_t EncodeUnderlying(const char* apInstrID, int aLength)
{
    uint32_t lVal = 0;
    FUnderlying*  lins = (FUnderlying*)&lVal;

    lins->v1 = CharToIndex(*apInstrID);
    ++apInstrID;
    --aLength;

    lins->v2 = CharToIndex(*apInstrID);
    if (lins->v2 > 0) {
        ++apInstrID;
        --aLength;
    }

    lins->date = (int)atoi_n(apInstrID, aLength);

    // invalid instrument id
    if (lins->v1 == 0 || lins->date == 0 /* || !IsNumericString(apInstrID, aLength)*/)
    {
        return 0;
    }

    return lVal;
}


/* calculate hash id */
static bool HashFuture(uint64_t* apOutHashID, const char* apInstrID, uint32_t aLength)
{
    const char* lpstr   = apInstrID;
    uint64_t    lHashID = 0;
    FCode*      lfc     = (FCode*)&lHashID;
    int         lCodeLength;

    lfc->mkt = F_MKT_FUT;
    if (aLength < 5)
    {
        return false;
    }
    else if (aLength <= 6)
    {
        if ((lfc->code = EncodeUnderlying(lpstr, aLength)) == 0)
        {
            return false;
        }

        *apOutHashID = lHashID;
        return true;
    }

    if (lpstr[2] == ' ' || lpstr[3] == ' ')
    {
        if (strncmp(lpstr, "SPC", 3) == 0) {
            lfc->flag = FLAG_SPC;
            lpstr += 4;
        }
        else if (strncmp(lpstr, "SP", 2) == 0) {
            lfc->flag = FLAG_SP;
            lpstr += 3;
        }
        else if (strncmp(lpstr, "SPD", 3) == 0) {
            lfc->flag = FLAG_SPD;
            lpstr += 3;
        }
        else {
            return false;
        }

        lCodeLength = GetUnderlyingLength(lpstr);
        lfc->code   = EncodeUnderlying(lpstr, lCodeLength);
        lpstr      += lCodeLength;

        // skip '&'
        lpstr += 1;

        lCodeLength = GetUnderlyingLength(lpstr);
        lfc->code2  = EncodeUnderlying(lpstr, lCodeLength);

        if (lfc->code == 0 || lfc->code2 == 0)
        {
            return false;
        }

        *apOutHashID = lHashID;
        return true;
    }
    else
    {
        lCodeLength = GetUnderlyingLength(lpstr);
        lfc->code   = EncodeUnderlying(lpstr, lCodeLength);
        if (lfc->code == 0)
        {
            return false;
        }

        lpstr      += lCodeLength;
        aLength    -= lCodeLength;

        switch (*lpstr)
        {
        case 'C':
        case 'c':
            lfc->flag = FLAG_Call;
            lpstr    += 1;
            aLength  -= 1;
            break;
        case 'P':
        case 'p':
            lfc->flag = FLAG_Put;
            lpstr    += 1;
            aLength  -= 1;
            break;
        case '-':
        {
            char ch = lpstr[1];
            if (ch == 'C' || ch == 'c')
                lfc->flag = FLAG_CallEx;
            else if (ch == 'P' || ch == 'p')
                lfc->flag = FLAG_PutEx;
            else
                return false;

            lpstr    += 3;
            aLength  -= 3;
            break;
        }
        default:
            return false;
        }

        lfc->px = (int)atoi_n(lpstr, aLength);
        if (lfc->px == 0)
        {
            return false;
        }
    }

    *apOutHashID = lHashID;
    return true;
}

static bool HashSecurity(uint64_t* apOutHashID, uint16_t aMarketId, const char* apInstrID, uint32_t aLength)
{
    const char* lpstr   = apInstrID;
    uint64_t lHashID    = 0;
    SCode*   lsc        = (SCode*)&lHashID;

    if (*lpstr < '0' || *lpstr > '9' || aLength < 5) {
        return false;
    }

    switch (aMarketId)
    {
    case MD_MI_SSE:  lsc->mkt = F_MKT_SH;    break;
    case MD_MI_SHOP: lsc->mkt = F_MKT_SH;    break;
    case MD_MI_SZSE: lsc->mkt = F_MKT_SZ;    break;
    case MD_MI_SZOP: lsc->mkt = F_MKT_SZ;    break;
    case MD_MI_SHHK: lsc->mkt = F_MKT_HK;    break;
    case MD_MI_SZHK: lsc->mkt = F_MKT_HK;    break;
    case MD_MI_NEEQ: lsc->mkt = F_MKT_NEEQ;  break;
    default:         lsc->mkt = F_MKT_OTHER; break;
    }

    uint32_t len = aLength < 6 ? aLength : 6;

    if (!IsNumericString(lpstr, len))
    {
        return false;
    }
    lsc->code   = (int)atoi_n(lpstr, len);
    lpstr      += len;

    if (*lpstr == 'C' || *lpstr == 'c')
        lsc->flag = FLAG_Call;
    else if (*lpstr == 'P' || *lpstr == 'p')
        lsc->flag = FLAG_Put;
    else if (*lpstr != '\0' && aLength > 6) {
        return false;
    }
    else {
        *apOutHashID = lHashID;
        return true;
    }
    lpstr += 1;

    // invalid security id
    if (aLength < STOCK_OPTION_SIZE) {
        return false;
    }

    lsc->date = (int)atoi_n(lpstr, 4);
    lpstr += 4;

    lsc->adjust = CharToIndex(*lpstr);
    lpstr += 1;

    lsc->px = (int)atoi_n(lpstr, 5);
    if (lsc->date == 0 || lsc->adjust == 0 || lsc->px == 0) {
        return false;
    }

    *apOutHashID = lHashID;
    return true;
}

bool md_code_get_hashid(uint64_t* phashid, const char* exchange, const char* instrument, uint32_t len)
{
#if defined(_DEBUG) || defined(DEBUG)
    assert(sizeof(SCode) == 8U);
    assert(sizeof(FCode) == 8U);
#endif//DEBUG

    bool lRet = false;
    int mi = convert_market_id(exchange);

    switch (mi)
    {
    case MD_MI_SSE:
    case MD_MI_SHOP:
    case MD_MI_SZSE:
    case MD_MI_SZOP:
    case MD_MI_SHHK:
    case MD_MI_SZHK:
    case MD_MI_NEEQ:
        lRet = HashSecurity(phashid, mi, instrument, len);
        break;
    case MD_MI_CFFEX:
    case MD_MI_SHFE:
    case MD_MI_DCE:
    case MD_MI_CZCE:
    case MD_MI_INE:
        lRet = HashFuture(phashid, instrument, len);
        break;
    default:
        break;
    }

    return lRet;
}

