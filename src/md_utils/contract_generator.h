#ifndef _CONTRACT_GENERATOR_H_
#define _CONTRACT_GENERATOR_H_


#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


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
