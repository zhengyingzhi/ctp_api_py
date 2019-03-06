void TdApi::processFrontConnected(Task task)
{
	PyLock lock;
	this->onFrontConnected();
};

void TdApi::processFrontDisconnected(Task task)
{
	PyLock lock;
	this->onFrontDisconnected(task.task_id);
};

void TdApi::processHeartBeatWarning(Task task)
{
	PyLock lock;
	this->onHeartBeatWarning(task.task_id);
};

void TdApi::processRspAuthenticate(Task task)
{
	PyLock lock;
	CThostFtdcRspAuthenticateField task_data = any_cast<CThostFtdcRspAuthenticateField>(task.task_data);
	dict data;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspAuthenticate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspUserLogin(Task task)
{
	PyLock lock;
	CThostFtdcRspUserLoginField task_data = any_cast<CThostFtdcRspUserLoginField>(task.task_data);
	dict data;
	data["CZCETime"] = boost::locale::conv::to_utf<char>(task_data.CZCETime, std::string("GB2312"));
	data["SHFETime"] = boost::locale::conv::to_utf<char>(task_data.SHFETime, std::string("GB2312"));
	data["MaxOrderRef"] = boost::locale::conv::to_utf<char>(task_data.MaxOrderRef, std::string("GB2312"));
	data["INETime"] = boost::locale::conv::to_utf<char>(task_data.INETime, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["SystemName"] = boost::locale::conv::to_utf<char>(task_data.SystemName, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["FFEXTime"] = boost::locale::conv::to_utf<char>(task_data.FFEXTime, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DCETime"] = boost::locale::conv::to_utf<char>(task_data.DCETime, std::string("GB2312"));
	data["LoginTime"] = boost::locale::conv::to_utf<char>(task_data.LoginTime, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspUserLogin(data, error, task.task_id, task.task_last);
};

void TdApi::processRspUserLogout(Task task)
{
	PyLock lock;
	CThostFtdcUserLogoutField task_data = any_cast<CThostFtdcUserLogoutField>(task.task_data);
	dict data;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspUserLogout(data, error, task.task_id, task.task_last);
};

void TdApi::processRspUserPasswordUpdate(Task task)
{
	PyLock lock;
	CThostFtdcUserPasswordUpdateField task_data = any_cast<CThostFtdcUserPasswordUpdateField>(task.task_data);
	dict data;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["NewPassword"] = boost::locale::conv::to_utf<char>(task_data.NewPassword, std::string("GB2312"));
	data["OldPassword"] = boost::locale::conv::to_utf<char>(task_data.OldPassword, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspUserPasswordUpdate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspTradingAccountPasswordUpdate(Task task)
{
	PyLock lock;
	CThostFtdcTradingAccountPasswordUpdateField task_data = any_cast<CThostFtdcTradingAccountPasswordUpdateField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["NewPassword"] = boost::locale::conv::to_utf<char>(task_data.NewPassword, std::string("GB2312"));
	data["OldPassword"] = boost::locale::conv::to_utf<char>(task_data.OldPassword, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspTradingAccountPasswordUpdate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspOrderInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputOrderField task_data = any_cast<CThostFtdcInputOrderField>(task.task_data);
	dict data;
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["TimeCondition"] = task_data.TimeCondition;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspOrderInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspParkedOrderInsert(Task task)
{
	PyLock lock;
	CThostFtdcParkedOrderField task_data = any_cast<CThostFtdcParkedOrderField>(task.task_data);
	dict data;
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Status"] = task_data.Status;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["UserType"] = task_data.UserType;
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["TimeCondition"] = task_data.TimeCondition;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["ErrorID"] = task_data.ErrorID;
	data["ParkedOrderID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspParkedOrderInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspParkedOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcParkedOrderActionField task_data = any_cast<CThostFtdcParkedOrderActionField>(task.task_data);
	dict data;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["ParkedOrderActionID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderActionID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["Status"] = task_data.Status;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["VolumeChange"] = task_data.VolumeChange;
	data["UserType"] = task_data.UserType;
	data["SessionID"] = task_data.SessionID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["ErrorID"] = task_data.ErrorID;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspParkedOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcInputOrderActionField task_data = any_cast<CThostFtdcInputOrderActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ActionFlag"] = task_data.ActionFlag;
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["VolumeChange"] = task_data.VolumeChange;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQueryMaxOrderVolume(Task task)
{
	PyLock lock;
	CThostFtdcQueryMaxOrderVolumeField task_data = any_cast<CThostFtdcQueryMaxOrderVolumeField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["MaxVolume"] = task_data.MaxVolume;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQueryMaxOrderVolume(data, error, task.task_id, task.task_last);
};

void TdApi::processRspSettlementInfoConfirm(Task task)
{
	PyLock lock;
	CThostFtdcSettlementInfoConfirmField task_data = any_cast<CThostFtdcSettlementInfoConfirmField>(task.task_data);
	dict data;
	data["SettlementID"] = task_data.SettlementID;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ConfirmDate"] = boost::locale::conv::to_utf<char>(task_data.ConfirmDate, std::string("GB2312"));
	data["ConfirmTime"] = boost::locale::conv::to_utf<char>(task_data.ConfirmTime, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspSettlementInfoConfirm(data, error, task.task_id, task.task_last);
};

void TdApi::processRspRemoveParkedOrder(Task task)
{
	PyLock lock;
	CThostFtdcRemoveParkedOrderField task_data = any_cast<CThostFtdcRemoveParkedOrderField>(task.task_data);
	dict data;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ParkedOrderID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspRemoveParkedOrder(data, error, task.task_id, task.task_last);
};

void TdApi::processRspRemoveParkedOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcRemoveParkedOrderActionField task_data = any_cast<CThostFtdcRemoveParkedOrderActionField>(task.task_data);
	dict data;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ParkedOrderActionID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderActionID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspRemoveParkedOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspExecOrderInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputExecOrderField task_data = any_cast<CThostFtdcInputExecOrderField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["CloseFlag"] = task_data.CloseFlag;
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["PosiDirection"] = task_data.PosiDirection;
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ActionType"] = task_data.ActionType;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ReservePositionFlag"] = task_data.ReservePositionFlag;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspExecOrderInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspExecOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcInputExecOrderActionField task_data = any_cast<CThostFtdcInputExecOrderActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["ExecOrderActionRef"] = task_data.ExecOrderActionRef;
	data["FrontID"] = task_data.FrontID;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspExecOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspForQuoteInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputForQuoteField task_data = any_cast<CThostFtdcInputForQuoteField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ForQuoteRef"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteRef, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspForQuoteInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQuoteInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputQuoteField task_data = any_cast<CThostFtdcInputQuoteField>(task.task_data);
	dict data;
	data["AskHedgeFlag"] = task_data.AskHedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["AskVolume"] = task_data.AskVolume;
	data["BidOrderRef"] = boost::locale::conv::to_utf<char>(task_data.BidOrderRef, std::string("GB2312"));
	data["BidHedgeFlag"] = task_data.BidHedgeFlag;
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["AskOffsetFlag"] = task_data.AskOffsetFlag;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["BidOffsetFlag"] = task_data.BidOffsetFlag;
	data["BidPrice"] = task_data.BidPrice;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["BidVolume"] = task_data.BidVolume;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["AskPrice"] = task_data.AskPrice;
	data["AskOrderRef"] = boost::locale::conv::to_utf<char>(task_data.AskOrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ForQuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteSysID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQuoteInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQuoteAction(Task task)
{
	PyLock lock;
	CThostFtdcInputQuoteActionField task_data = any_cast<CThostFtdcInputQuoteActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["QuoteActionRef"] = task_data.QuoteActionRef;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["FrontID"] = task_data.FrontID;
	data["QuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.QuoteSysID, std::string("GB2312"));
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQuoteAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspBatchOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcInputBatchOrderActionField task_data = any_cast<CThostFtdcInputBatchOrderActionField>(task.task_data);
	dict data;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspBatchOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspOptionSelfCloseInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputOptionSelfCloseField task_data = any_cast<CThostFtdcInputOptionSelfCloseField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptSelfCloseFlag"] = task_data.OptSelfCloseFlag;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspOptionSelfCloseInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspOptionSelfCloseAction(Task task)
{
	PyLock lock;
	CThostFtdcInputOptionSelfCloseActionField task_data = any_cast<CThostFtdcInputOptionSelfCloseActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptionSelfCloseSysID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["OptionSelfCloseActionRef"] = task_data.OptionSelfCloseActionRef;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspOptionSelfCloseAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspCombActionInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputCombActionField task_data = any_cast<CThostFtdcInputCombActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["CombActionRef"] = boost::locale::conv::to_utf<char>(task_data.CombActionRef, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CombDirection"] = task_data.CombDirection;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspCombActionInsert(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryOrder(Task task)
{
	PyLock lock;
	CThostFtdcOrderField task_data = any_cast<CThostFtdcOrderField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["VolumeTraded"] = task_data.VolumeTraded;
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["RelativeOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.RelativeOrderSysID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["InstallID"] = task_data.InstallID;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["VolumeTotal"] = task_data.VolumeTotal;
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["SessionID"] = task_data.SessionID;
	data["TimeCondition"] = task_data.TimeCondition;
	data["OrderStatus"] = task_data.OrderStatus;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["OrderType"] = task_data.OrderType;
	data["UpdateTime"] = boost::locale::conv::to_utf<char>(task_data.UpdateTime, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["ActiveTime"] = boost::locale::conv::to_utf<char>(task_data.ActiveTime, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["SuspendTime"] = boost::locale::conv::to_utf<char>(task_data.SuspendTime, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["BrokerOrderSeq"] = task_data.BrokerOrderSeq;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;
	data["OrderSource"] = task_data.OrderSource;
	data["ZCETotalTradedVolume"] = task_data.ZCETotalTradedVolume;
	data["ActiveTraderID"] = boost::locale::conv::to_utf<char>(task_data.ActiveTraderID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryOrder(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTrade(Task task)
{
	PyLock lock;
	CThostFtdcTradeField task_data = any_cast<CThostFtdcTradeField>(task.task_data);
	dict data;
	data["TradeType"] = task_data.TradeType;
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["Price"] = task_data.Price;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["TradeID"] = boost::locale::conv::to_utf<char>(task_data.TradeID, std::string("GB2312"));
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["BrokerOrderSeq"] = task_data.BrokerOrderSeq;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["TradeSource"] = task_data.TradeSource;
	data["PriceSource"] = task_data.PriceSource;
	data["TradingRole"] = task_data.TradingRole;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTrade(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestorPosition(Task task)
{
	PyLock lock;
	CThostFtdcInvestorPositionField task_data = any_cast<CThostFtdcInvestorPositionField>(task.task_data);
	dict data;
	data["ShortFrozenAmount"] = task_data.ShortFrozenAmount;
	data["FrozenMargin"] = task_data.FrozenMargin;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["PositionProfit"] = task_data.PositionProfit;
	data["Commission"] = task_data.Commission;
	data["MarginRateByVolume"] = task_data.MarginRateByVolume;
	data["StrikeFrozen"] = task_data.StrikeFrozen;
	data["CombPosition"] = task_data.CombPosition;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CashIn"] = task_data.CashIn;
	data["PreSettlementPrice"] = task_data.PreSettlementPrice;
	data["CombLongFrozen"] = task_data.CombLongFrozen;
	data["CloseAmount"] = task_data.CloseAmount;
	data["PosiDirection"] = task_data.PosiDirection;
	data["YdPosition"] = task_data.YdPosition;
	data["MarginRateByMoney"] = task_data.MarginRateByMoney;
	data["AbandonFrozen"] = task_data.AbandonFrozen;
	data["OpenVolume"] = task_data.OpenVolume;
	data["CloseVolume"] = task_data.CloseVolume;
	data["ExchangeMargin"] = task_data.ExchangeMargin;
	data["YdStrikeFrozen"] = task_data.YdStrikeFrozen;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["PositionDate"] = task_data.PositionDate;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["CloseProfitByTrade"] = task_data.CloseProfitByTrade;
	data["PreMargin"] = task_data.PreMargin;
	data["SettlementID"] = task_data.SettlementID;
	data["ShortFrozen"] = task_data.ShortFrozen;
	data["LongFrozen"] = task_data.LongFrozen;
	data["TodayPosition"] = task_data.TodayPosition;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["PositionCost"] = task_data.PositionCost;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["FrozenCash"] = task_data.FrozenCash;
	data["OpenAmount"] = task_data.OpenAmount;
	data["OpenCost"] = task_data.OpenCost;
	data["Position"] = task_data.Position;
	data["FrozenCommission"] = task_data.FrozenCommission;
	data["CombShortFrozen"] = task_data.CombShortFrozen;
	data["CloseProfitByDate"] = task_data.CloseProfitByDate;
	data["SettlementPrice"] = task_data.SettlementPrice;
	data["LongFrozenAmount"] = task_data.LongFrozenAmount;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["StrikeFrozenAmount"] = task_data.StrikeFrozenAmount;
	data["CloseProfit"] = task_data.CloseProfit;
	data["UseMargin"] = task_data.UseMargin;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestorPosition(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTradingAccount(Task task)
{
	PyLock lock;
	CThostFtdcTradingAccountField task_data = any_cast<CThostFtdcTradingAccountField>(task.task_data);
	dict data;
	data["SpecProductFrozenCommission"] = task_data.SpecProductFrozenCommission;
	data["FundMortgageOut"] = task_data.FundMortgageOut;
	data["Mortgage"] = task_data.Mortgage;
	data["ExchangeDeliveryMargin"] = task_data.ExchangeDeliveryMargin;
	data["FrozenMargin"] = task_data.FrozenMargin;
	data["WithdrawQuota"] = task_data.WithdrawQuota;
	data["PositionProfit"] = task_data.PositionProfit;
	data["Commission"] = task_data.Commission;
	data["SpecProductCloseProfit"] = task_data.SpecProductCloseProfit;
	data["Interest"] = task_data.Interest;
	data["SpecProductCommission"] = task_data.SpecProductCommission;
	data["CashIn"] = task_data.CashIn;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Available"] = task_data.Available;
	data["FundMortgageAvailable"] = task_data.FundMortgageAvailable;
	data["FrozenSwap"] = task_data.FrozenSwap;
	data["PreCredit"] = task_data.PreCredit;
	data["PreMortgage"] = task_data.PreMortgage;
	data["InterestBase"] = task_data.InterestBase;
	data["ExchangeMargin"] = task_data.ExchangeMargin;
	data["MortgageableFund"] = task_data.MortgageableFund;
	data["PreFundMortgageIn"] = task_data.PreFundMortgageIn;
	data["PreMargin"] = task_data.PreMargin;
	data["SettlementID"] = task_data.SettlementID;
	data["BizType"] = task_data.BizType;
	data["FundMortgageIn"] = task_data.FundMortgageIn;
	data["DeliveryMargin"] = task_data.DeliveryMargin;
	data["SpecProductFrozenMargin"] = task_data.SpecProductFrozenMargin;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["FrozenCash"] = task_data.FrozenCash;
	data["Withdraw"] = task_data.Withdraw;
	data["RemainSwap"] = task_data.RemainSwap;
	data["Balance"] = task_data.Balance;
	data["SpecProductMargin"] = task_data.SpecProductMargin;
	data["SpecProductPositionProfitByAlg"] = task_data.SpecProductPositionProfitByAlg;
	data["Reserve"] = task_data.Reserve;
	data["PreDeposit"] = task_data.PreDeposit;
	data["Credit"] = task_data.Credit;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ReserveBalance"] = task_data.ReserveBalance;
	data["SpecProductPositionProfit"] = task_data.SpecProductPositionProfit;
	data["SpecProductExchangeMargin"] = task_data.SpecProductExchangeMargin;
	data["PreBalance"] = task_data.PreBalance;
	data["CurrMargin"] = task_data.CurrMargin;
	data["FrozenCommission"] = task_data.FrozenCommission;
	data["CloseProfit"] = task_data.CloseProfit;
	data["PreFundMortgageOut"] = task_data.PreFundMortgageOut;
	data["Deposit"] = task_data.Deposit;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTradingAccount(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestor(Task task)
{
	PyLock lock;
	CThostFtdcInvestorField task_data = any_cast<CThostFtdcInvestorField>(task.task_data);
	dict data;
	data["CommModelID"] = boost::locale::conv::to_utf<char>(task_data.CommModelID, std::string("GB2312"));
	data["InvestorName"] = boost::locale::conv::to_utf<char>(task_data.InvestorName, std::string("GB2312"));
	data["Mobile"] = boost::locale::conv::to_utf<char>(task_data.Mobile, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["Telephone"] = boost::locale::conv::to_utf<char>(task_data.Telephone, std::string("GB2312"));
	data["MarginModelID"] = boost::locale::conv::to_utf<char>(task_data.MarginModelID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["Address"] = boost::locale::conv::to_utf<char>(task_data.Address, std::string("GB2312"));
	data["InvestorGroupID"] = boost::locale::conv::to_utf<char>(task_data.InvestorGroupID, std::string("GB2312"));
	data["OpenDate"] = boost::locale::conv::to_utf<char>(task_data.OpenDate, std::string("GB2312"));
	data["IsActive"] = task_data.IsActive;
	data["IdentifiedCardType"] = task_data.IdentifiedCardType;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestor(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTradingCode(Task task)
{
	PyLock lock;
	CThostFtdcTradingCodeField task_data = any_cast<CThostFtdcTradingCodeField>(task.task_data);
	dict data;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BizType"] = task_data.BizType;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ClientIDType"] = task_data.ClientIDType;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IsActive"] = task_data.IsActive;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTradingCode(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInstrumentMarginRate(Task task)
{
	PyLock lock;
	CThostFtdcInstrumentMarginRateField task_data = any_cast<CThostFtdcInstrumentMarginRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ShortMarginRatioByMoney"] = task_data.ShortMarginRatioByMoney;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["LongMarginRatioByMoney"] = task_data.LongMarginRatioByMoney;
	data["IsRelative"] = task_data.IsRelative;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["ShortMarginRatioByVolume"] = task_data.ShortMarginRatioByVolume;
	data["LongMarginRatioByVolume"] = task_data.LongMarginRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInstrumentMarginRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInstrumentCommissionRate(Task task)
{
	PyLock lock;
	CThostFtdcInstrumentCommissionRateField task_data = any_cast<CThostFtdcInstrumentCommissionRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OpenRatioByMoney"] = task_data.OpenRatioByMoney;
	data["CloseRatioByVolume"] = task_data.CloseRatioByVolume;
	data["BizType"] = task_data.BizType;
	data["CloseTodayRatioByMoney"] = task_data.CloseTodayRatioByMoney;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["CloseRatioByMoney"] = task_data.CloseRatioByMoney;
	data["OpenRatioByVolume"] = task_data.OpenRatioByVolume;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CloseTodayRatioByVolume"] = task_data.CloseTodayRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInstrumentCommissionRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryExchange(Task task)
{
	PyLock lock;
	CThostFtdcExchangeField task_data = any_cast<CThostFtdcExchangeField>(task.task_data);
	dict data;
	data["ExchangeProperty"] = task_data.ExchangeProperty;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ExchangeName"] = boost::locale::conv::to_utf<char>(task_data.ExchangeName, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryExchange(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryProduct(Task task)
{
	PyLock lock;
	CThostFtdcProductField task_data = any_cast<CThostFtdcProductField>(task.task_data);
	dict data;
	data["MaxLimitOrderVolume"] = task_data.MaxLimitOrderVolume;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MortgageFundUseRange"] = task_data.MortgageFundUseRange;
	data["PositionDateType"] = task_data.PositionDateType;
	data["MinLimitOrderVolume"] = task_data.MinLimitOrderVolume;
	data["CloseDealType"] = task_data.CloseDealType;
	data["MaxMarketOrderVolume"] = task_data.MaxMarketOrderVolume;
	data["PriceTick"] = task_data.PriceTick;
	data["ProductName"] = boost::locale::conv::to_utf<char>(task_data.ProductName, std::string("GB2312"));
	data["ExchangeProductID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeProductID, std::string("GB2312"));
	data["VolumeMultiple"] = task_data.VolumeMultiple;
	data["PositionType"] = task_data.PositionType;
	data["MinMarketOrderVolume"] = task_data.MinMarketOrderVolume;
	data["ProductClass"] = task_data.ProductClass;
	data["UnderlyingMultiple"] = task_data.UnderlyingMultiple;
	data["TradeCurrencyID"] = boost::locale::conv::to_utf<char>(task_data.TradeCurrencyID, std::string("GB2312"));
	data["ProductID"] = boost::locale::conv::to_utf<char>(task_data.ProductID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryProduct(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInstrument(Task task)
{
	PyLock lock;
	CThostFtdcInstrumentField task_data = any_cast<CThostFtdcInstrumentField>(task.task_data);
	dict data;
	data["IsTrading"] = task_data.IsTrading;
	data["ExpireDate"] = boost::locale::conv::to_utf<char>(task_data.ExpireDate, std::string("GB2312"));
	data["PositionDateType"] = task_data.PositionDateType;
	data["LongMarginRatio"] = task_data.LongMarginRatio;
	data["StrikePrice"] = task_data.StrikePrice;
	data["UnderlyingMultiple"] = task_data.UnderlyingMultiple;
	data["PositionType"] = task_data.PositionType;
	data["ProductClass"] = task_data.ProductClass;
	data["InstrumentName"] = boost::locale::conv::to_utf<char>(task_data.InstrumentName, std::string("GB2312"));
	data["ShortMarginRatio"] = task_data.ShortMarginRatio;
	data["VolumeMultiple"] = task_data.VolumeMultiple;
	data["MaxMarginSideAlgorithm"] = task_data.MaxMarginSideAlgorithm;
	data["DeliveryYear"] = task_data.DeliveryYear;
	data["CombinationType"] = task_data.CombinationType;
	data["CreateDate"] = boost::locale::conv::to_utf<char>(task_data.CreateDate, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["MaxLimitOrderVolume"] = task_data.MaxLimitOrderVolume;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinLimitOrderVolume"] = task_data.MinLimitOrderVolume;
	data["MaxMarketOrderVolume"] = task_data.MaxMarketOrderVolume;
	data["OptionsType"] = task_data.OptionsType;
	data["StartDelivDate"] = boost::locale::conv::to_utf<char>(task_data.StartDelivDate, std::string("GB2312"));
	data["DeliveryMonth"] = task_data.DeliveryMonth;
	data["PriceTick"] = task_data.PriceTick;
	data["InstLifePhase"] = task_data.InstLifePhase;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["MinMarketOrderVolume"] = task_data.MinMarketOrderVolume;
	data["EndDelivDate"] = boost::locale::conv::to_utf<char>(task_data.EndDelivDate, std::string("GB2312"));
	data["UnderlyingInstrID"] = boost::locale::conv::to_utf<char>(task_data.UnderlyingInstrID, std::string("GB2312"));
	data["OpenDate"] = boost::locale::conv::to_utf<char>(task_data.OpenDate, std::string("GB2312"));
	data["ProductID"] = boost::locale::conv::to_utf<char>(task_data.ProductID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInstrument(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryDepthMarketData(Task task)
{
	PyLock lock;
	CThostFtdcDepthMarketDataField task_data = any_cast<CThostFtdcDepthMarketDataField>(task.task_data);
	dict data;
	data["HighestPrice"] = task_data.HighestPrice;
	data["BidPrice5"] = task_data.BidPrice5;
	data["BidPrice4"] = task_data.BidPrice4;
	data["BidPrice1"] = task_data.BidPrice1;
	data["BidPrice3"] = task_data.BidPrice3;
	data["BidPrice2"] = task_data.BidPrice2;
	data["LowerLimitPrice"] = task_data.LowerLimitPrice;
	data["OpenPrice"] = task_data.OpenPrice;
	data["AskPrice5"] = task_data.AskPrice5;
	data["AskPrice4"] = task_data.AskPrice4;
	data["AskPrice3"] = task_data.AskPrice3;
	data["PreClosePrice"] = task_data.PreClosePrice;
	data["AskPrice1"] = task_data.AskPrice1;
	data["PreSettlementPrice"] = task_data.PreSettlementPrice;
	data["AskVolume1"] = task_data.AskVolume1;
	data["UpdateTime"] = boost::locale::conv::to_utf<char>(task_data.UpdateTime, std::string("GB2312"));
	data["UpdateMillisec"] = task_data.UpdateMillisec;
	data["AveragePrice"] = task_data.AveragePrice;
	data["BidVolume5"] = task_data.BidVolume5;
	data["BidVolume4"] = task_data.BidVolume4;
	data["BidVolume3"] = task_data.BidVolume3;
	data["BidVolume2"] = task_data.BidVolume2;
	data["PreOpenInterest"] = task_data.PreOpenInterest;
	data["AskPrice2"] = task_data.AskPrice2;
	data["Volume"] = task_data.Volume;
	data["AskVolume3"] = task_data.AskVolume3;
	data["AskVolume2"] = task_data.AskVolume2;
	data["AskVolume5"] = task_data.AskVolume5;
	data["AskVolume4"] = task_data.AskVolume4;
	data["UpperLimitPrice"] = task_data.UpperLimitPrice;
	data["BidVolume1"] = task_data.BidVolume1;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ClosePrice"] = task_data.ClosePrice;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["PreDelta"] = task_data.PreDelta;
	data["OpenInterest"] = task_data.OpenInterest;
	data["CurrDelta"] = task_data.CurrDelta;
	data["Turnover"] = task_data.Turnover;
	data["LastPrice"] = task_data.LastPrice;
	data["SettlementPrice"] = task_data.SettlementPrice;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["LowestPrice"] = task_data.LowestPrice;
	data["ActionDay"] = boost::locale::conv::to_utf<char>(task_data.ActionDay, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryDepthMarketData(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQrySettlementInfo(Task task)
{
	PyLock lock;
	CThostFtdcSettlementInfoField task_data = any_cast<CThostFtdcSettlementInfoField>(task.task_data);
	dict data;
	data["SettlementID"] = task_data.SettlementID;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["Content"] = boost::locale::conv::to_utf<char>(task_data.Content, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQrySettlementInfo(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTransferBank(Task task)
{
	PyLock lock;
	CThostFtdcTransferBankField task_data = any_cast<CThostFtdcTransferBankField>(task.task_data);
	dict data;
	data["BankName"] = boost::locale::conv::to_utf<char>(task_data.BankName, std::string("GB2312"));
	data["IsActive"] = task_data.IsActive;
	data["BankBrchID"] = boost::locale::conv::to_utf<char>(task_data.BankBrchID, std::string("GB2312"));
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTransferBank(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestorPositionDetail(Task task)
{
	PyLock lock;
	CThostFtdcInvestorPositionDetailField task_data = any_cast<CThostFtdcInvestorPositionDetailField>(task.task_data);
	dict data;
	data["PositionProfitByDate"] = task_data.PositionProfitByDate;
	data["ExchMargin"] = task_data.ExchMargin;
	data["TradeType"] = task_data.TradeType;
	data["MarginRateByMoney"] = task_data.MarginRateByMoney;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["MarginRateByVolume"] = task_data.MarginRateByVolume;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["CloseAmount"] = task_data.CloseAmount;
	data["OpenPrice"] = task_data.OpenPrice;
	data["Volume"] = task_data.Volume;
	data["LastSettlementPrice"] = task_data.LastSettlementPrice;
	data["CloseVolume"] = task_data.CloseVolume;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["CloseProfitByTrade"] = task_data.CloseProfitByTrade;
	data["SettlementID"] = task_data.SettlementID;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["Margin"] = task_data.Margin;
	data["TradeID"] = boost::locale::conv::to_utf<char>(task_data.TradeID, std::string("GB2312"));
	data["PositionProfitByTrade"] = task_data.PositionProfitByTrade;
	data["CloseProfitByDate"] = task_data.CloseProfitByDate;
	data["SettlementPrice"] = task_data.SettlementPrice;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["CombInstrumentID"] = boost::locale::conv::to_utf<char>(task_data.CombInstrumentID, std::string("GB2312"));
	data["OpenDate"] = boost::locale::conv::to_utf<char>(task_data.OpenDate, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestorPositionDetail(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryNotice(Task task)
{
	PyLock lock;
	CThostFtdcNoticeField task_data = any_cast<CThostFtdcNoticeField>(task.task_data);
	dict data;
	data["Content"] = boost::locale::conv::to_utf<char>(task_data.Content, std::string("GB2312"));
	data["SequenceLabel"] = boost::locale::conv::to_utf<char>(task_data.SequenceLabel, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryNotice(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQrySettlementInfoConfirm(Task task)
{
	PyLock lock;
	CThostFtdcSettlementInfoConfirmField task_data = any_cast<CThostFtdcSettlementInfoConfirmField>(task.task_data);
	dict data;
	data["SettlementID"] = task_data.SettlementID;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ConfirmDate"] = boost::locale::conv::to_utf<char>(task_data.ConfirmDate, std::string("GB2312"));
	data["ConfirmTime"] = boost::locale::conv::to_utf<char>(task_data.ConfirmTime, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQrySettlementInfoConfirm(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestorPositionCombineDetail(Task task)
{
	PyLock lock;
	CThostFtdcInvestorPositionCombineDetailField task_data = any_cast<CThostFtdcInvestorPositionCombineDetailField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["TradeGroupID"] = task_data.TradeGroupID;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MarginRateByVolume"] = task_data.MarginRateByVolume;
	data["ComTradeID"] = boost::locale::conv::to_utf<char>(task_data.ComTradeID, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["TotalAmt"] = task_data.TotalAmt;
	data["Margin"] = task_data.Margin;
	data["ExchMargin"] = task_data.ExchMargin;
	data["LegMultiple"] = task_data.LegMultiple;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["TradeID"] = boost::locale::conv::to_utf<char>(task_data.TradeID, std::string("GB2312"));
	data["LegID"] = task_data.LegID;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["MarginRateByMoney"] = task_data.MarginRateByMoney;
	data["Direction"] = task_data.Direction;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombInstrumentID"] = boost::locale::conv::to_utf<char>(task_data.CombInstrumentID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["OpenDate"] = boost::locale::conv::to_utf<char>(task_data.OpenDate, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestorPositionCombineDetail(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryCFMMCTradingAccountKey(Task task)
{
	PyLock lock;
	CThostFtdcCFMMCTradingAccountKeyField task_data = any_cast<CThostFtdcCFMMCTradingAccountKeyField>(task.task_data);
	dict data;
	data["KeyID"] = task_data.KeyID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["CurrentKey"] = boost::locale::conv::to_utf<char>(task_data.CurrentKey, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryCFMMCTradingAccountKey(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryEWarrantOffset(Task task)
{
	PyLock lock;
	CThostFtdcEWarrantOffsetField task_data = any_cast<CThostFtdcEWarrantOffsetField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["Direction"] = task_data.Direction;
	data["Volume"] = task_data.Volume;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryEWarrantOffset(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestorProductGroupMargin(Task task)
{
	PyLock lock;
	CThostFtdcInvestorProductGroupMarginField task_data = any_cast<CThostFtdcInvestorProductGroupMarginField>(task.task_data);
	dict data;
	data["ExchMargin"] = task_data.ExchMargin;
	data["ShortExchOffsetAmount"] = task_data.ShortExchOffsetAmount;
	data["FrozenMargin"] = task_data.FrozenMargin;
	data["ShortFrozenMargin"] = task_data.ShortFrozenMargin;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["PositionProfit"] = task_data.PositionProfit;
	data["Commission"] = task_data.Commission;
	data["LongOffsetAmount"] = task_data.LongOffsetAmount;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CashIn"] = task_data.CashIn;
	data["ShortUseMargin"] = task_data.ShortUseMargin;
	data["ShortOffsetAmount"] = task_data.ShortOffsetAmount;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["LongExchOffsetAmount"] = task_data.LongExchOffsetAmount;
	data["LongUseMargin"] = task_data.LongUseMargin;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["FrozenCash"] = task_data.FrozenCash;
	data["LongFrozenMargin"] = task_data.LongFrozenMargin;
	data["ShortExchMargin"] = task_data.ShortExchMargin;
	data["FrozenCommission"] = task_data.FrozenCommission;
	data["ProductGroupID"] = boost::locale::conv::to_utf<char>(task_data.ProductGroupID, std::string("GB2312"));
	data["ExchOffsetAmount"] = task_data.ExchOffsetAmount;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["LongExchMargin"] = task_data.LongExchMargin;
	data["CloseProfit"] = task_data.CloseProfit;
	data["OffsetAmount"] = task_data.OffsetAmount;
	data["UseMargin"] = task_data.UseMargin;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestorProductGroupMargin(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryExchangeMarginRate(Task task)
{
	PyLock lock;
	CThostFtdcExchangeMarginRateField task_data = any_cast<CThostFtdcExchangeMarginRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ShortMarginRatioByMoney"] = task_data.ShortMarginRatioByMoney;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["LongMarginRatioByMoney"] = task_data.LongMarginRatioByMoney;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ShortMarginRatioByVolume"] = task_data.ShortMarginRatioByVolume;
	data["LongMarginRatioByVolume"] = task_data.LongMarginRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryExchangeMarginRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryExchangeMarginRateAdjust(Task task)
{
	PyLock lock;
	CThostFtdcExchangeMarginRateAdjustField task_data = any_cast<CThostFtdcExchangeMarginRateAdjustField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ShortMarginRatioByMoney"] = task_data.ShortMarginRatioByMoney;
	data["ExchLongMarginRatioByMoney"] = task_data.ExchLongMarginRatioByMoney;
	data["ExchShortMarginRatioByMoney"] = task_data.ExchShortMarginRatioByMoney;
	data["LongMarginRatioByMoney"] = task_data.LongMarginRatioByMoney;
	data["ExchShortMarginRatioByVolume"] = task_data.ExchShortMarginRatioByVolume;
	data["ExchLongMarginRatioByVolume"] = task_data.ExchLongMarginRatioByVolume;
	data["NoShortMarginRatioByMoney"] = task_data.NoShortMarginRatioByMoney;
	data["NoLongMarginRatioByMoney"] = task_data.NoLongMarginRatioByMoney;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["NoLongMarginRatioByVolume"] = task_data.NoLongMarginRatioByVolume;
	data["NoShortMarginRatioByVolume"] = task_data.NoShortMarginRatioByVolume;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ShortMarginRatioByVolume"] = task_data.ShortMarginRatioByVolume;
	data["LongMarginRatioByVolume"] = task_data.LongMarginRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryExchangeMarginRateAdjust(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryExchangeRate(Task task)
{
	PyLock lock;
	CThostFtdcExchangeRateField task_data = any_cast<CThostFtdcExchangeRateField>(task.task_data);
	dict data;
	data["FromCurrencyID"] = boost::locale::conv::to_utf<char>(task_data.FromCurrencyID, std::string("GB2312"));
	data["FromCurrencyUnit"] = task_data.FromCurrencyUnit;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ExchangeRate"] = task_data.ExchangeRate;
	data["ToCurrencyID"] = boost::locale::conv::to_utf<char>(task_data.ToCurrencyID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryExchangeRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQrySecAgentACIDMap(Task task)
{
	PyLock lock;
	CThostFtdcSecAgentACIDMapField task_data = any_cast<CThostFtdcSecAgentACIDMapField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BrokerSecAgentID"] = boost::locale::conv::to_utf<char>(task_data.BrokerSecAgentID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQrySecAgentACIDMap(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryProductExchRate(Task task)
{
	PyLock lock;
	CThostFtdcProductExchRateField task_data = any_cast<CThostFtdcProductExchRateField>(task.task_data);
	dict data;
	data["QuoteCurrencyID"] = boost::locale::conv::to_utf<char>(task_data.QuoteCurrencyID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ExchangeRate"] = task_data.ExchangeRate;
	data["ProductID"] = boost::locale::conv::to_utf<char>(task_data.ProductID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryProductExchRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryProductGroup(Task task)
{
	PyLock lock;
	CThostFtdcProductGroupField task_data = any_cast<CThostFtdcProductGroupField>(task.task_data);
	dict data;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ProductGroupID"] = boost::locale::conv::to_utf<char>(task_data.ProductGroupID, std::string("GB2312"));
	data["ProductID"] = boost::locale::conv::to_utf<char>(task_data.ProductID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryProductGroup(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryMMInstrumentCommissionRate(Task task)
{
	PyLock lock;
	CThostFtdcMMInstrumentCommissionRateField task_data = any_cast<CThostFtdcMMInstrumentCommissionRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OpenRatioByMoney"] = task_data.OpenRatioByMoney;
	data["CloseRatioByVolume"] = task_data.CloseRatioByVolume;
	data["CloseTodayRatioByMoney"] = task_data.CloseTodayRatioByMoney;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["CloseRatioByMoney"] = task_data.CloseRatioByMoney;
	data["OpenRatioByVolume"] = task_data.OpenRatioByVolume;
	data["CloseTodayRatioByVolume"] = task_data.CloseTodayRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryMMInstrumentCommissionRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryMMOptionInstrCommRate(Task task)
{
	PyLock lock;
	CThostFtdcMMOptionInstrCommRateField task_data = any_cast<CThostFtdcMMOptionInstrCommRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OpenRatioByMoney"] = task_data.OpenRatioByMoney;
	data["StrikeRatioByMoney"] = task_data.StrikeRatioByMoney;
	data["CloseRatioByVolume"] = task_data.CloseRatioByVolume;
	data["CloseTodayRatioByMoney"] = task_data.CloseTodayRatioByMoney;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["CloseRatioByMoney"] = task_data.CloseRatioByMoney;
	data["OpenRatioByVolume"] = task_data.OpenRatioByVolume;
	data["StrikeRatioByVolume"] = task_data.StrikeRatioByVolume;
	data["CloseTodayRatioByVolume"] = task_data.CloseTodayRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryMMOptionInstrCommRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInstrumentOrderCommRate(Task task)
{
	PyLock lock;
	CThostFtdcInstrumentOrderCommRateField task_data = any_cast<CThostFtdcInstrumentOrderCommRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["OrderActionCommByVolume"] = task_data.OrderActionCommByVolume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["OrderCommByVolume"] = task_data.OrderCommByVolume;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInstrumentOrderCommRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQrySecAgentTradingAccount(Task task)
{
	PyLock lock;
	CThostFtdcTradingAccountField task_data = any_cast<CThostFtdcTradingAccountField>(task.task_data);
	dict data;
	data["SpecProductFrozenCommission"] = task_data.SpecProductFrozenCommission;
	data["FundMortgageOut"] = task_data.FundMortgageOut;
	data["Mortgage"] = task_data.Mortgage;
	data["ExchangeDeliveryMargin"] = task_data.ExchangeDeliveryMargin;
	data["FrozenMargin"] = task_data.FrozenMargin;
	data["WithdrawQuota"] = task_data.WithdrawQuota;
	data["PositionProfit"] = task_data.PositionProfit;
	data["Commission"] = task_data.Commission;
	data["SpecProductCloseProfit"] = task_data.SpecProductCloseProfit;
	data["Interest"] = task_data.Interest;
	data["SpecProductCommission"] = task_data.SpecProductCommission;
	data["CashIn"] = task_data.CashIn;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Available"] = task_data.Available;
	data["FundMortgageAvailable"] = task_data.FundMortgageAvailable;
	data["FrozenSwap"] = task_data.FrozenSwap;
	data["PreCredit"] = task_data.PreCredit;
	data["PreMortgage"] = task_data.PreMortgage;
	data["InterestBase"] = task_data.InterestBase;
	data["ExchangeMargin"] = task_data.ExchangeMargin;
	data["MortgageableFund"] = task_data.MortgageableFund;
	data["PreFundMortgageIn"] = task_data.PreFundMortgageIn;
	data["PreMargin"] = task_data.PreMargin;
	data["SettlementID"] = task_data.SettlementID;
	data["BizType"] = task_data.BizType;
	data["FundMortgageIn"] = task_data.FundMortgageIn;
	data["DeliveryMargin"] = task_data.DeliveryMargin;
	data["SpecProductFrozenMargin"] = task_data.SpecProductFrozenMargin;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["FrozenCash"] = task_data.FrozenCash;
	data["Withdraw"] = task_data.Withdraw;
	data["RemainSwap"] = task_data.RemainSwap;
	data["Balance"] = task_data.Balance;
	data["SpecProductMargin"] = task_data.SpecProductMargin;
	data["SpecProductPositionProfitByAlg"] = task_data.SpecProductPositionProfitByAlg;
	data["Reserve"] = task_data.Reserve;
	data["PreDeposit"] = task_data.PreDeposit;
	data["Credit"] = task_data.Credit;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ReserveBalance"] = task_data.ReserveBalance;
	data["SpecProductPositionProfit"] = task_data.SpecProductPositionProfit;
	data["SpecProductExchangeMargin"] = task_data.SpecProductExchangeMargin;
	data["PreBalance"] = task_data.PreBalance;
	data["CurrMargin"] = task_data.CurrMargin;
	data["FrozenCommission"] = task_data.FrozenCommission;
	data["CloseProfit"] = task_data.CloseProfit;
	data["PreFundMortgageOut"] = task_data.PreFundMortgageOut;
	data["Deposit"] = task_data.Deposit;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQrySecAgentTradingAccount(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQrySecAgentCheckMode(Task task)
{
	PyLock lock;
	CThostFtdcSecAgentCheckModeField task_data = any_cast<CThostFtdcSecAgentCheckModeField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerSecAgentID"] = boost::locale::conv::to_utf<char>(task_data.BrokerSecAgentID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CheckSelfAccount"] = task_data.CheckSelfAccount;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQrySecAgentCheckMode(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryOptionInstrTradeCost(Task task)
{
	PyLock lock;
	CThostFtdcOptionInstrTradeCostField task_data = any_cast<CThostFtdcOptionInstrTradeCostField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ExchMiniMargin"] = task_data.ExchMiniMargin;
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["Royalty"] = task_data.Royalty;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["MiniMargin"] = task_data.MiniMargin;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["ExchFixedMargin"] = task_data.ExchFixedMargin;
	data["FixedMargin"] = task_data.FixedMargin;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryOptionInstrTradeCost(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryOptionInstrCommRate(Task task)
{
	PyLock lock;
	CThostFtdcOptionInstrCommRateField task_data = any_cast<CThostFtdcOptionInstrCommRateField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OpenRatioByMoney"] = task_data.OpenRatioByMoney;
	data["StrikeRatioByMoney"] = task_data.StrikeRatioByMoney;
	data["CloseRatioByVolume"] = task_data.CloseRatioByVolume;
	data["CloseTodayRatioByMoney"] = task_data.CloseTodayRatioByMoney;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["CloseRatioByMoney"] = task_data.CloseRatioByMoney;
	data["OpenRatioByVolume"] = task_data.OpenRatioByVolume;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["StrikeRatioByVolume"] = task_data.StrikeRatioByVolume;
	data["CloseTodayRatioByVolume"] = task_data.CloseTodayRatioByVolume;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryOptionInstrCommRate(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryExecOrder(Task task)
{
	PyLock lock;
	CThostFtdcExecOrderField task_data = any_cast<CThostFtdcExecOrderField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["CloseFlag"] = task_data.CloseFlag;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ExecResult"] = task_data.ExecResult;
	data["ReservePositionFlag"] = task_data.ReservePositionFlag;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InstallID"] = task_data.InstallID;
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["PosiDirection"] = task_data.PosiDirection;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["ActionType"] = task_data.ActionType;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["ExecOrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderLocalID, std::string("GB2312"));
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["BrokerExecOrderSeq"] = task_data.BrokerExecOrderSeq;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryExecOrder(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryForQuote(Task task)
{
	PyLock lock;
	CThostFtdcForQuoteField task_data = any_cast<CThostFtdcForQuoteField>(task.task_data);
	dict data;
	data["ForQuoteLocalID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteLocalID, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["BrokerForQutoSeq"] = task_data.BrokerForQutoSeq;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ForQuoteRef"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteRef, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["ForQuoteStatus"] = task_data.ForQuoteStatus;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryForQuote(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryQuote(Task task)
{
	PyLock lock;
	CThostFtdcQuoteField task_data = any_cast<CThostFtdcQuoteField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["AskHedgeFlag"] = task_data.AskHedgeFlag;
	data["BidOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.BidOrderSysID, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["AskVolume"] = task_data.AskVolume;
	data["BidOrderRef"] = boost::locale::conv::to_utf<char>(task_data.BidOrderRef, std::string("GB2312"));
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["BidHedgeFlag"] = task_data.BidHedgeFlag;
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["AskOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.AskOrderSysID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["AskOffsetFlag"] = task_data.AskOffsetFlag;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BidOffsetFlag"] = task_data.BidOffsetFlag;
	data["BidPrice"] = task_data.BidPrice;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["QuoteStatus"] = task_data.QuoteStatus;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["QuoteLocalID"] = boost::locale::conv::to_utf<char>(task_data.QuoteLocalID, std::string("GB2312"));
	data["BidVolume"] = task_data.BidVolume;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["QuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.QuoteSysID, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["ForQuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteSysID, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["AskPrice"] = task_data.AskPrice;
	data["SequenceNo"] = task_data.SequenceNo;
	data["AskOrderRef"] = boost::locale::conv::to_utf<char>(task_data.AskOrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["BrokerQuoteSeq"] = task_data.BrokerQuoteSeq;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryQuote(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryOptionSelfClose(Task task)
{
	PyLock lock;
	CThostFtdcOptionSelfCloseField task_data = any_cast<CThostFtdcOptionSelfCloseField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ExecResult"] = task_data.ExecResult;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["SessionID"] = task_data.SessionID;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptionSelfCloseSysID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["OptSelfCloseFlag"] = task_data.OptSelfCloseFlag;
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BrokerOptionSelfCloseSeq"] = task_data.BrokerOptionSelfCloseSeq;
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;
	data["OptionSelfCloseLocalID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseLocalID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryOptionSelfClose(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryInvestUnit(Task task)
{
	PyLock lock;
	CThostFtdcInvestUnitField task_data = any_cast<CThostFtdcInvestUnitField>(task.task_data);
	dict data;
	data["CommModelID"] = boost::locale::conv::to_utf<char>(task_data.CommModelID, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["MarginModelID"] = boost::locale::conv::to_utf<char>(task_data.MarginModelID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["InvestorUnitName"] = boost::locale::conv::to_utf<char>(task_data.InvestorUnitName, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestorGroupID"] = boost::locale::conv::to_utf<char>(task_data.InvestorGroupID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryInvestUnit(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryCombInstrumentGuard(Task task)
{
	PyLock lock;
	CThostFtdcCombInstrumentGuardField task_data = any_cast<CThostFtdcCombInstrumentGuardField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["GuarantRatio"] = task_data.GuarantRatio;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryCombInstrumentGuard(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryCombAction(Task task)
{
	PyLock lock;
	CThostFtdcCombActionField task_data = any_cast<CThostFtdcCombActionField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ComTradeID"] = boost::locale::conv::to_utf<char>(task_data.ComTradeID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ActionStatus"] = task_data.ActionStatus;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CombDirection"] = task_data.CombDirection;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["SessionID"] = task_data.SessionID;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombActionRef"] = boost::locale::conv::to_utf<char>(task_data.CombActionRef, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryCombAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTransferSerial(Task task)
{
	PyLock lock;
	CThostFtdcTransferSerialField task_data = any_cast<CThostFtdcTransferSerialField>(task.task_data);
	dict data;
	data["BankNewAccount"] = boost::locale::conv::to_utf<char>(task_data.BankNewAccount, std::string("GB2312"));
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["OperatorCode"] = boost::locale::conv::to_utf<char>(task_data.OperatorCode, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureAccType"] = task_data.FutureAccType;
	data["ErrorID"] = task_data.ErrorID;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["IdCardType"] = task_data.IdCardType;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["FutureSerial"] = task_data.FutureSerial;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["AvailabilityFlag"] = task_data.AvailabilityFlag;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTransferSerial(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryAccountregister(Task task)
{
	PyLock lock;
	CThostFtdcAccountregisterField task_data = any_cast<CThostFtdcAccountregisterField>(task.task_data);
	dict data;
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["CustType"] = task_data.CustType;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["OutDate"] = boost::locale::conv::to_utf<char>(task_data.OutDate, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["RegDate"] = boost::locale::conv::to_utf<char>(task_data.RegDate, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["TID"] = task_data.TID;
	data["OpenOrDestroy"] = task_data.OpenOrDestroy;
	data["IdCardType"] = task_data.IdCardType;
	data["TradeDay"] = boost::locale::conv::to_utf<char>(task_data.TradeDay, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryAccountregister(data, error, task.task_id, task.task_last);
};

void TdApi::processRspError(Task task)
{
	PyLock lock;
	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspError(error, task.task_id, task.task_last);
};

void TdApi::processRtnOrder(Task task)
{
	PyLock lock;
	CThostFtdcOrderField task_data = any_cast<CThostFtdcOrderField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["VolumeTraded"] = task_data.VolumeTraded;
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["RelativeOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.RelativeOrderSysID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["InstallID"] = task_data.InstallID;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["VolumeTotal"] = task_data.VolumeTotal;
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["SessionID"] = task_data.SessionID;
	data["TimeCondition"] = task_data.TimeCondition;
	data["OrderStatus"] = task_data.OrderStatus;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["OrderType"] = task_data.OrderType;
	data["UpdateTime"] = boost::locale::conv::to_utf<char>(task_data.UpdateTime, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["ActiveTime"] = boost::locale::conv::to_utf<char>(task_data.ActiveTime, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["SuspendTime"] = boost::locale::conv::to_utf<char>(task_data.SuspendTime, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["BrokerOrderSeq"] = task_data.BrokerOrderSeq;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;
	data["OrderSource"] = task_data.OrderSource;
	data["ZCETotalTradedVolume"] = task_data.ZCETotalTradedVolume;
	data["ActiveTraderID"] = boost::locale::conv::to_utf<char>(task_data.ActiveTraderID, std::string("GB2312"));

	this->onRtnOrder(data);
};

void TdApi::processRtnTrade(Task task)
{
	PyLock lock;
	CThostFtdcTradeField task_data = any_cast<CThostFtdcTradeField>(task.task_data);
	dict data;
	data["TradeType"] = task_data.TradeType;
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["Price"] = task_data.Price;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["TradeID"] = boost::locale::conv::to_utf<char>(task_data.TradeID, std::string("GB2312"));
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["BrokerOrderSeq"] = task_data.BrokerOrderSeq;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["TradeSource"] = task_data.TradeSource;
	data["PriceSource"] = task_data.PriceSource;
	data["TradingRole"] = task_data.TradingRole;

	this->onRtnTrade(data);
};

void TdApi::processErrRtnOrderInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputOrderField task_data = any_cast<CThostFtdcInputOrderField>(task.task_data);
	dict data;
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["TimeCondition"] = task_data.TimeCondition;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnOrderInsert(data, error);
};

void TdApi::processErrRtnOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcOrderActionField task_data = any_cast<CThostFtdcOrderActionField>(task.task_data);
	dict data;
	data["ActionTime"] = boost::locale::conv::to_utf<char>(task_data.ActionTime, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["OrderActionStatus"] = task_data.OrderActionStatus;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["VolumeChange"] = task_data.VolumeChange;
	data["SessionID"] = task_data.SessionID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["ActionDate"] = boost::locale::conv::to_utf<char>(task_data.ActionDate, std::string("GB2312"));
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnOrderAction(data, error);
};

void TdApi::processRtnInstrumentStatus(Task task)
{
	PyLock lock;
	CThostFtdcInstrumentStatusField task_data = any_cast<CThostFtdcInstrumentStatusField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["EnterTime"] = boost::locale::conv::to_utf<char>(task_data.EnterTime, std::string("GB2312"));
	data["SettlementGroupID"] = boost::locale::conv::to_utf<char>(task_data.SettlementGroupID, std::string("GB2312"));
	data["TradingSegmentSN"] = task_data.TradingSegmentSN;
	data["EnterReason"] = task_data.EnterReason;
	data["InstrumentStatus"] = task_data.InstrumentStatus;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));

	this->onRtnInstrumentStatus(data);
};

void TdApi::processRtnBulletin(Task task)
{
	PyLock lock;
	CThostFtdcBulletinField task_data = any_cast<CThostFtdcBulletinField>(task.task_data);
	dict data;
	data["URLLink"] = boost::locale::conv::to_utf<char>(task_data.URLLink, std::string("GB2312"));
	data["Content"] = boost::locale::conv::to_utf<char>(task_data.Content, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["Abstract"] = boost::locale::conv::to_utf<char>(task_data.Abstract, std::string("GB2312"));
	data["BulletinID"] = task_data.BulletinID;
	data["NewsUrgency"] = task_data.NewsUrgency;
	data["NewsType"] = boost::locale::conv::to_utf<char>(task_data.NewsType, std::string("GB2312"));
	data["ComeFrom"] = boost::locale::conv::to_utf<char>(task_data.ComeFrom, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["SendTime"] = boost::locale::conv::to_utf<char>(task_data.SendTime, std::string("GB2312"));
	data["MarketID"] = boost::locale::conv::to_utf<char>(task_data.MarketID, std::string("GB2312"));

	this->onRtnBulletin(data);
};

void TdApi::processRtnTradingNotice(Task task)
{
	PyLock lock;
	CThostFtdcTradingNoticeInfoField task_data = any_cast<CThostFtdcTradingNoticeInfoField>(task.task_data);
	dict data;
	data["SequenceSeries"] = task_data.SequenceSeries;
	data["SequenceNo"] = task_data.SequenceNo;
	data["FieldContent"] = boost::locale::conv::to_utf<char>(task_data.FieldContent, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["SendTime"] = boost::locale::conv::to_utf<char>(task_data.SendTime, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	this->onRtnTradingNotice(data);
};

void TdApi::processRtnErrorConditionalOrder(Task task)
{
	PyLock lock;
	CThostFtdcErrorConditionalOrderField task_data = any_cast<CThostFtdcErrorConditionalOrderField>(task.task_data);
	dict data;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["VolumeTraded"] = task_data.VolumeTraded;
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["RelativeOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.RelativeOrderSysID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["InstallID"] = task_data.InstallID;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["VolumeTotal"] = task_data.VolumeTotal;
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["SessionID"] = task_data.SessionID;
	data["TimeCondition"] = task_data.TimeCondition;
	data["OrderStatus"] = task_data.OrderStatus;
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["OrderType"] = task_data.OrderType;
	data["ErrorID"] = task_data.ErrorID;
	data["UpdateTime"] = boost::locale::conv::to_utf<char>(task_data.UpdateTime, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["ActiveTime"] = boost::locale::conv::to_utf<char>(task_data.ActiveTime, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;
	data["SuspendTime"] = boost::locale::conv::to_utf<char>(task_data.SuspendTime, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["OrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.OrderLocalID, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["BrokerOrderSeq"] = task_data.BrokerOrderSeq;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["OrderSource"] = task_data.OrderSource;
	data["ZCETotalTradedVolume"] = task_data.ZCETotalTradedVolume;
	data["ActiveTraderID"] = boost::locale::conv::to_utf<char>(task_data.ActiveTraderID, std::string("GB2312"));

	this->onRtnErrorConditionalOrder(data);
};

void TdApi::processRtnExecOrder(Task task)
{
	PyLock lock;
	CThostFtdcExecOrderField task_data = any_cast<CThostFtdcExecOrderField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["CloseFlag"] = task_data.CloseFlag;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ExecResult"] = task_data.ExecResult;
	data["ReservePositionFlag"] = task_data.ReservePositionFlag;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InstallID"] = task_data.InstallID;
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["PosiDirection"] = task_data.PosiDirection;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["ActionType"] = task_data.ActionType;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["ExecOrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderLocalID, std::string("GB2312"));
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["BrokerExecOrderSeq"] = task_data.BrokerExecOrderSeq;
	data["FrontID"] = task_data.FrontID;

	this->onRtnExecOrder(data);
};

void TdApi::processErrRtnExecOrderInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputExecOrderField task_data = any_cast<CThostFtdcInputExecOrderField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["CloseFlag"] = task_data.CloseFlag;
	data["OffsetFlag"] = task_data.OffsetFlag;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["PosiDirection"] = task_data.PosiDirection;
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ActionType"] = task_data.ActionType;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ReservePositionFlag"] = task_data.ReservePositionFlag;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnExecOrderInsert(data, error);
};

void TdApi::processErrRtnExecOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcExecOrderActionField task_data = any_cast<CThostFtdcExecOrderActionField>(task.task_data);
	dict data;
	data["ActionTime"] = boost::locale::conv::to_utf<char>(task_data.ActionTime, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["OrderActionStatus"] = task_data.OrderActionStatus;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["ExecOrderRef"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderRef, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["ActionType"] = task_data.ActionType;
	data["ActionFlag"] = task_data.ActionFlag;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExecOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ExecOrderLocalID"] = boost::locale::conv::to_utf<char>(task_data.ExecOrderLocalID, std::string("GB2312"));
	data["ActionDate"] = boost::locale::conv::to_utf<char>(task_data.ActionDate, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ExecOrderActionRef"] = task_data.ExecOrderActionRef;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnExecOrderAction(data, error);
};

void TdApi::processErrRtnForQuoteInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputForQuoteField task_data = any_cast<CThostFtdcInputForQuoteField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ForQuoteRef"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteRef, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnForQuoteInsert(data, error);
};

void TdApi::processRtnQuote(Task task)
{
	PyLock lock;
	CThostFtdcQuoteField task_data = any_cast<CThostFtdcQuoteField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["AskHedgeFlag"] = task_data.AskHedgeFlag;
	data["BidOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.BidOrderSysID, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["AskVolume"] = task_data.AskVolume;
	data["BidOrderRef"] = boost::locale::conv::to_utf<char>(task_data.BidOrderRef, std::string("GB2312"));
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["BidHedgeFlag"] = task_data.BidHedgeFlag;
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["AskOrderSysID"] = boost::locale::conv::to_utf<char>(task_data.AskOrderSysID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["AskOffsetFlag"] = task_data.AskOffsetFlag;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BidOffsetFlag"] = task_data.BidOffsetFlag;
	data["BidPrice"] = task_data.BidPrice;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["QuoteStatus"] = task_data.QuoteStatus;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["QuoteLocalID"] = boost::locale::conv::to_utf<char>(task_data.QuoteLocalID, std::string("GB2312"));
	data["BidVolume"] = task_data.BidVolume;
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["QuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.QuoteSysID, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["ForQuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteSysID, std::string("GB2312"));
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["AskPrice"] = task_data.AskPrice;
	data["SequenceNo"] = task_data.SequenceNo;
	data["AskOrderRef"] = boost::locale::conv::to_utf<char>(task_data.AskOrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["BrokerQuoteSeq"] = task_data.BrokerQuoteSeq;
	data["FrontID"] = task_data.FrontID;

	this->onRtnQuote(data);
};

void TdApi::processErrRtnQuoteInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputQuoteField task_data = any_cast<CThostFtdcInputQuoteField>(task.task_data);
	dict data;
	data["AskHedgeFlag"] = task_data.AskHedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["AskVolume"] = task_data.AskVolume;
	data["BidOrderRef"] = boost::locale::conv::to_utf<char>(task_data.BidOrderRef, std::string("GB2312"));
	data["BidHedgeFlag"] = task_data.BidHedgeFlag;
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["AskOffsetFlag"] = task_data.AskOffsetFlag;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["BidOffsetFlag"] = task_data.BidOffsetFlag;
	data["BidPrice"] = task_data.BidPrice;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["BidVolume"] = task_data.BidVolume;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["AskPrice"] = task_data.AskPrice;
	data["AskOrderRef"] = boost::locale::conv::to_utf<char>(task_data.AskOrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["ForQuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteSysID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnQuoteInsert(data, error);
};

void TdApi::processErrRtnQuoteAction(Task task)
{
	PyLock lock;
	CThostFtdcQuoteActionField task_data = any_cast<CThostFtdcQuoteActionField>(task.task_data);
	dict data;
	data["ActionTime"] = boost::locale::conv::to_utf<char>(task_data.ActionTime, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["OrderActionStatus"] = task_data.OrderActionStatus;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["QuoteRef"] = boost::locale::conv::to_utf<char>(task_data.QuoteRef, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["QuoteActionRef"] = task_data.QuoteActionRef;
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["QuoteLocalID"] = boost::locale::conv::to_utf<char>(task_data.QuoteLocalID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["QuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.QuoteSysID, std::string("GB2312"));
	data["ActionDate"] = boost::locale::conv::to_utf<char>(task_data.ActionDate, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnQuoteAction(data, error);
};

void TdApi::processRtnForQuoteRsp(Task task)
{
	PyLock lock;
	CThostFtdcForQuoteRspField task_data = any_cast<CThostFtdcForQuoteRspField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ActionDay"] = boost::locale::conv::to_utf<char>(task_data.ActionDay, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["ForQuoteSysID"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteSysID, std::string("GB2312"));
	data["ForQuoteTime"] = boost::locale::conv::to_utf<char>(task_data.ForQuoteTime, std::string("GB2312"));

	this->onRtnForQuoteRsp(data);
};

void TdApi::processRtnCFMMCTradingAccountToken(Task task)
{
	PyLock lock;
	CThostFtdcCFMMCTradingAccountTokenField task_data = any_cast<CThostFtdcCFMMCTradingAccountTokenField>(task.task_data);
	dict data;
	data["Token"] = boost::locale::conv::to_utf<char>(task_data.Token, std::string("GB2312"));
	data["KeyID"] = task_data.KeyID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	this->onRtnCFMMCTradingAccountToken(data);
};

void TdApi::processErrRtnBatchOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcBatchOrderActionField task_data = any_cast<CThostFtdcBatchOrderActionField>(task.task_data);
	dict data;
	data["ActionDate"] = boost::locale::conv::to_utf<char>(task_data.ActionDate, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["ActionTime"] = boost::locale::conv::to_utf<char>(task_data.ActionTime, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["InstallID"] = task_data.InstallID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["OrderActionStatus"] = task_data.OrderActionStatus;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnBatchOrderAction(data, error);
};

void TdApi::processRtnOptionSelfClose(Task task)
{
	PyLock lock;
	CThostFtdcOptionSelfCloseField task_data = any_cast<CThostFtdcOptionSelfCloseField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ActiveUserID"] = boost::locale::conv::to_utf<char>(task_data.ActiveUserID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ExecResult"] = task_data.ExecResult;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["SessionID"] = task_data.SessionID;
	data["OrderSubmitStatus"] = task_data.OrderSubmitStatus;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptionSelfCloseSysID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InsertTime"] = boost::locale::conv::to_utf<char>(task_data.InsertTime, std::string("GB2312"));
	data["ClearingPartID"] = boost::locale::conv::to_utf<char>(task_data.ClearingPartID, std::string("GB2312"));
	data["OptSelfCloseFlag"] = task_data.OptSelfCloseFlag;
	data["CancelTime"] = boost::locale::conv::to_utf<char>(task_data.CancelTime, std::string("GB2312"));
	data["BrokerOptionSelfCloseSeq"] = task_data.BrokerOptionSelfCloseSeq;
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["InsertDate"] = boost::locale::conv::to_utf<char>(task_data.InsertDate, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;
	data["OptionSelfCloseLocalID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseLocalID, std::string("GB2312"));

	this->onRtnOptionSelfClose(data);
};

void TdApi::processErrRtnOptionSelfCloseInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputOptionSelfCloseField task_data = any_cast<CThostFtdcInputOptionSelfCloseField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptSelfCloseFlag"] = task_data.OptSelfCloseFlag;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnOptionSelfCloseInsert(data, error);
};

void TdApi::processErrRtnOptionSelfCloseAction(Task task)
{
	PyLock lock;
	CThostFtdcOptionSelfCloseActionField task_data = any_cast<CThostFtdcOptionSelfCloseActionField>(task.task_data);
	dict data;
	data["ActionTime"] = boost::locale::conv::to_utf<char>(task_data.ActionTime, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["OrderActionStatus"] = task_data.OrderActionStatus;
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["OptionSelfCloseRef"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseRef, std::string("GB2312"));
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["OptionSelfCloseSysID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseSysID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["OptionSelfCloseActionRef"] = task_data.OptionSelfCloseActionRef;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ActionDate"] = boost::locale::conv::to_utf<char>(task_data.ActionDate, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;
	data["OptionSelfCloseLocalID"] = boost::locale::conv::to_utf<char>(task_data.OptionSelfCloseLocalID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnOptionSelfCloseAction(data, error);
};

void TdApi::processRtnCombAction(Task task)
{
	PyLock lock;
	CThostFtdcCombActionField task_data = any_cast<CThostFtdcCombActionField>(task.task_data);
	dict data;
	data["NotifySequence"] = task_data.NotifySequence;
	data["ComTradeID"] = boost::locale::conv::to_utf<char>(task_data.ComTradeID, std::string("GB2312"));
	data["UserProductInfo"] = boost::locale::conv::to_utf<char>(task_data.UserProductInfo, std::string("GB2312"));
	data["TraderID"] = boost::locale::conv::to_utf<char>(task_data.TraderID, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["ActionStatus"] = task_data.ActionStatus;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CombDirection"] = task_data.CombDirection;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["InstallID"] = task_data.InstallID;
	data["ParticipantID"] = boost::locale::conv::to_utf<char>(task_data.ParticipantID, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["SessionID"] = task_data.SessionID;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["StatusMsg"] = boost::locale::conv::to_utf<char>(task_data.StatusMsg, std::string("GB2312"));
	data["SettlementID"] = task_data.SettlementID;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombActionRef"] = boost::locale::conv::to_utf<char>(task_data.CombActionRef, std::string("GB2312"));
	data["BranchID"] = boost::locale::conv::to_utf<char>(task_data.BranchID, std::string("GB2312"));
	data["SequenceNo"] = task_data.SequenceNo;
	data["ActionLocalID"] = boost::locale::conv::to_utf<char>(task_data.ActionLocalID, std::string("GB2312"));
	data["ExchangeInstID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeInstID, std::string("GB2312"));
	data["FrontID"] = task_data.FrontID;

	this->onRtnCombAction(data);
};

void TdApi::processErrRtnCombActionInsert(Task task)
{
	PyLock lock;
	CThostFtdcInputCombActionField task_data = any_cast<CThostFtdcInputCombActionField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["CombActionRef"] = boost::locale::conv::to_utf<char>(task_data.CombActionRef, std::string("GB2312"));
	data["HedgeFlag"] = task_data.HedgeFlag;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["Volume"] = task_data.Volume;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["CombDirection"] = task_data.CombDirection;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnCombActionInsert(data, error);
};

void TdApi::processRspQryContractBank(Task task)
{
	PyLock lock;
	CThostFtdcContractBankField task_data = any_cast<CThostFtdcContractBankField>(task.task_data);
	dict data;
	data["BankName"] = boost::locale::conv::to_utf<char>(task_data.BankName, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["BankBrchID"] = boost::locale::conv::to_utf<char>(task_data.BankBrchID, std::string("GB2312"));
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryContractBank(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryParkedOrder(Task task)
{
	PyLock lock;
	CThostFtdcParkedOrderField task_data = any_cast<CThostFtdcParkedOrderField>(task.task_data);
	dict data;
	data["ContingentCondition"] = task_data.ContingentCondition;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["CombOffsetFlag"] = boost::locale::conv::to_utf<char>(task_data.CombOffsetFlag, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["UserForceClose"] = task_data.UserForceClose;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["Status"] = task_data.Status;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["Direction"] = task_data.Direction;
	data["IsSwapOrder"] = task_data.IsSwapOrder;
	data["UserType"] = task_data.UserType;
	data["VolumeTotalOriginal"] = task_data.VolumeTotalOriginal;
	data["ClientID"] = boost::locale::conv::to_utf<char>(task_data.ClientID, std::string("GB2312"));
	data["OrderPriceType"] = task_data.OrderPriceType;
	data["TimeCondition"] = task_data.TimeCondition;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["IsAutoSuspend"] = task_data.IsAutoSuspend;
	data["StopPrice"] = task_data.StopPrice;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["MinVolume"] = task_data.MinVolume;
	data["ForceCloseReason"] = task_data.ForceCloseReason;
	data["ErrorID"] = task_data.ErrorID;
	data["ParkedOrderID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CombHedgeFlag"] = boost::locale::conv::to_utf<char>(task_data.CombHedgeFlag, std::string("GB2312"));
	data["GTDDate"] = boost::locale::conv::to_utf<char>(task_data.GTDDate, std::string("GB2312"));
	data["BusinessUnit"] = boost::locale::conv::to_utf<char>(task_data.BusinessUnit, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["VolumeCondition"] = task_data.VolumeCondition;
	data["RequestID"] = task_data.RequestID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryParkedOrder(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryParkedOrderAction(Task task)
{
	PyLock lock;
	CThostFtdcParkedOrderActionField task_data = any_cast<CThostFtdcParkedOrderActionField>(task.task_data);
	dict data;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["LimitPrice"] = task_data.LimitPrice;
	data["ParkedOrderActionID"] = boost::locale::conv::to_utf<char>(task_data.ParkedOrderActionID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["Status"] = task_data.Status;
	data["MacAddress"] = boost::locale::conv::to_utf<char>(task_data.MacAddress, std::string("GB2312"));
	data["OrderActionRef"] = task_data.OrderActionRef;
	data["VolumeChange"] = task_data.VolumeChange;
	data["UserType"] = task_data.UserType;
	data["SessionID"] = task_data.SessionID;
	data["ActionFlag"] = task_data.ActionFlag;
	data["IPAddress"] = boost::locale::conv::to_utf<char>(task_data.IPAddress, std::string("GB2312"));
	data["ErrorID"] = task_data.ErrorID;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["OrderSysID"] = boost::locale::conv::to_utf<char>(task_data.OrderSysID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["OrderRef"] = boost::locale::conv::to_utf<char>(task_data.OrderRef, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["RequestID"] = task_data.RequestID;
	data["FrontID"] = task_data.FrontID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryParkedOrderAction(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryTradingNotice(Task task)
{
	PyLock lock;
	CThostFtdcTradingNoticeField task_data = any_cast<CThostFtdcTradingNoticeField>(task.task_data);
	dict data;
	data["SequenceSeries"] = task_data.SequenceSeries;
	data["SequenceNo"] = task_data.SequenceNo;
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["FieldContent"] = boost::locale::conv::to_utf<char>(task_data.FieldContent, std::string("GB2312"));
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["SendTime"] = boost::locale::conv::to_utf<char>(task_data.SendTime, std::string("GB2312"));
	data["InvestorRange"] = task_data.InvestorRange;
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryTradingNotice(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryBrokerTradingParams(Task task)
{
	PyLock lock;
	CThostFtdcBrokerTradingParamsField task_data = any_cast<CThostFtdcBrokerTradingParamsField>(task.task_data);
	dict data;
	data["MarginPriceType"] = task_data.MarginPriceType;
	data["Algorithm"] = task_data.Algorithm;
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["OptionRoyaltyPriceType"] = task_data.OptionRoyaltyPriceType;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["AvailIncludeCloseProfit"] = task_data.AvailIncludeCloseProfit;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryBrokerTradingParams(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQryBrokerTradingAlgos(Task task)
{
	PyLock lock;
	CThostFtdcBrokerTradingAlgosField task_data = any_cast<CThostFtdcBrokerTradingAlgosField>(task.task_data);
	dict data;
	data["InstrumentID"] = boost::locale::conv::to_utf<char>(task_data.InstrumentID, std::string("GB2312"));
	data["HandlePositionAlgoID"] = task_data.HandlePositionAlgoID;
	data["ExchangeID"] = boost::locale::conv::to_utf<char>(task_data.ExchangeID, std::string("GB2312"));
	data["FindMarginRateAlgoID"] = task_data.FindMarginRateAlgoID;
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["HandleTradingAccountAlgoID"] = task_data.HandleTradingAccountAlgoID;

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQryBrokerTradingAlgos(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQueryCFMMCTradingAccountToken(Task task)
{
	PyLock lock;
	CThostFtdcQueryCFMMCTradingAccountTokenField task_data = any_cast<CThostFtdcQueryCFMMCTradingAccountTokenField>(task.task_data);
	dict data;
	data["InvestorID"] = boost::locale::conv::to_utf<char>(task_data.InvestorID, std::string("GB2312"));
	data["InvestUnitID"] = boost::locale::conv::to_utf<char>(task_data.InvestUnitID, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQueryCFMMCTradingAccountToken(data, error, task.task_id, task.task_last);
};

void TdApi::processRtnFromBankToFutureByBank(Task task)
{
	PyLock lock;
	CThostFtdcRspTransferField task_data = any_cast<CThostFtdcRspTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnFromBankToFutureByBank(data);
};

void TdApi::processRtnFromFutureToBankByBank(Task task)
{
	PyLock lock;
	CThostFtdcRspTransferField task_data = any_cast<CThostFtdcRspTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnFromFutureToBankByBank(data);
};

void TdApi::processRtnRepealFromBankToFutureByBank(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromBankToFutureByBank(data);
};

void TdApi::processRtnRepealFromFutureToBankByBank(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromFutureToBankByBank(data);
};

void TdApi::processRtnFromBankToFutureByFuture(Task task)
{
	PyLock lock;
	CThostFtdcRspTransferField task_data = any_cast<CThostFtdcRspTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnFromBankToFutureByFuture(data);
};

void TdApi::processRtnFromFutureToBankByFuture(Task task)
{
	PyLock lock;
	CThostFtdcRspTransferField task_data = any_cast<CThostFtdcRspTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnFromFutureToBankByFuture(data);
};

void TdApi::processRtnRepealFromBankToFutureByFutureManual(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromBankToFutureByFutureManual(data);
};

void TdApi::processRtnRepealFromFutureToBankByFutureManual(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromFutureToBankByFutureManual(data);
};

void TdApi::processRtnQueryBankBalanceByFuture(Task task)
{
	PyLock lock;
	CThostFtdcNotifyQueryAccountField task_data = any_cast<CThostFtdcNotifyQueryAccountField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["BankUseAmount"] = task_data.BankUseAmount;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));
	data["BankFetchAmount"] = task_data.BankFetchAmount;

	this->onRtnQueryBankBalanceByFuture(data);
};

void TdApi::processErrRtnBankToFutureByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqTransferField task_data = any_cast<CThostFtdcReqTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnBankToFutureByFuture(data, error);
};

void TdApi::processErrRtnFutureToBankByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqTransferField task_data = any_cast<CThostFtdcReqTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnFutureToBankByFuture(data, error);
};

void TdApi::processErrRtnRepealBankToFutureByFutureManual(Task task)
{
	PyLock lock;
	CThostFtdcReqRepealField task_data = any_cast<CThostFtdcReqRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnRepealBankToFutureByFutureManual(data, error);
};

void TdApi::processErrRtnRepealFutureToBankByFutureManual(Task task)
{
	PyLock lock;
	CThostFtdcReqRepealField task_data = any_cast<CThostFtdcReqRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnRepealFutureToBankByFutureManual(data, error);
};

void TdApi::processErrRtnQueryBankBalanceByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqQueryAccountField task_data = any_cast<CThostFtdcReqQueryAccountField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onErrRtnQueryBankBalanceByFuture(data, error);
};

void TdApi::processRtnRepealFromBankToFutureByFuture(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromBankToFutureByFuture(data);
};

void TdApi::processRtnRepealFromFutureToBankByFuture(Task task)
{
	PyLock lock;
	CThostFtdcRspRepealField task_data = any_cast<CThostFtdcRspRepealField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["BankRepealFlag"] = task_data.BankRepealFlag;
	data["RepealedTimes"] = task_data.RepealedTimes;
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["FutureRepealSerial"] = task_data.FutureRepealSerial;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["BrokerRepealFlag"] = task_data.BrokerRepealFlag;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankRepealSerial"] = boost::locale::conv::to_utf<char>(task_data.BankRepealSerial, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["RepealTimeInterval"] = task_data.RepealTimeInterval;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["PlateRepealSerial"] = task_data.PlateRepealSerial;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnRepealFromFutureToBankByFuture(data);
};

void TdApi::processRspFromBankToFutureByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqTransferField task_data = any_cast<CThostFtdcReqTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspFromBankToFutureByFuture(data, error, task.task_id, task.task_last);
};

void TdApi::processRspFromFutureToBankByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqTransferField task_data = any_cast<CThostFtdcReqTransferField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["FeePayFlag"] = task_data.FeePayFlag;
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["TransferStatus"] = task_data.TransferStatus;
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["FutureFetchAmount"] = task_data.FutureFetchAmount;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BrokerFee"] = task_data.BrokerFee;
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Message"] = boost::locale::conv::to_utf<char>(task_data.Message, std::string("GB2312"));
	data["CustFee"] = task_data.CustFee;
	data["TradeAmount"] = task_data.TradeAmount;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspFromFutureToBankByFuture(data, error, task.task_id, task.task_last);
};

void TdApi::processRspQueryBankAccountMoneyByFuture(Task task)
{
	PyLock lock;
	CThostFtdcReqQueryAccountField task_data = any_cast<CThostFtdcReqQueryAccountField>(task.task_data);
	dict data;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["RequestID"] = task_data.RequestID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));
	data["IdCardType"] = task_data.IdCardType;
	data["PlateSerial"] = task_data.PlateSerial;
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["FutureSerial"] = task_data.FutureSerial;
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	CThostFtdcRspInfoField task_error = any_cast<CThostFtdcRspInfoField>(task.task_error);
	dict error;
	error["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_error.ErrorMsg, std::string("GB2312"));
	error["ErrorID"] = task_error.ErrorID;

	this->onRspQueryBankAccountMoneyByFuture(data, error, task.task_id, task.task_last);
};

void TdApi::processRtnOpenAccountByBank(Task task)
{
	PyLock lock;
	CThostFtdcOpenAccountField task_data = any_cast<CThostFtdcOpenAccountField>(task.task_data);
	dict data;
	data["MoneyAccountStatus"] = task_data.MoneyAccountStatus;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["Telephone"] = boost::locale::conv::to_utf<char>(task_data.Telephone, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["Fax"] = boost::locale::conv::to_utf<char>(task_data.Fax, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["CountryCode"] = boost::locale::conv::to_utf<char>(task_data.CountryCode, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["Address"] = boost::locale::conv::to_utf<char>(task_data.Address, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["Gender"] = task_data.Gender;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CashExchangeCode"] = task_data.CashExchangeCode;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["MobilePhone"] = boost::locale::conv::to_utf<char>(task_data.MobilePhone, std::string("GB2312"));
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["ZipCode"] = boost::locale::conv::to_utf<char>(task_data.ZipCode, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["EMail"] = boost::locale::conv::to_utf<char>(task_data.EMail, std::string("GB2312"));
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));

	this->onRtnOpenAccountByBank(data);
};

void TdApi::processRtnCancelAccountByBank(Task task)
{
	PyLock lock;
	CThostFtdcCancelAccountField task_data = any_cast<CThostFtdcCancelAccountField>(task.task_data);
	dict data;
	data["MoneyAccountStatus"] = task_data.MoneyAccountStatus;
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["UserID"] = boost::locale::conv::to_utf<char>(task_data.UserID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["Telephone"] = boost::locale::conv::to_utf<char>(task_data.Telephone, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["Fax"] = boost::locale::conv::to_utf<char>(task_data.Fax, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["CountryCode"] = boost::locale::conv::to_utf<char>(task_data.CountryCode, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["Address"] = boost::locale::conv::to_utf<char>(task_data.Address, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["Gender"] = task_data.Gender;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["OperNo"] = boost::locale::conv::to_utf<char>(task_data.OperNo, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BankSecuAcc"] = boost::locale::conv::to_utf<char>(task_data.BankSecuAcc, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["CashExchangeCode"] = task_data.CashExchangeCode;
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["MobilePhone"] = boost::locale::conv::to_utf<char>(task_data.MobilePhone, std::string("GB2312"));
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["ZipCode"] = boost::locale::conv::to_utf<char>(task_data.ZipCode, std::string("GB2312"));
	data["BankSecuAccType"] = task_data.BankSecuAccType;
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["EMail"] = boost::locale::conv::to_utf<char>(task_data.EMail, std::string("GB2312"));
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));
	data["DeviceID"] = boost::locale::conv::to_utf<char>(task_data.DeviceID, std::string("GB2312"));

	this->onRtnCancelAccountByBank(data);
};

void TdApi::processRtnChangeAccountByBank(Task task)
{
	PyLock lock;
	CThostFtdcChangeAccountField task_data = any_cast<CThostFtdcChangeAccountField>(task.task_data);
	dict data;
	data["MoneyAccountStatus"] = task_data.MoneyAccountStatus;
	data["NewBankPassWord"] = boost::locale::conv::to_utf<char>(task_data.NewBankPassWord, std::string("GB2312"));
	data["BrokerBranchID"] = boost::locale::conv::to_utf<char>(task_data.BrokerBranchID, std::string("GB2312"));
	data["BankPassWord"] = boost::locale::conv::to_utf<char>(task_data.BankPassWord, std::string("GB2312"));
	data["Telephone"] = boost::locale::conv::to_utf<char>(task_data.Telephone, std::string("GB2312"));
	data["IdentifiedCardNo"] = boost::locale::conv::to_utf<char>(task_data.IdentifiedCardNo, std::string("GB2312"));
	data["VerifyCertNoFlag"] = task_data.VerifyCertNoFlag;
	data["TID"] = task_data.TID;
	data["AccountID"] = boost::locale::conv::to_utf<char>(task_data.AccountID, std::string("GB2312"));
	data["BankAccount"] = boost::locale::conv::to_utf<char>(task_data.BankAccount, std::string("GB2312"));
	data["Fax"] = boost::locale::conv::to_utf<char>(task_data.Fax, std::string("GB2312"));
	data["InstallID"] = task_data.InstallID;
	data["SecuPwdFlag"] = task_data.SecuPwdFlag;
	data["CustomerName"] = boost::locale::conv::to_utf<char>(task_data.CustomerName, std::string("GB2312"));
	data["CountryCode"] = boost::locale::conv::to_utf<char>(task_data.CountryCode, std::string("GB2312"));
	data["TradeCode"] = boost::locale::conv::to_utf<char>(task_data.TradeCode, std::string("GB2312"));
	data["BankBranchID"] = boost::locale::conv::to_utf<char>(task_data.BankBranchID, std::string("GB2312"));
	data["SessionID"] = task_data.SessionID;
	data["NewBankAccount"] = boost::locale::conv::to_utf<char>(task_data.NewBankAccount, std::string("GB2312"));
	data["Address"] = boost::locale::conv::to_utf<char>(task_data.Address, std::string("GB2312"));
	data["PlateSerial"] = task_data.PlateSerial;
	data["BankPwdFlag"] = task_data.BankPwdFlag;
	data["ErrorID"] = task_data.ErrorID;
	data["CustType"] = task_data.CustType;
	data["LongCustomerName"] = boost::locale::conv::to_utf<char>(task_data.LongCustomerName, std::string("GB2312"));
	data["Gender"] = task_data.Gender;
	data["BankID"] = boost::locale::conv::to_utf<char>(task_data.BankID, std::string("GB2312"));
	data["BankSerial"] = boost::locale::conv::to_utf<char>(task_data.BankSerial, std::string("GB2312"));
	data["TradingDay"] = boost::locale::conv::to_utf<char>(task_data.TradingDay, std::string("GB2312"));
	data["BrokerID"] = boost::locale::conv::to_utf<char>(task_data.BrokerID, std::string("GB2312"));
	data["IdCardType"] = task_data.IdCardType;
	data["Password"] = boost::locale::conv::to_utf<char>(task_data.Password, std::string("GB2312"));
	data["MobilePhone"] = boost::locale::conv::to_utf<char>(task_data.MobilePhone, std::string("GB2312"));
	data["TradeDate"] = boost::locale::conv::to_utf<char>(task_data.TradeDate, std::string("GB2312"));
	data["CurrencyID"] = boost::locale::conv::to_utf<char>(task_data.CurrencyID, std::string("GB2312"));
	data["ErrorMsg"] = boost::locale::conv::to_utf<char>(task_data.ErrorMsg, std::string("GB2312"));
	data["BankAccType"] = task_data.BankAccType;
	data["LastFragment"] = task_data.LastFragment;
	data["ZipCode"] = boost::locale::conv::to_utf<char>(task_data.ZipCode, std::string("GB2312"));
	data["BrokerIDByBank"] = boost::locale::conv::to_utf<char>(task_data.BrokerIDByBank, std::string("GB2312"));
	data["TradeTime"] = boost::locale::conv::to_utf<char>(task_data.TradeTime, std::string("GB2312"));
	data["EMail"] = boost::locale::conv::to_utf<char>(task_data.EMail, std::string("GB2312"));
	data["Digest"] = boost::locale::conv::to_utf<char>(task_data.Digest, std::string("GB2312"));

	this->onRtnChangeAccountByBank(data);
};

