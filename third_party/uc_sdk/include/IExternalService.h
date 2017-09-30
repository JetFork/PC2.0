/*
 * IExternalService.h
 *
 *  Created on: Jan 27, 2016
 *      Author: yongshuai.cheng
 */

#ifndef IEXTERNALSERVICE_H_
#define IEXTERNALSERVICE_H_

#include "UCSDK.h"
#include "IUCEngine.h"
#include "ExternalInfo.h"

namespace uc {

	interface UC_API IExternalServiceEvent {
		virtual ~IExternalServiceEvent() {};

		/**
		* ������ʷ�б�;
		* @param result  	     ������Ϣ;
		* @param type			 �Ự����,����/������;
		* @param relateId	     �û�ID/������ID;
		* @param msgCount		 message count;
		* @param min_timestamp   min timestamp;
		*/
		virtual void OnChatHistoryReceived(int32_t result, ConversationType type, int64_t relateId, int32_t msgCount, int64_t min_timestamp, const list<ExternalMessage*>& msgList) = 0;
	};

	interface UC_API IExternalService : public IUCService, Event<IExternalServiceEvent>, IUCEngineEvent {
		virtual ~IExternalService() {};
		/**
		* ��ȡ��ʷ�������ݽӿ�;
		* @param type               ��������;
		* @param externalId         ����Ŀ��ID(P2P:�û��˺�/������:�ⲿID);
		* @param count              ������������������Ĭ��20�����ܳ���200;
		* @param startTime          �����������ݵĿ�ʼʱ��;
		* @param endTime            �����������ݵĽ���ʱ��;
		* @return					������;
		*/
		virtual int32_t GetChatHistory(ConversationType type, const string& externalId, int32_t count, int64_t startTime, int64_t endTime) = 0;

		/**
		* �Ƿ���ʾ�ⲿ��Ϣ;
		* @param externalId		����Ŀ��ID(P2P:�û��˺�/������:�ⲿID);
		* @return					 0������ʾ/1����ʾ;
		*/
		virtual int32_t ShowExternalMessage(const string& externalId) = 0;

	};
}

#endif /* IEXTERNALSERVICE_H_ */
