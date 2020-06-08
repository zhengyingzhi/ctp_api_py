#ifndef _UFX_TRADE_STRUCT_H_
#define _UFX_TRADE_STRUCT_H_

#include <stdint.h>


/// some common function id
#define UFX_FUNC_QRY_SECINFO        330300      // ֤ȯ������Ϣ��ѯ
#define UFX_FUNC_QRY_CASH_FAST      332254      // �ͻ��ʽ���ٲ�ѯ
#define UFX_FUNC_QRY_CASH           332255      // �ͻ��ʽ�ȷ��ѯ
#define UFX_FUNC_PLACE_ORDER        333002      // ��ͨί��
#define UFX_FUNC_CANCEL_ORDER       333017      // ί�г���
#define UFX_FUNC_QRY_PURCHASE       333030      // �¹��깺��ѯ
#define UFX_FUNC_QRY_ORDER          333101      // ֤ȯί�в�ѯ
#define UFX_FUNC_QRY_TRADE          333102      // ֤ȯ�ɽ���ѯ
#define UFX_FUNC_QRY_POS_FAST       333103      // ֤ȯ�ֲֿ��ٲ�ѯ
#define UFX_FUNC_QRY_POSITION       333104      // ֤ȯ�ֲֲ�ѯ
#define UFX_FUNC_QRY_SECMD          400         // ֤ȯ�����ѯ
#define UFX_FUNC_LOGIN              331100      // �ͻ���¼
#define UFX_FUNC_QRY_ACCOUNTINFO    331155      // �˻���Ϣ��ȡ
#define UFX_FUNC_PWD_UPDATE         331101      // �˻��������

#define UFX_FUNC_QRY_MATCHINFO_HIST 339301      // ��ʷ�����Ϣ��ѯ
#define UFX_FUNC_QRY_LUCKINFO_HIST  339302      // ��ʷ��ǩ��Ϣ��ѯ
#define UFX_FUNC_QRY_ORDER_HIST     339303      // ��ʷ֤ȯί�в�ѯ
#define UFX_FUNC_QRY_TRADE_HIST     339304      // ��ʷ֤ȯ�ɽ���ѯ
#define UFX_FUNC_QRY_CASH_HIST      339305      // ��ʷ�ʽ�֤ȯ��ˮ��ѯ

#define UFX_FUNC_HEART              620000      // ����
#define UFX_FUNC_SUBSCRIBE          620001      // ���Ļر�
#define UFX_FUNC_SUB_CANCEL         620002      // ȡ������
#define UFX_FUNC_RTN_DATA           620003      // �ر�����

#define UFX_ISSUE_TYPE_TRADE        12          // �ɽ�����
#define UFX_ISSUE_TYPE_ORDER        23          // ί������
#define UFX_ISSUE_TYPE_DEFAULT      0           // 


/// �ֶ����Ͷ��� field type
#define UFX_FT_Char                  'C'         // �ַ�
#define UFX_FT_Str                   'S'         // �ַ���
#define UFX_FT_Int                   'I'         // ����
#define UFX_FT_Decimal               'D'         // ������
#define UFX_FT_Float                 'F'         // ������
#define UFX_FT_Raw                   'R'         // ������


/// һЩ�����ֶγ��ȶ���
#define UFX_CLIENT_ID_LEN            18
#define UFX_STOCK_CODE_LEN           24
#define UFX_STOCK_NAME_LEN           16
#define UFX_EXCHANGE_TYPE_LEN        4
#define UFX_ENTRUST_PROP_LEN         4
#define UFX_STOCK_TYPE_LEN           4
#define UFX_ENTRUST_NO_LEN           16
#define UFX_BUSINESS_NO_LEN          32


/// ������� exchange_type C4
#define UFX_EXCHANGE_CASH    "0"         // �ʽ�
#define UFX_EXCHANGE_SSE     "1"         // �Ϻ�
#define UFX_EXCHANGE_SZSE    "2"         // ����

/// �������� entrust_bs C1
#define UFX_D_Buy            '1'         // ����
#define UFX_D_Sell           '2'         // ����

/// ί������ entrust_prop C3
#define UFX_EP_BuySell       "0"         // ����
#define UFX_EP_R             "R"         // �嵵�м�ת�޼�
#define UFX_EP_U             "U"         // �嵵�мۼ���ʣ��
#define UFX_EP_d             "d"         // �޼�ί��

/// ֤ȯ��� stock_type C4
#define UFX_ST_Stock         "0"         // ��Ʊ
#define UFX_ST_Fund          "1"         // ����
#define UFX_ST_Bonus         "2"         // ����
#define UFX_ST_NormSubcri    "4"         // ��ͨ�깺
#define UFX_ST_FundSubRed    "A"         // ��������
#define UFX_ST_FundLOF       "L"         // LOF����
#define UFX_ST_ETFSubscri    "M"         // ETF�Ϲ�
#define UFX_ST_ETFRed        "N"         // ETF����
#define UFX_ST_FundETF       "T"         // ETF����
#define UFX_ST_StockGEM      "c"         // ��ҵ��

/// ί��״̬ entrust_status C1
#define UFX_ES_NotPlace          '0'     // δ��
#define UFX_ES_ToPlace           '1'     // ����
#define UFX_ES_Placed            '2'     // �ѱ�
#define UFX_ES_PlacedToCancel    '3'     // �ѱ�����
#define UFX_ES_PartTradeToCancel '4'     // ���ɴ���
#define UFX_ES_PartCancel        '5'     // ����
#define UFX_ES_Cancelled         '6'     // �ѳ�
#define UFX_ES_PartTrade         '7'     // ����
#define UFX_ES_Trade             '8'     // �ѳ�
#define UFX_ES_Placing           'C'     // ����

/// ί����� entrust_type C1
#define UFX_ET_Entrust           '0'     // ί��
#define UFX_ET_Query             '1'     // ��ѯ
#define UFX_ET_EntrustCancel     '2'     // ����
#define UFX_ET_EntrustAgain      '3'     // ����


/// �������
#define UFX_PWD_TYPE_FUND        '1'     // �ʽ�����
#define UFX_PWD_TYPE_TRADE       '2'     // ��������


#pragma pack(push,4)

struct UFXRspInfoField
{
    int         error_no;               // �������
    char        error_info[256];        // ������ʾ
};


///����Ӧ������ 620001
struct UFXRspSubscribeField
{
    int         branch_no;              // Ӫҵ����
    char        client_id[18];          // �˺�
    int         issue_type;             // �ɽ�-12 ί��-23
    char        sub_status;             // ����״̬
};

///��ѯ��������
struct UFXReqQueryField
{
    int         branch_no;              // Ӫҵ����
    char        client_id[18];          // �˺�
    char        exchange_type[4];       // ������
    char        stock_code[24];         // ֤ȯ����
};

///�ͻ���¼ 331100
struct UFXReqUserLoginField
{
    char        client_id[18];          // �˺�
    char        password[24];           // ����
    char        password_type;          // ��������
    char        mac_address[16];        // Ͷ����Mac��ַ
    char        ip_address[16];         // Ͷ����IP��ַ
};

///�ͻ���¼Ӧ��
struct UFXRspUserLoginField
{
    char        client_id[18];          // �˺�
    char        client_name[16];        // Ͷ��������
    char        company_name[24];       // ��˾ID
    int         branch_no;              // Ӫҵ����
    int         trading_date;           // ������
    int         pre_trading_date;       // �ϸ�������
    int         bill_confirm_flag;      // �˵�ȷ�ϱ�־
    int         system_id;              // ϵͳ��
    char        asset_prop;             // �ʲ����� 0-��ͨ�ͻ� 1-�����˻� B-����Ʒ�˻�
    char        last_op_station[256];   // �ϴε�¼վ��
    char        user_token[512];        // �û�����
};

///����¼�� 333002
struct UFXReqOrderInsertField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺� client_id/fund_account
    char        exchange_type[4];   // ������� 0-�ʽ� 1-�Ϻ� 2-����
    char        stock_code[24];     // ֤ȯ���� or option_code
    char        entrust_prop[4];    // ί������ 0-���� R-��ת�� U-�мۼ���ʣ��
    char        entrust_bs;         // �������� 1-���� 2-����
    char        entrust_oc;         // ��ƽ��־ O-���� C-ƽ�� X-��Ȩ A-�Զ���Ȩ
    char        covered_flag;       // ���ұ�־ 
    int         entrust_amount;     // ί������
    double      entrust_price;      // ί�м۸�
    int         batch_no;           // ���κ�
};

///�������� 333017
struct UFXReqOrderActionField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺�
    char        exchange_type[4];   // ������
    char        stock_code[24];     // ֤ȯ����
    char        entrust_no[16];     // ί�к�
    int         batch_no;           // ���κ�
};

///�ʽ��˻����� 332255
struct HSTradingAccountField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺�
    double      asset_balance;      // �ʲ�ֵ
    double      begin_balance;      // �ڳ��ʽ�
    double      frozen_balance;     // �����ʽ�
    double      current_balance;    // ��ǰ���
    double      enable_balance;     // �����ʽ�
    double      fetch_balance;      // ��ȡ�ʽ�
    double      real_buy_balance;   // �ر�������
    double      real_sell_balance;  // �ر��������
    double      net_asset;          // ���ʲ�
    double      market_value;       // �ֲ���ֵ
};


///�ֲ����� 333104
struct HSPositionField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺�
    char        exchange_type[4];   // ������
    char        stock_code[24];     // ֤ȯ����
    char        stock_name[16];     // ����
    int         init_date;          // ������
    char        stock_type[4];      // ֤ȯ���
    int         current_amount;     // ��ǰ����
    int         enable_amount;      // ��������
    int         frozen_amount;      // ��������
    int         real_buy_amount;    // �ر���������
    int         real_sell_amount;   // �ر���������
    double      cost_price;         // �ɱ���
    double      last_price;         // ���¼�
    double      market_value;       // ֤ȯ��ֵ
    double      income_balance;     // ӯ�����
    double      cost_balance;       // �ֲֳɱ�
    double      profit_ratio;       // ӯ������
    char        delist_flag;        // ���б�־
    int         delist_date;        // ��������
};

///�������� 333101
struct HSOrderField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺�
    char        exchange_type[4];   // ������
    char        stock_code[24];     // ֤ȯ����
    char        stock_name[16];     // ����
    char        entrust_no[16];     // ί�к� N10
    char        entrust_prop[4];    // ί������
    char        stock_type[4];      // ֤ȯ���
    int         init_date;          // ����
    int         entrust_date;       // ί������
    int         entrust_time;       // ί��ʱ��
    char        entrust_bs;         // ί�з���
    char        entrust_oc;         // ��ƽ��־
    char        covered_flag;       // ���ұ�־
    char        entrust_type;       // ί�����
    char        entrust_status;     // ί��״̬
    int         entrust_amount;     // ί������
    double      entrust_price;      // ί�м۸�
    double      business_price;     // �ɽ��۸�
    int         business_amount;    // �ɽ�����
    int         batch_no;           // ���κ�
};

///�ɽ����� 333102
struct HSTradeField
{
    int         branch_no;          // Ӫҵ����
    char        client_id[18];      // �˺�
    char        stock_code[24];     // ֤ȯ����
    char        stock_name[16];     // ����
    char        entrust_no[16];     // ί�к�
    char        business_no[32];    // �ɽ����
    int         date;               // �ɽ�����
    int         business_time;      // �ɽ�ʱ��
    char        real_type;          // �ɽ����� 0-���� 1-��ѯ 2-����
    char        real_status;        // �ɽ�״̬ 0-�ɽ� 2-�ϵ� 4-ȷ��
    char        exchange_type[4];   // ������� 0-�ʽ� 1-�Ϻ� 2-����
    char        stock_type[4];      // ֤ȯ��� 0-��Ʊ 1-���� 2-���� 4-��ͨ�깺 A-�������� M/N-ETF�Ϲ�/���� T-ETF����
    char        entrust_prop[4];    // ί������ 0-���� R-��ת�� U-�мۼ���ʣ��
    char        entrust_type;       // ί����� 0-ί�� 1-��ѯ 2-���� 3-���� 4-ȷ��
    char        entrust_status;     // ί��״̬ 0-δ�� 1-���� 2-�ѱ� 3-�ѱ����� 4-���ɴ��� 5-���� 6-�ѳ� 7-���� 8-�ѳ� 9-�ϵ�
    char        entrust_bs;         // ί�з��� 1-���� 2-����
    char        entrust_oc;         // ��ƽ��־ O-���� C-ƽ�� X-��Ȩ A-�Զ���Ȩ
    char        covered_flag;       // ���ұ�־ 
    char        opt_hold_type;      // ��Ȩ�ֲ���� 0-Ȩ���� 1-���� 2-���ҷ�
    char        option_type;        // ��Ȩ���� C/P
    int         business_amount;    // �ɽ�����
    double      business_price;     // �ɽ��۸� or opt_business_price
    double      business_balance;   // �ɽ����
    double      entrust_price;      // ί�м۸� or opt_entrust_price
    int         entrust_amount;     // ί������
    int         batch_no;           // ���κ�
};

///֤ȯ��������
struct HSSecurityInfoField
{
    char        exchange_type[4];   // ������
    char        stock_code[24];     // ֤ȯ����
    char        stock_name[16];     // ����
};

///��������
struct HSMarketDataField
{
    char        exchange_type[4];   // ������
    char        stock_code[24];     // ֤ȯ����
    int         trading_date;       // ������
    int         update_time;        // ����ʱ��
    double      last_price;
    double      OpenPrice;
    double      HighPrice;
    double      LowPrice;
    double      Turnover;
    int64_t     Volume;

    double      PreClose;
    double      UpperLimit;
    double      LowerLimit;

    double      BidPrice1;
    int         BidVolume1;
    double      AskPrice1;
    int         AskVolume1;
    double      BidPrice2;
    int         BidVolume2;
    double      AskPrice2;
    int         AskVolume2;
    double      BidPrice3;
    int         BidVolume3;
    double      AskPrice3;
    int         AskVolume3;
    double      BidPrice4;
    int         BidVolume4;
    double      AskPrice4;
    int         AskVolume4;
    double      BidPrice5;
    int         BidVolume5;
    double      AskPrice5;
    int         AskVolume5;
};

#pragma pack(pop)

#endif//_UFX_TRADE_STRUCT_H_
