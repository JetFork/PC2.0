/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file HeartBeatMessage.h
* @brief ���ڿɿ�ͨ���ط�����Ϣ.
* 
* @author John
*
* @date May. 6 2010
*
* @version 1.0.0
* 
* @warning none.
*
* Revision History
* @if  CR/PR ID    Author         Date        Major Change @endif
* @bug

******************************************************************************/

#pragma  once

#include <Message\CtrlMessage.h>
#include <Message\TransportMessage.h>
#include <Message\TransServerHeartBeatMsg.h>
#include <vector>

using namespace std;


#pragma pack(1)
class HeartBeatCtrlMessageReq : public CtrlMessageImpl<HeartBeatCtrlMessageReq>
{
public:
    HeartBeatCtrlMessageReq()
    {
        this->ctrlHeader.message_type = TANG_PROTOCOL_MESSAGE_TYPE_CHANNEL;
        this->ctrlHeader.message_id = CHANNEL_HEARTBEAT_REQ_MSG_ID;

        uCountRecv = 0;
        uCountLoss = 0;
        uDelayTime = 0;
        uInterarrivalJitter = 0;
    }

    virtual ~HeartBeatCtrlMessageReq()
    {
    }

public:
    UINT16 uCountRecv;          /**<���յ��İ����� */
    UINT16 uCountLoss;          /**<��ʧ�������� */
    UINT16 uDelayTime;          /**<�ӳ�ʱ�� */
    UINT16 uInterarrivalJitter; /**<���� */
};


class HeartBeatCtrlMessageRsp : public CtrlMessageImpl<HeartBeatCtrlMessageRsp>
{
public:
    HeartBeatCtrlMessageRsp()
    {
        this->ctrlHeader.message_type = TANG_PROTOCOL_MESSAGE_TYPE_CHANNEL;
        this->ctrlHeader.message_id = CHANNEL_HEARTBEAT_RSP_MSG_ID;

        uCountRecv = 0;
        uCountLoss = 0;
        uAverageDelayTime = 0;
        uAverageInterarrivalJitter = 0;
        uMaxDelayTime = 0;
        uMaxInterarrivalJitter = 0;
    }

    virtual ~HeartBeatCtrlMessageRsp()
    {
    }

public:
    UINT16 uCountRecv;                  /**<���յ��İ����� */
    UINT16 uCountLoss;                  /**<��ʧ�������� */
    UINT16 uAverageDelayTime;           /**<ƽ���ӳ�ʱ�� */
    UINT16 uAverageInterarrivalJitter;  /**<ƽ������ */
    UINT16 uMaxDelayTime;               /**<����ӳ�ʱ�� */
    UINT16 uMaxInterarrivalJitter;      /**<��󶶶� */
};


struct HeartbeatDataReq
{
    HeartbeatDataReq()
    {
        memset(this, 0, sizeof(*this));
    }

    UINT32 uSsrc;                       /**<SSRC */
    UINT32 uChannelId;                  /**<ChannelId */
    UINT16 uLastSeq;                    /**<LastSeq */
    UINT16 uCountLoss;                  /**<��ʧ�������� */
    UINT16 uDelayTime;                  /**<�ӳ�ʱ�� ms */
    UINT16 uInterarrivalJitter;         /**<���� ms */
};


class HeartBeatTransportMessageReq : public TransportMessageImpl<HeartBeatTransportMessageReq>
{
public:
    HeartBeatTransportMessageReq()
    {
        this->rtpExtendHdr.SetMsgType(TANG_PROTOCOL_MESSAGE_TYPE_CHANNEL);
        this->rtpExtendHdr.SetMsgID(CHANNEL_HEARTBEAT_REQ_MSG_ID);
    }

    virtual ~HeartBeatTransportMessageReq()
    {
    }

    size_t SerializeBody(__in LPBYTE pBuf, __in size_t cbBuf)\
    {
        HRESULT hr = S_OK;

        LPBYTE pThis = (LPBYTE)this + sizeof(TransportMessage);

        //���¼���������Ҫ������Ǽ̳ж�����ͷ�Ĳ���
        size_t sizeStableData = sizeof(*this) 
                                - sizeof(TransportMessage) 
                                - sizeof(listHeartbeatData)
                                -sizeof(m_uUserID)
                                ;
        size_t sizeList = listHeartbeatData.size() * sizeof(HeartbeatDataReq);

        ARG_CHECK(pBuf);
        if ((sizeStableData + sizeList + sizeof(m_uUserID)) <= cbBuf)
        {
            //�����̳в��ֵ��ڴ�
            memcpy(pBuf, pThis, sizeStableData);
            memcpy(pBuf+sizeStableData,&m_uUserID,sizeof(m_uUserID));

            if (listHeartbeatData.size() > 0)
            {
                pBuf += sizeStableData;
                memcpy(pBuf+sizeof(m_uUserID), &(listHeartbeatData.at(0)), sizeList);
            }
        }
        else
        {
            LOG_ERR(L"memeory not enught ,buffer len %u .",cbBuf);
            sizeList = 0;
        }

    exit:
        return (sizeStableData + sizeList+sizeof(m_uUserID));
    }

    size_t DeSerializeBody(__in LPBYTE pBuf, __in size_t cbBuf)\
    {
        HRESULT hr = S_OK;
        //pThis �����ָ�����ಿ�ֵ��ڴ�
        LPBYTE pThis = (LPBYTE)this + sizeof(TransportMessage);

        size_t sizeStableData = sizeof(*this) 
                                    - sizeof(TransportMessage) 
                                    - sizeof(listHeartbeatData)
                                    -sizeof(m_uUserID)
                                    ;
        size_t sizeList = 0;

        ARG_CHECK(pBuf);
        if (sizeStableData <= cbBuf)
        {
            //���̳ж������ֵ�����
            memcpy(pThis, pBuf, sizeStableData);

            UINT32 uListCount = (cbBuf - sizeStableData-sizeof(m_uUserID))/sizeof(HeartbeatDataReq);
            sizeList = uListCount * sizeof(HeartbeatDataReq);
            if ((sizeStableData + sizeList+sizeof(m_uUserID)) <= cbBuf)
            {
                //����ļӷ��ǰ�ָ��ƫ�Ƶ�vector����ȥ
                    HeartbeatDataReq* pTangPoint = (HeartbeatDataReq*)(pBuf + sizeStableData+sizeof(m_uUserID));
                    for (UINT32 i = 0; i < uListCount; i++)
                    {
                        listHeartbeatData.push_back(pTangPoint[i]);
                    }
            }
            else
            {
                sizeList = 0;
                LOG_ERR(L"memeory not enught ,buffer len %u .",cbBuf);
            }
        }
    exit:
        return (sizeStableData + sizeList+sizeof(m_uUserID));
    }

    UINT32 GetBodyLength(void)
    {
        size_t sizeStableData = sizeof(*this) - sizeof(TransportMessage) - sizeof(listHeartbeatData)-sizeof(m_uUserID);
        size_t sizeList = listHeartbeatData.size() * sizeof(HeartbeatDataReq);
        return sizeStableData + sizeList+sizeof(m_uUserID);
    }

public:
    UINT32 m_uUserID;
    vector<HeartbeatDataReq> listHeartbeatData; /**<������Ϣ��������� */
};


struct HeartbeatDataRsp
{
    HeartbeatDataRsp()
    {
        memset(this, 0, sizeof(*this));
    }

    UINT32 uGroupId;
    UINT16 uRecvLastSeq; // ƽ�������������
    UINT16 uAverageLoss;
    UINT16 uAverageDelayTime;
    UINT16 uAverageInterarrivalJitter;
    UINT16 uMaxDelayTime;
    UINT16 uMaxInterarrivalJitter;
};


class HeartBeatDataMessageRsp : public TransportMessageImpl<HeartBeatDataMessageRsp>
{
public:
    HeartBeatDataMessageRsp()
    {
        this->rtpExtendHdr.SetMsgType(TANG_PROTOCOL_MESSAGE_TYPE_CHANNEL);
        this->rtpExtendHdr.SetMsgID(CHANNEL_HEARTBEAT_RSP_MSG_ID);
    }

    virtual ~HeartBeatDataMessageRsp()
    {
    }

    size_t SerializeBody(__in LPBYTE pBuf, __in size_t cbBuf)\
    {
        HRESULT hr = S_OK;

        LPBYTE pThis = (LPBYTE)this + sizeof(TransportMessage);

        //���¼���������Ҫ������Ǽ̳ж�����ͷ�Ĳ���
        size_t sizeStableData = sizeof(*this) 
            - sizeof(TransportMessage) 
            - sizeof(listHeartbeatData)
            -sizeof(m_uUserID)
            ;
        size_t sizeList = listHeartbeatData.size() * sizeof(HeartbeatDataRsp);

        ARG_CHECK(pBuf);
        if ((sizeStableData + sizeList + sizeof(m_uUserID)) <= cbBuf)
        {
            //�����̳в��ֵ��ڴ�
            memcpy(pBuf, pThis, sizeStableData);
            memcpy(pBuf+sizeStableData,&m_uUserID,sizeof(m_uUserID));

            if (listHeartbeatData.size() > 0)
            {
                pBuf += sizeStableData;
                memcpy(pBuf, &(listHeartbeatData.at(0)), sizeList);
            }
        }
        else
        {
            sizeList = 0;
            LOG_ERR(L"memeory not enught ,buffer len %u .",cbBuf);
        }
exit:
        return (sizeStableData + sizeList+sizeof(m_uUserID));
    }

    size_t DeSerializeBody(__in LPBYTE pBuf, __in size_t cbBuf)\
    {
        HRESULT hr = S_OK;
        //pThis �����ָ�����ಿ�ֵ��ڴ�
        LPBYTE pThis = (LPBYTE)this + sizeof(TransportMessage);

        size_t sizeStableData = sizeof(*this) 
            - sizeof(TransportMessage) 
            - sizeof(listHeartbeatData)
            -sizeof(m_uUserID)
            ;
        size_t sizeList = 0;

        ARG_CHECK(pBuf);
        if (sizeStableData <= cbBuf)
        {
            //���̳ж������ֵ�����
            memcpy(pThis, pBuf, sizeStableData);
            memcpy(&m_uUserID,pBuf+sizeStableData,sizeof(m_uUserID));

            UINT32 uListCount = (cbBuf - sizeStableData-sizeof(m_uUserID))/sizeof(HeartbeatDataRsp);
            sizeList = uListCount * sizeof(HeartbeatDataRsp);
            if ((sizeStableData + sizeList+sizeof(m_uUserID)) <= cbBuf)
            {
                //����ļӷ��ǰ�ָ��ƫ�Ƶ�vector����ȥ
                HeartbeatDataRsp* pTangPoint = (HeartbeatDataRsp*)(pBuf + sizeStableData+sizeof(m_uUserID));
                for (UINT32 i = 0; i < uListCount; i++)
                {
                    listHeartbeatData.push_back(pTangPoint[i]);
                }
            }
            else
            {
                sizeList = 0;
                LOG_ERR(L"memeory not enught ,buffer len %u .",cbBuf);
            }
        }
exit:
        return (sizeStableData + sizeList+sizeof(m_uUserID));
    }

    UINT32 GetBodyLength(void)
    {
        size_t sizeStableData = sizeof(*this) 
            - sizeof(TransportMessage) 
            - sizeof(listHeartbeatData)
            -sizeof(m_uUserID)
            ;

        size_t sizeList = listHeartbeatData.size() * sizeof(HeartbeatDataRsp);
        return sizeStableData + sizeList+sizeof(m_uUserID);
    }

public:
    UINT32 m_uUserID;
    vector<HeartbeatDataRsp> listHeartbeatData;
};
#pragma pack()