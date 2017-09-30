/*
* IAudioService.h
*
*  Created on: May 6, 2015
*      Author: ethan
*/

#ifndef IAUDIOSERVICE_H_
#define IAUDIOSERVICE_H_

#include "UCSDK.h"
#include "IUCEngine.h"
#include "ITangUser.h"
#include "IConferenceEvent.h"
#include "IConference.h"
#include "IGroupService.h"
#include "audio_service/CallDef.h"

namespace uc {
	interface UC_API IAudioServiceEvent;

	interface UC_API IAudioService: public IUCService, IConference, Event<IAudioServiceEvent> {
		virtual ~IAudioService() {};

		/**
		* ����ָ��ID������
		* @param id			������ID�����û�ID
		* @param type		Project: 1, Group: 2, Single: 3
		*/
		virtual int32_t CreateCallById(UCID id, int32_t type) = 0;

		/**
		* ������
		* @param groupName			����������
		* @param logourl			������ͷ���ַ
		* @param userInfo			�������Ա�б�
		*/
		virtual int32_t CreateCallByUserList(const std::wstring& groupName, std::vector<UserInfo>& userInfo) = 0;

		/**
		* �����
		* @param callType Project: 1, Group: 2, Single: 3
		* @param tempConferenceId	����ID
		* @param groupId			������ID����������������м��������Ҫ��д��ֵ
		*/
		virtual int32_t JoinCall(int callType, const std::wstring& tempConferenceId, int64_t groupId = 0) = 0;

		/**
		* �˳���
		* @param tempConferenceId	������ID
		*/
		virtual int32_t LeaveCall(const std::wstring& tempConferenceId) = 0;

		/**
		* ���������û����뵱ǰһ��һcall
		* @param callType Project: 1, Group: 2, Single: 3
		* @param groupName ����������
		* @param userInfo �û��б�
		* @param conferenceId ������Ļ���ID
		*/
		virtual int32_t InviteUsersToSingleCall(int callType, const std::wstring& groupName, std::vector<UserInfo>& userInfo, const std::wstring& conferenceId) = 0;

		/**
		* ���������û����뵱ǰȺcall
		* @param callType Project: 1, Group: 2, Single: 3
		* @param userInfo �û��б�
		* @param groupId  ������ȺID
		* @param conferenceId ������Ļ���ID
		*/
		virtual int32_t InviteUsersToGroupCall(int callType, std::vector<UserInfo>& userInfo, int groupId, const std::wstring& conferenceId) = 0;

		/**
		* ��ȡ�û���״̬
		* @param userIds �û�ID
		*/
		virtual int32_t GetUserCallStatus(std::vector<int32_t>& userIds, int32_t id, int32_t type) = 0;

		/**
		* ��ȡ��ʷ������
		*/
		virtual int32_t GetHistoryPhoneNum() = 0;

		/**
		* @brief �����һ�κ���Ϣ
		*/
		virtual int32_t GetLastAudioInvite(int32_t userId, int32_t requestId) = 0;

		/**
		* @brief ��ȡ����������
		*/
		virtual int32_t GetSpeakerNum() = 0;

		/**
		* @brief ��ȡ��˷�����
		*/
		virtual int32_t GetMicrophoneNum() = 0;

		/**
		* @brief ��ȡ�������б�
		*/
		virtual std::wstring GetSpeakerList() = 0;

		/**
		* @brief ��ȡ��˷��б�
		*/
		virtual std::wstring GetMicrophoneList() = 0;

		/**
		* @brief ���������
		*/
		virtual int32_t StartSpeakerDiagnosis(const std::wstring& speaker) = 0;

		/**
		* @brief ��˷����
		*/
		virtual int32_t StartMikeDiagnosis(const std::wstring& mike) = 0;

		/**
		* @brief ֹͣ���������
		*/
		virtual int32_t StopSpeakerDiagnosis() = 0;

		/**
		* @brief ֹͣ��˷����
		*/
		virtual int32_t StopMikeDiagnosis() = 0;

		/**
		* @brief ��ȡ����ͷ����
		*/
		virtual int32_t GetCameraNum() = 0;

		/**
		* @brief ��ȡ����ͷ�б�
		*/
		virtual std::wstring GetCameraList() = 0;

		/**
		* @brief �ж�����ͷ�Ƿ����
		*/
		virtual bool IsCameraAvailable() = 0;

		/**
		* @brief ������ƵԤ��
		*/
		virtual int32_t CameraDiagnosis(const std::wstring& camera, void *pWindow) = 0;

		/**
		* @brief ������ƵԤ��
		*/
		virtual int32_t StopCameraDiagnosis() = 0;
	};

	interface UC_API IAudioServiceEvent : public IConferenceEvent {
		virtual ~IAudioServiceEvent() {};

		/**
		* ����������ķ����¼�
		* @param errorCode				������
		* @param tempConferenceId		������Id
		* @param tempUserId				���û�Id
		* @param groupId			    ������Id
		*/
		virtual void OnCallCreated(int32_t errorCode, int type, int tempConferenceId, int Id) = 0;

		/**
		* ���������ķ����¼�
		* @param errorCode				������
		* @param tempConferenceId		������Id
		* @param tempUserId				���û�Id
		* @param groupId				������Id
		*/
		virtual void OnCallJoined(int32_t errorCode, int tempConferenceId, int tempUserId, int groupId) = 0;

		/**
		* �뿪������ķ����¼�
		* @param errorCode				������
		* @param tempConferenceId		������Id
		* @param callType				0:һ��һ�� 1:Ⱥ��
		* @param userId					�û�ID��ȺID
		*/
		virtual void OnCallLeaved(int32_t errorCode, int tempConferenceId, int callType, int userId) = 0;

		/**
		* VOIP���������ص��¼�
		* @param conferenceId		������Id
		* @param callType			0:һ��һ�� 1:Ⱥ��
		* @param userId				�û�ID
		* @param quality			�������� 0:�ǳ���;1:�ܺ�;2:��ͨ;3:��;4:�ܲ�
		*/
		virtual void OnNetworkQualityChanged(int conferenceId, int callType, int userId, int quality) = 0;

		/**
		* ��˷�״̬����ص��¼�
		* @param type				0��level 1��volume
		* @param value			    ��С
		*/
		virtual void OnMicrophoneStatusChanged(int type, int value) = 0;

		/**
		* ������״̬����ص��¼�
		* @param type				0��level 1��volume
		* @param value			    ��С
		*/
		virtual void OnSpeakerStatusChanged(int type, int value) = 0;

		/**
		* ��Ƶ�豸״̬���
		* @param isReady true������Ƶ�豸 false������Ƶ�豸
		*/
		virtual void OnAudioDeviceStatusChanged(bool isReady) = 0;

		/**
		* �����û����뵱ǰȺ���ص��¼�
		*/
		virtual void OnAddMemberToGroupCall(int32_t errorCode, int callType, int tempConferenceId, int id) = 0;

		/**
		* �����û����뵱ǰһ��һ���ص��¼�
		*/
		virtual void OnAddMemberToSingleCall(int32_t errorCode, int callType, int tempConferenceId, int id) = 0;

		/**
		* ��ȡ�û���״̬�ص��¼�
		*/
		virtual void OnUserCallStatusGot(int32_t errorCode, const vector<UserCallStatus>& userStatus, int32_t id, int32_t type) = 0;

		/**
		* ��ȡ�û�����ʷ�绰����
		*/
		virtual void OnHistoryPhoneNumGot(int32_t errorCode, const std::list<string>& phoneNums) = 0;

		virtual void OnSpeakerLevelGot(const std::wstring& speaker, int level) = 0;

		virtual void OnMikeLevelGot(const std::wstring& mike, int level) = 0;

		virtual void OnSpeakerPlayFinished(const std::wstring& speaker) = 0;

		virtual void OnInviteCallSend(int32_t errorCode, int32_t userId, int32_t conferenceId, int callType, int32_t chatType) = 0;

		virtual void OnLastAudioInviteGot(int32_t errorCode, int32_t conferenceId, int32_t from, int32_t to, int32_t type, int32_t requestId) = 0;
	};
}

#endif /* IAUDIOSERVICE_H_ */
