#ifndef PBXSERVICE_H_
#define PBXSERVICE_H_
#pragma once

#include "IPbxService.h"

#include "EasiioLibrary.h"
#include "base/Timer.h"

#include "PbxErrorCode.h"
#include "TasksMgr.h"

#ifdef PBX_SERVICE
namespace uc {
	class PbxService : public IPbxService, IUCEngineEvent
	{
	public:
		PbxService(IUCEngine *engine);
		virtual ~PbxService();

		static PbxService* _lpPbxSrv;

	public:

	protected:
		enum CALL_STATE {
			CALL_STATE_REJECT = CALL_STATE_DISCONNECTED + 1, // �ܽ�����
			CALL_STATE_HANGUP, // �����Ҷ�ͨ��
		};

		enum LOGIN_RET {
			LR_SUCCESS,
			LR_PBXSRV = LOGIN_FAILED_LIBRARY_ERROR + 1, // pbx���������ش���
			LR_RELOGIN, // ����
			LR_OAUTH, // �˺���֤ʧ��
			LR_PARAM, // ��������
			LR_TOKEN, // ��ȡtoken
			LR_WAITING, // 
			LR_NOTOPEN, // 
		};

	protected:
		// IPbxService
		virtual void ConnectPbx(int32_t userID, const std::string &token, int expiresIn);
		virtual void SetProxyUrl(const std::string& proxyUrl);
		virtual void SetProxyInfo(int32_t auth_type, const std::string& auth_account, const std::string& auth_passwd);

		virtual int32_t CheckLogin(int &nRet);
		// VoIP����
		virtual int32_t MakeCall(const std::string &userName, int userID, const std::string &number,
			int callType, const std::string &postValue, __int64 &timeStamp, int32_t &taskID);
		// ����Ӧ��
		virtual int32_t AnswerCall(int taskID, int callType);
		// �ܽ�����
		virtual int32_t RejectCall(int taskID);
		// �Ҷ�ͨ��
		virtual int32_t HangupCall(int taskID);
		// ͨ������
		virtual int32_t HoldCall(int taskID);
		// ���ͨ������
		virtual int32_t UnHoldCall(int taskID);
		// ͨ������
		virtual int32_t MuteCall(int taskID);
		// ���ͨ������
		virtual int32_t UnMuteCall(int taskID);

		// ����ͨ����˷�����
		virtual int32_t AdjustMicLevel(int taskID, int level);
		// ˢ����˷�
		virtual int32_t RefreshMic(int &micState);

		// ����DTMF
		virtual int32_t SendDTMF(int taskID, const std::string &dtmf);
		// ��ȡ��ǰPBX����
		virtual int32_t GetCurrentPBXAccount(std::string &pbxAccount);
		//////////////////////////////////////////////////////////////////////////
		virtual int32_t IncomingCallUserName(int taskID, std::string &userName);

		virtual int32_t GetCallRecordLst(int32_t& count, int64_t timeStamp);

		virtual int32_t GetCallDetailsLst(const std::string &number, int userID, int64_t timeStamp, int count);

		virtual int32_t SearchCallRecord(const std::string &keyWord, int64_t timeStamp, int count);

		virtual int32_t SaveCallDetails(int64_t timeStamp, int taskID, const std::string &number,
			int userID, const std::string &name, int incoming, int type, int state, int druation);
		//////////////////////////////////////////////////////////////////////////

	protected:
		int _LoginPbx();
		void _LogoutPbx();

		int _MicRet(int ret);

	protected:
		// IUCEngineEvent
		// ucas���ӵ��¼�
		virtual void OnUCASConnected(int32_t errorCode);
		// ucas�Ͽ����ӵĻص��¼�
		virtual void OnUCASDisconnected(int32_t errorCode);
		// ��½�ɹ��Ļص��¼�
		virtual void OnLoginSucceeded();
		// �ǳ��ɹ��Ļص��¼�
		virtual void OnLogoutSucceeded();
		//////////////////////////////////////////////////////////////////////////

	private:
		// EASIIO_CALLBACK_INTERFACE
		static void OnLoginResult(int resultCode, const char* resultMsg); // ��¼���

		static void OnCallStatusChanged(int callID, int callState,
			const char* resultCode, const char* callUUID); // ͨ��״̬�仯֪ͨ

		static void OnMakeCallResult(int callID, const char* toNumber); // VoIP����֪ͨ

		static void OnIncomingCall(int callID, int callType,
			const char* fromNumber, const char* postValue); // ����֪ͨ

		static void OnPJSIPServiceStateChanged(int stateCode); // PJSIP״̬�仯֪ͨ

	private:
		int InitPbx();
		void DestroyPbx();

		int32_t CreateCallInfo(int status, int callType, const std::string &userName,
			int userID, const std::string &number, const char* postValue);
		int SaveCallID(const std::string &number, int callID);
		bool GetCallKey(const std::string &number, __int64 &timeStamp, int32_t &taskID);

		int UpCallHold(int callID, bool isHold);
		int UpCallMute(int callID, bool isMute);

		bool UpCallStatus(int callID, int status, const char* callUUID, 
			int32_t &taskID, __int64 &opTime);

		void NewData();
		void DeleteData();

		void ReLogin(int expiresIn);
		void onTimeReLogin();
		void CancelTimer();

		void GetToken();

		void InitConsoleWindow();

	private:
		bool _isInit;
		bool _reLogin;
		bool _isConnected;
		int _loginRet;

		int _paramsCount;
		EasiioLoginParams* _lpParams;
		EASIIO_CALLBACK_INTERFACE *_lpCBInterface;

		PbxTasks _tasks;

		timer_id _timerId;

		std::string _account;

		std::string _serverUrlKey;
		std::string _serverUrlValue;

		std::string _accessTokenKey;
		std::string _accessTokenValue;

		std::string _logDirA;

		bool _login;

	protected:
		// ˫�����
		virtual int32_t MakeTwoWayCall(const std::string &toNumber,
			const std::string &fromNumber, int openRecord);
		static void OnMakeTwoWayCallResult(EasiioResponseReason reason, const char* caller,
			const char* callee, const char* callUUID, const char* retryId); // ˫����н��
		static void OnTwoWayCallStatusChanged(int callState, const char* callUUID,
			const char* caller, const char* callee, int retrying); // ˫�����ͨ��״̬֪ͨ

		// ͨ���п�ʼ��ֹͣ¼��
		virtual int32_t SwitchRecordInCall(int recordSwitch, const std::string &callUUID);
		static void OnCallRecordingReaponse(EasiioResponseReason reason,
			int switchRecord, const char* callUUID); // ͨ���п�����ֹͣ¼�����ý��

		// ��ȡ����¼���б�Ŀ¼
		virtual int32_t GetAllRecord(int recordType);
		// ��ȡ����ͨ��¼��
		virtual int32_t GetRecord(int recordType, const std::string &callUUID);
		static void OnGetAllRecordResponse(EasiioResponseReason reason,
			int count, EasiioRecord *record); // ��ȡ����¼���ص���� 
		static void OnGetRecordResponse(EasiioResponseReason reason,
			const char* callUUID, int count, EasiioRecordItem *recordItem); // ��ȡ����ͨ��¼���ص����
	};
}
#endif // PBX_SERVICE

#endif /* PBXSERVICE_H_ */