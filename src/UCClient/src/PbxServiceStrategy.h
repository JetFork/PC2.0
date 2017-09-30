#ifndef PBX_SERVICE_STRATEGY_H_
#define PBX_SERVICE_STRATEGY_H_

#include "IPbxService.h"
#include "CefClient.h"
#include "assist/PbxServiceJSEventAssist.h"

namespace ucclient {

using namespace uc;

class PbxServiceStrategy : public IPbxServiceEvent {
public:
	PbxServiceStrategy(IUCEngine *engine, cefIntegration::CefClient *cefClient);
	virtual ~PbxServiceStrategy();

public:
	// 
	virtual int32_t CheckLogin(int &nRet);
	// VoIP����
	virtual int32_t MakeCall(const std::string &userName, int userID, const std::string &number,
		int callType, const std::string &postValue, __int64 &timeStamp, int32_t &taskID);
	// ����Ӧ��
	virtual int32_t AnswerCall(int32_t taskID, int callType);
	// �ܽ�����
	virtual int32_t RejectCall(int32_t taskID);
	// �Ҷ�ͨ��
	virtual int32_t HangupCall(int32_t taskID);
	// ͨ������
	virtual int32_t HoldCall(int32_t taskID);
	// ���ͨ������
	virtual int32_t UnHoldCall(int32_t taskID);
	// ͨ������
	virtual int32_t MuteCall(int32_t taskID);
	// ���ͨ������
	virtual int32_t UnMuteCall(int32_t taskID);
	// ����ͨ����˷�����
	virtual int32_t AdjustMicLevel(int32_t taskID, int level); 
	// ˢ����˷�
	virtual int32_t RefreshMic(int &micState);
	// ����DTMF
	virtual int32_t SendDTMF(int32_t taskID, const std::string &dtmf);
	// ��ȡ��ǰPBX����
	virtual int32_t GetCurrentPBXAccount(std::string &pbxAccount);

	virtual int32_t IncomingCallUserName(int32_t taskID, std::string &userName);

	virtual int32_t GetCallRecordLst(int32_t &count, int64_t timeStamp);

	virtual int32_t GetCallDetailsLst(const std::string &number, int userID, int64_t timeStamp, int count);

	virtual int32_t SearchCallRecord(const std::string &keyWord, int64_t timeStamp, int count);

	virtual int32_t SaveCallDetails(int64_t timeStamp, int taskID, const std::string &number,
		int userID, const std::string &name, int incoming, int type, int state, int druation);

	// ͨ���п�ʼ��ֹͣ¼��
	virtual int32_t SwitchRecordInCall(int recordSwitch, const std::string &callUUID);
	// ˫�����
	virtual int32_t MakeTwoWayCall(const std::string &toNumber, const std::string &fromNumber, int openRecord);
	// ��ȡ����¼���б�Ŀ¼
	virtual int32_t GetAllRecord(int recordType);
	// ��ȡ����ͨ��¼��
	virtual int32_t GetRecord(int recordType, const std::string &callUUID);

public:
	virtual void OnLoginPbx(int32_t errorCode);

	virtual void OnStatusChanged(__int64 timeStamp, int32_t taskID, int callState,
		const char* resultCode, const char* callUUID);

	virtual void OnMakeCall(int32_t errorCode, __int64 timeStamp, 
		int32_t taskID, int micState, const char* toNumber);

	virtual void OnCallIncoming(__int64 timeStamp, int32_t taskID, int callType,
		const char* fromNumber, const char* postValue);

	virtual void OnServiceStateChanged(int stateCode);

	virtual void OnCallHangup(const AC_PbxCallRecord &data);

	virtual void OnGetCallRecordLst(int32_t errorCode, const PbxCallRecordList &lstData);

	virtual void OnGetCallDetailsLst(int32_t errorCode, const PbxCallRecordList &lstData);

	virtual void OnSearchCallRecord(int32_t errorCode, const PbxCallRecordList &lstData);

private:
	cefIntegration::CefClient *_cefClient;
	IPbxService *_lpService;
	IUCEngine *_engine;
	PbxServiceJSEventAssist _jsEventAssist;
};

}

#endif  PBX_SERVICE_STRATEGY_H_ 
