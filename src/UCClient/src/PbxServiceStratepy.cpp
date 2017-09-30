#include "PbxServiceStrategy.h"
#include "UCDefs.h"
#include "LogManager.h"

namespace ucclient {

	PbxServiceStrategy::PbxServiceStrategy(IUCEngine *engine, cefIntegration::CefClient *cefClient)
		: _engine(engine), _cefClient(cefClient), _jsEventAssist(cefClient) {

#ifdef PBX_SERVICE
		_lpService = (IPbxService*)_engine->QueryService(UCPbxService);
#endif // PBX_SERVICE
	}

	PbxServiceStrategy::~PbxServiceStrategy() {
	}

	// UCExtension::PbxServiceMethodProcess
	int32_t PbxServiceStrategy::CheckLogin(int &nRet) {
		return _lpService->CheckLogin(nRet);
	}

	// VoIP����
	int32_t PbxServiceStrategy::MakeCall(const std::string &userName, int userID, 
		const std::string &number, int callType, const std::string &postValue, 
		__int64 &timeStamp, int32_t &taskID) {
		return _lpService->MakeCall(userName, userID, number, callType, 
			postValue, timeStamp, taskID);
	}

	// ˫�����
	int32_t PbxServiceStrategy::MakeTwoWayCall(const std::string &toNumber,
		const std::string &fromNumber, int openRecord) {
		return _lpService->MakeTwoWayCall(toNumber, fromNumber, openRecord);
	}

	// ����Ӧ��
	int32_t PbxServiceStrategy::AnswerCall(int32_t taskID, int callType) {
		return _lpService->AnswerCall(taskID, callType);
	}

	// �ܽ�����
	int32_t PbxServiceStrategy::RejectCall(int32_t taskID) {
		return _lpService->RejectCall(taskID);
	}

	// �Ҷ�ͨ��
	int32_t PbxServiceStrategy::HangupCall(int32_t taskID) {
		return _lpService->HangupCall(taskID);
	}

	// ͨ������
	int32_t PbxServiceStrategy::HoldCall(int32_t taskID) {
		return _lpService->HoldCall(taskID);
	}

	// ���ͨ������
	int32_t PbxServiceStrategy::UnHoldCall(int32_t taskID) {
		return _lpService->UnHoldCall(taskID);
	}
	// ͨ������
	int32_t PbxServiceStrategy::MuteCall(int32_t taskID) {
		return _lpService->MuteCall(taskID);
	}

	// ���ͨ������
	int32_t PbxServiceStrategy::UnMuteCall(int32_t taskID) {
		return _lpService->UnMuteCall(taskID);
	}

	// ����ͨ����˷�����
	int32_t PbxServiceStrategy::AdjustMicLevel(int32_t taskID, int level) {
		return _lpService->AdjustMicLevel(taskID, level);
	}

	// ˢ����˷�
	int32_t PbxServiceStrategy::RefreshMic(int &micState) {
		return _lpService->RefreshMic(micState);
	}

	// ����DTMF
	int32_t PbxServiceStrategy::SendDTMF(int32_t taskID, const std::string &dtmf) {
		return _lpService->SendDTMF(taskID, dtmf);
	}

	// ͨ���п�ʼ��ֹͣ¼��
	int32_t PbxServiceStrategy::SwitchRecordInCall(int recordSwitch, const std::string &callUUID) {
		return _lpService->SwitchRecordInCall(recordSwitch, callUUID);
	}

	// ��ȡ��ǰPBX����
	int32_t PbxServiceStrategy::GetCurrentPBXAccount(std::string &pbxAccount) {
		return _lpService->GetCurrentPBXAccount(pbxAccount);
	}

	// ��ȡ����¼���б�Ŀ¼
	int32_t PbxServiceStrategy::GetAllRecord(int recordType) {
		return _lpService->GetAllRecord(recordType);
	}

	// ��ȡ����ͨ��¼��
	int32_t PbxServiceStrategy::GetRecord(int recordType, const std::string &callUUID) {
		return _lpService->GetRecord(recordType, callUUID);
	}

	//////////////////////////////////////////////////////////////////////////
	int32_t PbxServiceStrategy::IncomingCallUserName(int32_t taskID, std::string &userName) {
		return _lpService->IncomingCallUserName(taskID, userName);
	}

	int32_t PbxServiceStrategy::GetCallRecordLst(int32_t& count, int64_t timeStamp) {
		return _lpService->GetCallRecordLst(count, timeStamp);
	}

	int32_t PbxServiceStrategy::GetCallDetailsLst(const std::string &number, int userID, int64_t timeStamp, int count) {
		return _lpService->GetCallDetailsLst(number, userID, timeStamp, count);
	}

	int32_t PbxServiceStrategy::SearchCallRecord(const std::string &keyWord, int64_t timeStamp, int count) {
		return _lpService->SearchCallRecord(keyWord, timeStamp, count);
	}

	int32_t PbxServiceStrategy::SaveCallDetails(int64_t timeStamp, int taskID, const std::string &number,
		int userID, const std::string &name, int incoming, int type, int state, int druation) {
		return _lpService->SaveCallDetails(timeStamp, taskID, number,
			userID, name, incoming, type, state, druation);
	}
	//////////////////////////////////////////////////////////////////////////
	void PbxServiceStrategy::OnLoginPbx(int32_t errorCode){

		std::string parameters = _jsEventAssist.MakeParametersForOnLoginPbx(
			errorCode);

		cout << "OnLoginPbx to js parameters=" << parameters << endl;
		UC_LOG(INFO) << "OnLoginPbx to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_LOGINPBX_EVENT, parameters);
	}

	void PbxServiceStrategy::OnServiceStateChanged(int stateCode) {
		std::string parameters = _jsEventAssist.MakeParametersForOnServiceStateChanged(stateCode);

		UC_LOG(INFO) << "OnServiceStateChanged to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_SERVICESTATECHANGED_EVENT, parameters);
	}

	void PbxServiceStrategy::OnStatusChanged(__int64 timeStamp, int32_t taskID, int callState,
		const char* resultCode, const char* callUUID) {
		std::string parameters = _jsEventAssist.MakeParametersForOnStatusChanged(
			timeStamp, taskID, callState, resultCode, callUUID);

		UC_LOG(INFO) << "OnStatusChanged to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_STATUSCHANGED_EVENT, parameters);
	}

	void PbxServiceStrategy::OnMakeCall(int32_t errorCode, __int64 timeStamp, 
		int32_t taskID, int micState, const char* toNumber) {
		std::string parameters = _jsEventAssist.MakeParametersForOnMakeCall(
			errorCode, timeStamp, taskID, micState, toNumber);

		UC_LOG(INFO) << "���� to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_MAKECALL_EVENT, parameters);
	}

	void PbxServiceStrategy::OnCallIncoming(__int64 timeStamp, int32_t taskID, int callType,
		const char* fromNumber, const char* postValue) {
		std::string parameters = _jsEventAssist.MakeParametersForOnCallIncoming(
			timeStamp, taskID, callType, fromNumber, postValue);

		UC_LOG(INFO) << "�Է����� to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_INCOMINGCALL_EVENT, parameters);
	}

	void PbxServiceStrategy::OnCallHangup(const AC_PbxCallRecord &data) {
		std::string parameters = _jsEventAssist.MakeParametersForOnCallHangup(data);

		UC_LOG(INFO) << "�Ҷ�֪ͨ to js parameters=" << parameters;
		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_HANGUPCALL_EVENT, parameters);
	}

	void PbxServiceStrategy::OnGetCallRecordLst(int32_t errorCode, const PbxCallRecordList &lstData) {
		std::string parameters = _jsEventAssist.MakeParametersForOnGetCallRecordLst(errorCode, lstData);

// 		UC_LOG(INFO) << "OnGetCallRecordLst to js parameters=" << parameters;
// 		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_GETCALLRECORDLST_EVENT, parameters);
	}

	void PbxServiceStrategy::OnGetCallDetailsLst(int32_t errorCode, const PbxCallRecordList &lstData) {
		std::string parameters = _jsEventAssist.MakeParametersForOnGetCallDetailsLst(errorCode, lstData);

// 		UC_LOG(INFO) << "OnGetCallDetailsLst to js parameters=" << parameters;
// 		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_GETCALLDETAILSLST_EVENT, parameters);
	}

	void PbxServiceStrategy::OnSearchCallRecord(int32_t errorCode, const PbxCallRecordList &lstData) {
		std::string parameters = _jsEventAssist.MakeParametersForOnSearchCallRecord(errorCode, lstData);

// 		UC_LOG(INFO) << "OnSearchCallRecord to js parameters=" << parameters;
// 		// UC_LOG(INFO) << endl;
		_jsEventAssist.MakeJSEventAndExecute(PBX_SERVICE_SEARCHCALLRECORD_EVENT, parameters);

	}
}