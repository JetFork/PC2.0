#ifndef IMICROBLOGSERVICE_H_
#define IMICROBLOGSERVICE_H_

#include "UCSDK.h"
#include "IUCEngine.h"

namespace uc {

	interface UC_API IMicroblogServiceEvent {
		virtual ~IMicroblogServiceEvent() {};

		virtual void OnNewMsgCountGet(int32_t errorCode, int32_t countAll, int32_t relationCount, int32_t timenewsCount, int32_t pollCount, int32_t caseCount) = 0;
		/**
		* ��ӹ�ע�Ļص��¼�
		* @param errorCode ������
		* @param userId ��ע�û���id
		* @param relation ��ϵ 1:��ע, 2:��˿, 3:�໥��ע, 4:û�й���
		*/
		virtual void OnAttentionAdd(int32_t errorCode, int32_t userId, int32_t relation) = 0;

		/**
		* ȡ����ע�Ļص��¼�
		* @param errorCode ������
		* @param userId ȡ����ע�û���id
		* @param relation ��ϵ 1:��ע, 2:��˿, 3:�໥��ע, 4:û�й���
		*/
		virtual void OnAttentionCancelled(int32_t errorCode, int32_t userId, int32_t relation) = 0;

		/**
		* ����¼�Ʒ���ص��¼�
		* @param errorCode ������
		* @param resourceID ����ID
		*/
		virtual void OnMeetingRecordingShared(int32_t errorCode, int64_t resourceID) = 0;
	};

	interface UC_API IMicroblogService : public IUCService, Event<IMicroblogServiceEvent> {
		virtual ~IMicroblogService() {};

		/**
		 * ��ȡ�Լ��Ĺ���Ȧ��ҳ
		 */
		virtual std::string GetHomePageURL(int32_t userId) = 0;

		/**
		* ��ȡ���˵Ĺ���Ȧ��ҳ
		* @param SelfId �Լ���Id
		* @param userId Ŀ���Id
		*/
		virtual std::string GetUserHomePageURL(int32_t selfId, int32_t userId) = 0;

		/**
		 * ��ȡδ����Ϣ������URL
		 */
		virtual void GetNewMsgCount() = 0;

		/**
		 * ��ӹ�ע
		 * @param userId ����ע�û�id
		 */
		virtual void AddAttention(int32_t userId) = 0;

		/**
		 * ȡ����ע
		 * @param userId ��ȡ����ע���û�id
		 */
		virtual void CancelAttention(int32_t userId) = 0;

		/**
		 * ��ȡ��ע״̬
		 * @param userId �Է��û�id
		 * @return ����ֵ 1:��ע, 2:��˿, 3:�໥��ע, 4:û�й���
		 */
		virtual int32_t GetRelation(int32_t userId) = 0;

		/**
		 * �������recording
		 * @param url recording��ַ
		 * @param comments ע��
		 */

		virtual void ShareMeetingRecording(std::string urlVideo,std::string urlThumb, std::string fileName,int64_t fileSize,int64_t voiceTime, std::string comments) = 0;
	};
}


#endif /* IMICROBLOGSERVICE_H_*/