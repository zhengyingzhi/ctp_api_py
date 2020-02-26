#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>
#include <set>
#include "ztl_shm.h"

using namespace std;
#define HAVE_PY 1
#if HAVE_PY
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace pybind11;
#endif//HAVE_PY


#define MAX_FIELDS      128

#define FILTER_EXCLUDED -1      // 过滤不在指定范围内的
#define FILTER_NONE     0       // 无过滤
#define FILTER_IN       1       // 过滤指定范围内


typedef enum
{
    FT_String = 0,
    FT_Double = 1,
    FT_Int32 = 2,
    FT_Int64 = 3
}FieldType;


typedef struct {
    char* ptr;
    int   len;
}zditem_t;


class DictReader
{
public:
    int to_int(const std::string& val);
    double to_double(const std::string& val);

    int field_type_str();
    int field_type_double();
    int field_type_int32();
    int field_type_int64();

    std::string get_version();

public:
    DictReader();
    ~DictReader();

    void set_fieldindex_key(int fieldindex);
    void set_filename(const std::string& filename, char sep);
    void set_fieldtypes(const std::vector<int>& fieldtypes);
    void set_fieldnames(const std::vector<std::string>& fieldnames);
    std::vector<std::string> get_fieldnames();

    void add_filter_keys(const std::vector<std::string>& filterkeys, int filter_flag);

    int open_file();
    int close_file();

    int proc_all_lines(int skiprows);

    std::string next_line();

#if HAVE_PY
    dict convert_as_dict(const std::string& line);
    dict next_dict_data();

    // deprecated can_parse ?
    virtual int can_parse(int index, const std::string& key) { return 1; }
    virtual void on_line(const std::string& line) {}
    virtual void on_data(const dict& data) {}
#endif//HAVE_PY

public:
    char* next_line_raw(int* psize);

private:
#if HAVE_PY
    void parse_line(const char* line, int len, char sep);
    void _parse_line_as_dict(dict& data, zditem_t aItemArr[], int aSize);
#else
    void parse_line(const char* line, int len, char sep) {}
#endif//HAVE_PY

    int open_shm();

private:
    FILE* m_fp;
    char* m_pcur;
    char m_sep;
    int m_fieldindex;
    int  m_fields;
    int  m_length;
    char* m_buffer;
    ztl_shm_t* m_shm;
    std::string m_filename;
    std::string m_fieldnames[MAX_FIELDS];
    FieldType m_fieldtypes[MAX_FIELDS];

    int m_filterflag;
    std::set<std::string> m_filterset;
};
