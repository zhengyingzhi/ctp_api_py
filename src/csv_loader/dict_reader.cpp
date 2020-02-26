#include "dict_reader.h"

#include <limits.h>

#include <codecvt>
#include <locale>


#define DICT_READER_VER     "0.0.1"


#if 0 // demo struct
typedef struct {
    char DateTime[32];
    char InstrumentID[32];
    char Exchange[8];
    double Open;
    double High;
    double Low;
    double Close;
    double Turnover;
    int Volume;
    int OpenInterest;
}BarData;
#endif

int str_delimiter_ex(const char* src, int length, zditem_t* retArr, int arrSize, char sep)
{
    int index = 0;
    const char* end;
    const char* sentinel = src + length;
    while (index < arrSize)
    {
        end = strchr(src, sep);

        if (end == NULL || end >= sentinel)
        {
            // last item
            if (sentinel - src > 0)
            {
                retArr[index].ptr = (char*)src;
                retArr[index].len = (int)(sentinel - src);
                index++;
            }
            break;
        }

        retArr[index].ptr = (char*)src;
        retArr[index].len = (int)(end - src);
        index++;

        // next pos
        src = end + 1;
    }

    return index;
}


static std::string to_utf(const std::string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");
#else
    const static locale loc("zh_CN.GB18030");
#endif

    vector<wchar_t> wstr(gb2312.size());
    wchar_t* wstrEnd = nullptr;
    const char* gbEnd = nullptr;
    mbstate_t state = {};
    int res = use_facet<codecvt<wchar_t, char, mbstate_t> >
        (loc).in(state,
            gb2312.data(), gb2312.data() + gb2312.size(), gbEnd,
            wstr.data(), wstr.data() + wstr.size(), wstrEnd);

    if (codecvt_base::ok == res)
    {
        wstring_convert<codecvt_utf8<wchar_t>> cutf8;
        return cutf8.to_bytes(wstring(wstr.data(), wstrEnd));
    }

    return std::string();
}


DictReader::DictReader()
    : m_fp(NULL)
    , m_pcur(NULL)
    , m_sep(',')
    , m_fieldindex(-1)
    , m_fields(0)
    , m_length(0)
    , m_buffer(NULL)
    , m_shm(NULL)
    , m_filename()
    , m_fieldnames()
    , m_fieldtypes()
    , m_filterflag(INT_MAX)
    , m_filterset()
{
    // default str types
    for (int k = 0; k < MAX_FIELDS; ++k)
    {
        m_fieldtypes[k] = FT_String;
    }

#if 0
    m_fieldnames[0] = "DateTime";
    m_fieldnames[1] = "InstrumentID";
    m_fieldnames[2] = "Exchange";
    m_fieldnames[3] = "Open";
    m_fieldnames[4] = "High";
    m_fieldnames[5] = "Low";
    m_fieldnames[6] = "Close";
    m_fieldnames[7] = "Volume";
    m_fieldnames[8] = "Turnover";
    m_fieldnames[9] = "OpenInterest";

    m_fieldtypes[0] = FT_String;    // DateTime
    m_fieldtypes[1] = FT_String;    // InstrumentID
    m_fieldtypes[2] = FT_String;    // Exchange
    m_fieldtypes[3] = FT_Double;    // Open
    m_fieldtypes[4] = FT_Double;    // High
    m_fieldtypes[5] = FT_Double;    // Low
    m_fieldtypes[6] = FT_Double;    // Close
    m_fieldtypes[7] = FT_Int64;     // Volume
    m_fieldtypes[8] = FT_Double;    // Turnover
    m_fieldtypes[9] = FT_Int32;     // OpenInterest
#endif
}

DictReader::~DictReader()
{
    // fprintf(stderr, "~DictReader!\n");
    close_file();

    if (m_shm)
    {
        ztl_shm_detach(m_shm);
        ztl_shm_release(m_shm);
        m_shm = NULL;
    }
}

void DictReader::set_fieldindex_key(int fieldindex)
{
    m_fieldindex = fieldindex;
}

void DictReader::set_filename(const std::string& filename, char sep)
{
    m_filename = filename;
    m_sep = sep;
}

void DictReader::set_fieldtypes(const std::vector<int>& fieldtypes)
{
    for (int i = 0; i < (int)fieldtypes.size() && i < MAX_FIELDS; ++i)
    {
        m_fieldtypes[i] = FieldType(fieldtypes[i]);
    }
}

void DictReader::set_fieldnames(const std::vector<std::string>& fieldnames)
{
    for (int i = 0; i < (int)fieldnames.size() && i < MAX_FIELDS; ++i)
    {
        m_fieldnames[i] = fieldnames[i];
        m_fields += 1;
    }
}

std::vector<std::string> DictReader::get_fieldnames()
{
    std::vector<std::string> names;
    names.reserve(m_fields);

    for (int i = 0; i < m_fields; ++i)
    {
        names.push_back(m_fieldnames[i]);
    }
    return names;
}

void DictReader::add_filter_keys(const std::vector<std::string>& filterkeys, int filterflag)
{
    for (int i = 0; i < (int)filterkeys.size(); ++i)
    {
        m_filterset.insert(filterkeys[i]);
    }
    m_filterflag = filterflag;
}

int DictReader::open_file()
{
    if (m_filename.empty())
    {
        return -1;
    }

    close_file();

    m_fp = fopen(m_filename.c_str(), "r");
    if (!m_fp)
    {
        return -1;
    }

#if 0
    m_buffer = new char[128 * 1024];
    memset(m_buffer, 0, sizeof(m_buffer));
    m_length = fread(m_buffer, sizeof(m_buffer) - 1, 1, m_fp);
#else
    open_shm();
#endif
    m_pcur = m_buffer;

    return 0;
}

int DictReader::close_file()
{
    if (m_fp)
    {
        fclose(m_fp);
        m_fp = NULL;
    }
    return 0;
}


char* DictReader::next_line_raw(int* psize)
{
    if (!m_fp)
    {
        return NULL;
    }
    else if (!m_pcur)
    {
        return 0;
    }

    int len, temp;
    char* pend;
    while (1)
    {
        pend = strchr(m_pcur, '\n');
        if (!pend)
        {
            // return (int)strlen(m_pcur);
            return NULL;
        }

        len = int(pend - m_pcur);
        if (len < 2)
        {
            m_pcur = pend + 1;
            continue;
        }

        temp = 1;
        while (*(pend - temp) == '\r' && len > 0)
        {
            ++temp;
            --len;
        }

#if 0
        int len2 = size > len ? len : size;
        memcpy(buffer, m_pcur, len2);
        m_pcur = pend + 1;
        return len2;
#else
        char* ptr = m_pcur;
        m_pcur = pend + 1;
        *psize = len;
        return ptr;
#endif
    }
    return 0;
}

std::string DictReader::next_line()
{
    int len = 0;
    char* buffer;
    buffer = next_line_raw(&len);
    if (buffer)
    {
        return std::string(buffer, len);
    }
    return std::string();
}

#if HAVE_PY
dict DictReader::next_dict_data()
{
    dict data;
    std::string line;
    while (1)
    {
        line = next_line();
        if (line.empty())
        {
            break;
        }

        if (m_fields == 0)
        {
            // init field names
            zditem_t lItemArr[MAX_FIELDS] = { 0 };
            int n = str_delimiter_ex(line.c_str(), (int)line.length(), lItemArr, MAX_FIELDS, m_sep);
            for (int i = 0; i < n; ++i)
            {
                m_fieldnames[i] = std::string(lItemArr[i].ptr, lItemArr[i].len);
                m_fields += 1;
            }

            continue;
        }

        // try filter
        zditem_t lItemArr[MAX_FIELDS] = { 0 };
        int n = str_delimiter_ex(line.c_str(), (int)line.length(), lItemArr, MAX_FIELDS, m_sep);
        if (m_fieldindex >= 0 && m_fieldindex < n && m_filterflag != INT_MAX)
        {
            std::string key(lItemArr[m_fieldindex].ptr, lItemArr[m_fieldindex].len);
            if (m_filterflag == FILTER_EXCLUDED)
            {
                // 在set中的key均过滤掉
                if (m_filterset.count(key))
                    continue;
            }
            else if (!m_filterset.count(key))
            {
                // 非已订阅的key则过滤掉
                continue;
            }
        }

        _parse_line_as_dict(data, lItemArr, n);
        break;
    }

    return data;
}

dict DictReader::convert_as_dict(const std::string& line)
{
    dict data;
    if (!line.empty())
    {
        zditem_t lItemArr[MAX_FIELDS] = { 0 };
        int n = str_delimiter_ex(line.c_str(), (int)line.length(), lItemArr, MAX_FIELDS, m_sep);
        if (n > 0)
        {
            _parse_line_as_dict(data, lItemArr, n);
        }
    }
    return data;
}

void DictReader::_parse_line_as_dict(dict& data, zditem_t aItemArr[], int aSize)
{
    for (int i = 0; i < aSize && i < m_fields; ++i)
    {
        switch (m_fieldtypes[i])
        {
        case FT_Double:
        {
            data[m_fieldnames[i].c_str()] = atof(aItemArr[i].ptr);
            break;
        }
        case FT_Int32:
        {
            data[m_fieldnames[i].c_str()] = atoi(aItemArr[i].ptr);
            break;
        }
        case FT_Int64:
        {
            data[m_fieldnames[i].c_str()] = atoll(aItemArr[i].ptr);
            break;
        }
        case FT_String:
        {
            std::string str(aItemArr[i].ptr, aItemArr[i].len);
            // data[m_fieldnames[i].c_str()] = to_utf(str);
            data[m_fieldnames[i].c_str()] = str;
            break;
        }
        default:
            break;
        }
    }

}

void DictReader::parse_line(const char* line, int len, char sep)
{
    if (len == 0)
    {
        return;
    }

    zditem_t lItemArr[MAX_FIELDS] = { 0 };
    int n = str_delimiter_ex(line, len, lItemArr, MAX_FIELDS, sep);

    gil_scoped_acquire acquire;

    std::string temp(line, len);
    this->on_line(temp);

    if (m_fieldindex >= 0)
    {
        std::string lItem(lItemArr[m_fieldindex].ptr, lItemArr[m_fieldindex].len);
        int rv = this->can_parse(m_fieldindex, lItem);
        if (!rv)
        {
            return;
        }
    }

    // generate your data here, and callback
    dict data;
    if (n > 0)
    {
        _parse_line_as_dict(data, lItemArr, n);
    }

    this->on_data(data);
}
#endif//HAVE_PY

int DictReader::proc_all_lines(int skiprows)
{
    int count = 0;
    int len = 0;
    char* buffer;

    while ((buffer = next_line_raw(&len)) != NULL)
    {
        if (count == 0 && m_fields == 0)
        {
            // init field names
            // std::string s(buffer, len);
            // fprintf(stderr, "init names %s**%d\n", s.c_str(), len);

            zditem_t lItemArr[MAX_FIELDS] = { 0 };
            int n = str_delimiter_ex(buffer, len, lItemArr, MAX_FIELDS, m_sep);
            for (int i = 0; i < n; ++i)
            {
                m_fieldnames[i] = std::string(lItemArr[i].ptr, lItemArr[i].len);
                m_fields += 1;
            }

            skiprows -= 1;
            count += 1;
            continue;
        }
        count += 1;

        // fprintf(stderr, "%s\n", buffer);
        if (skiprows > 0)
        {
            skiprows -= 1;
            continue;
        }

        parse_line(buffer, len, m_sep);
    }

    return count;
}

int DictReader::open_shm()
{
    int rv;
    ztl_shm_t* zshm;

    zshm = ztl_shm_create(m_filename.c_str(), ztl_open_only, ztl_read_only, false);
    if (!zshm)
    {
        fprintf(stderr, "shm_create failed!\n");
        return -1;
    }

    // use 0 to auto get file size internally
    rv = ztl_shm_truncate(zshm, 0);
    if (rv != 0)
    {
        fprintf(stderr, "shm_truncate failed rv:%d!\n", rv);
        return rv;
    }

    rv = ztl_shm_map_region(zshm, ztl_read_only);
    if (rv != 0)
    {
        fprintf(stderr, "shm_map_region failed rv:%d!\n", rv);
        return rv;
    }

    // read shared memory data
    char* lpAddr;
    int len;
    lpAddr = (char*)ztl_shm_get_address(zshm);
    if (!lpAddr)
    {
        fprintf(stderr, "shm_get_address failed!\n");
        return -2;
    }
    len = (int)strlen(lpAddr);

    m_length = len;
    m_buffer = lpAddr;
    m_shm = zshm;
    return 0;
}


int DictReader::to_int(const std::string& val)
{
    return atoi(val.c_str());
}

double DictReader::to_double(const std::string& val)
{
    return atof(val.c_str());
}

int DictReader::field_type_str()
{
    return FT_String;
}

int DictReader::field_type_double()
{
    return FT_Double;
}

int DictReader::field_type_int32()
{
    return FT_Int32;
}

int DictReader::field_type_int64()
{
    return FT_Int64;
}

std::string DictReader::get_version()
{
    return std::string(DICT_READER_VER);
}


//////////////////////////////////////////////////////////////////////////
#if HAVE_PY
class PyDictReader : public DictReader
{
public:
    virtual int can_parse(int index, const std::string& key) override
    {
        try
        {
            PYBIND11_OVERLOAD(int, DictReader, can_parse, index, key);
        }
        catch (const error_already_set &e)
        {
            std::cout << e.what() << std::endl;
        }
        return 0;
    }

    void on_line(const std::string& line) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, DictReader, on_line, line);
        }
        catch (const error_already_set &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void on_data(const dict& data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, DictReader, on_data, data);
        }
        catch (const error_already_set &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
};


PYBIND11_MODULE(csv_reader, m)
{
    class_<DictReader, PyDictReader> csvl(m, "DictReader", module_local());
    csvl
        .def(init<>())
        .def("get_version", &DictReader::get_version)
        .def("set_fieldindex_key", &DictReader::set_fieldindex_key)
        .def("add_filter_keys", &DictReader::add_filter_keys, pybind11::return_value_policy::reference)
        .def("set_filename", &DictReader::set_filename)
        .def("set_fieldtypes", &DictReader::set_fieldtypes, pybind11::return_value_policy::reference)
        .def("set_fieldnames", &DictReader::set_fieldnames, pybind11::return_value_policy::reference)
        .def("get_fieldnames", &DictReader::get_fieldnames, pybind11::return_value_policy::reference)
        .def("open_file", &DictReader::open_file)
        .def("close_file", &DictReader::close_file)
        .def("next_line", &DictReader::next_line)
        .def("next_dict_data", &DictReader::next_dict_data)
        .def("convert_as_dict", &DictReader::convert_as_dict)
        .def("proc_all_lines", &DictReader::proc_all_lines)
        .def("can_parse", &DictReader::can_parse)
        .def("on_line", &DictReader::on_line)
        .def("on_data", &DictReader::on_data)
        .def("to_int", &DictReader::to_int)
        .def("to_double", &DictReader::to_double)
        .def("field_type_str", &DictReader::field_type_str)
        .def("field_type_double", &DictReader::field_type_double)
        .def("field_type_int32", &DictReader::field_type_int32)
        .def("field_type_int64", &DictReader::field_type_int64)
        ;
}
#endif//HAVE_PY
