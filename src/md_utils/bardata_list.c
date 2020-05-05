#include <assert.h>
#include <string.h>

#include "bardata_list.h"


#define BARDATA_LIST_VERSION      "1.0.0"


uint8_t* struct_mem_data(struct_mem_t* sm)
{
    return (uint8_t*)(sm + 1);
}

uint8_t* struct_mem_end(struct_mem_t* sm)
{
    uint8_t* dst;
    dst = struct_mem_data(sm) + sm->count * sm->elem_size;
    return dst;
}

bool struct_mem_full(struct_mem_t* sm)
{
    return sm->count == sm->capacity;
}

uint32_t struct_mem_increment(struct_mem_t* sm)
{
    return sm->count++;
}

uint32_t struct_mem_count(struct_mem_t* sm)
{
    return sm->count;
}


//////////////////////////////////////////////////////////////////////////
void MD_UTILS_STDCALL bardata_list_init(bardata_list_t* bl, int size, void* addr)
{
    memset(bl, 0, sizeof(bardata_list_t));

    if (addr)
    {
        bar_data_t* bar;
        bl->base_addr = addr;
        bl->sm = (struct_mem_t*)(bl->base_addr);
        bar = (bar_data_t*)struct_mem_data(bl->sm);

        for (uint32_t i = 0; i < bl->sm->count; ++i)
        {
            bl->bar_datas[bl->index++] = bar;
            bar += 1;
        }
    }
    else
    {
        int alloc_size;
        alloc_size = sizeof(struct_mem_t) + sizeof(bar_data_t) * size;
        bl->base_addr = malloc(alloc_size);
        memset(bl->base_addr, 0, alloc_size);

        bl->alloced = 1;
        bl->sm = (struct_mem_t*)(bl->base_addr);
        bl->sm->capacity = size;
        bl->sm->elem_size = sizeof(bar_data_t);
    }
}

void MD_UTILS_STDCALL bardata_list_release(bardata_list_t* bl)
{
    if (bl->alloced && bl->base_addr)
    {
        free(bl->base_addr);
        bl->base_addr = NULL;
    }
}

void MD_UTILS_STDCALL bardata_list_reset(bardata_list_t* bl)
{
    bl->sm->count = 0;
}

int MD_UTILS_STDCALL bardata_list_push_bar(bardata_list_t* bl,
    const char* exchange, const char* instruent, const char* date, int update_time,
    double open, double high, double low, double close,
    int64_t volume, double turnover, int open_interest)
{
    bar_data_t* dst_bar;
    if (struct_mem_full(bl->sm)) {
        return -1;
    }

    dst_bar = (bar_data_t*)struct_mem_end(bl->sm);
    if (!dst_bar) {
        return -1;
    }

    strncpy(dst_bar->ExchangeID, exchange, sizeof(dst_bar->ExchangeID) - 1);
    strncpy(dst_bar->InstrumentID, exchange, sizeof(dst_bar->InstrumentID) - 1);
    strncpy(dst_bar->Date, exchange, sizeof(dst_bar->Date) - 1);
    dst_bar->Open = open;
    dst_bar->High = high;
    dst_bar->Low = low;
    dst_bar->Close = close;
    dst_bar->Volume = volume;
    dst_bar->Turnover = turnover;
    dst_bar->OpenInterest = open_interest;

    sim_time_t st;
    int_to_ptime(&st, update_time, 0);
    sprintf(dst_bar->Time, "%02d:%02d:%02d", st.tm_hour, st.tm_min, st.tm_sec);

    // append to tail
    bl->bar_datas[bl->index++] = dst_bar;

    struct_mem_increment(bl->sm);
    return 0;
}

int MD_UTILS_STDCALL bardata_list_push_bar2(bardata_list_t* bl,
    const bar_data_t* bar)
{
    bar_data_t* dst_bar;
    if (struct_mem_full(bl->sm)) {
        return -1;
    }

    dst_bar = (bar_data_t*)struct_mem_end(bl->sm);
    if (!dst_bar) {
        return -1;
    }

    *dst_bar = *bar;

    struct_mem_increment(bl->sm);
    return 0;
}

bar_data_t* MD_UTILS_STDCALL bardata_list_head(bardata_list_t* bl)
{
    if (bardata_list_len(bl) == 0) {
        return NULL;
    }

    return (bar_data_t*)struct_mem_data(bl->sm);
}

bar_data_t* MD_UTILS_STDCALL bardata_list_tail(bardata_list_t* bl)
{
    if (bardata_list_len(bl) == 0) {
        return NULL;
    }

    return (bar_data_t*)struct_mem_end(bl->sm) - 1;
}

int MD_UTILS_STDCALL bardata_list_len(bardata_list_t* bl)
{
    return struct_mem_count(bl->sm);
}

bar_data_t* MD_UTILS_STDCALL bardata_list_all(bardata_list_t* bl)
{
    return (bar_data_t*)struct_mem_data(bl->sm);
}

bar_data_t* MD_UTILS_STDCALL bardata_list_prevs(bardata_list_t* bl, int bar_count)
{
    int len;
    bar_data_t* dst_bar;

    len = bardata_list_len(bl);
    if (len == 0) {
        return NULL;
    }

    // if (bar_count < bardata_list_len(bl))
    //     bar_count = bardata_list_len(bl);

    dst_bar = (bar_data_t*)struct_mem_end(bl->sm);
    dst_bar = dst_bar - bar_count;

    return dst_bar;
}
