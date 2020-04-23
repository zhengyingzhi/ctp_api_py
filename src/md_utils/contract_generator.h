#ifndef _CONTRACT_GENERATOR_H_
#define _CONTRACT_GENERATOR_H_


#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define MD_EXCHANGE_DCE     "DCE"
#define MD_EXCHANGE_CZCE    "CZCE"
#define MD_EXCHANGE_SHFE    "SHFE"
#define MD_EXCHANGE_INE     "INE"
#define MD_EXCHANGE_CFFEX   "CFFEX"

#define PAUSE_TIMES_SIZE    8
#define INVAL_PAUSE_TIME   -1


typedef struct
{
    int     cur_index;
    char*   pcur;
    char    buff[4080];
}contract_generator_t;

MD_UTILS_API int MD_UTILS_STDCALL generate_contract(char instrument[],
    const char* exchange, const char* code, int year_month);

MD_UTILS_API void MD_UTILS_STDCALL contract_generator_init(contract_generator_t* contr_gen);

MD_UTILS_API int MD_UTILS_STDCALL generate_contracts(contract_generator_t* contr_gen,
    char* pinstruments[], int* count, const char* exchange, const char* code, int month[], int size);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_CONTRACT_GENERATOR_H_
