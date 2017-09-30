/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file IChannel.h
* @brief Definite an interface class for the channel action
* 
* @author Limin Hao (limin.hao@gnetis.com)
*
* @date Jan. 25 2010
*
* @version 1.0.0
* 
* @warning 
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug 

******************************************************************************/

#if !defined(EA_D7157FF7_5627_4974_A3CC_349F919F7EFC__INCLUDED_)
#define EA_D7157FF7_5627_4974_A3CC_349F919F7EFC__INCLUDED_

/*----------------------------HEADER FILE INCLUDES---------------------------*/
#include <Message\ExternalMessage.h>

/*----------------------------- ENUMERATIONS ------------------------------------*/
/** @brief the status of the channel */
typedef enum ENUM_ChannelStatus
{
    /************************************************************************/
    /* Channel status                                                       */
    /************************************************************************/
    CHN_STATUS_NO_CONNECTION,/**<ͨ��δ�󶨵��������ӣ�ͨ��������*/
    CHN_STATUS_CONNECTED,/**<ͨ���󶨵��������ӳɹ���ͨ��������*/
    CHN_STATUS_SUCCEED_AUTHED, /**<ͨ����֤�ɹ���ͨ������*/
    CHN_STATUS_FAILED_AUTHED, /**<ͨ����֤ʧ��,ͨ��������*/
    CHN_STATUS_CONNECTION_ERROR,/**<ͨ���󶨵��������Ӷ�ʧ��ͨ��������*/
	CHN_STATUS_INVALID /**<ͨ�����յ�ͨ��ʧЧ����Ϣ��ID=10��ͨ����������*/
}EChannelStatus;

/*----------------------------- ENUMERATIONS ------------------------------------*/
/** @brief the transport type of the channel */
typedef enum ENUM_CHANNEL_TRANSPORT_TYPE
{
    CHANNEL_TYPE_CONTROL = 0x00,   /**< ����ͨ��*/
    CHANNEL_TYPE_UNRELIABLE_TRANSPORT = 0x10, /**<�ɿ������ݴ���ͨ��*/
    CHANNEL_TYPE_RELIABLE_TRANSPORT = 0x11,   /**<���ɿ������ݴ���ͨ��*/
}EChannelTransportType;
/**
* @class IChannelMgr
* @brief The interface for the service plug-in to create channel or destroy channel
* 
* @author Limin.hao
* @version 1.0.0
*/
class IChannel
{

public:
    /**
    * @brief The empty constructure function for the interface.
    */
    IChannel() {};
    /**
    * @brief The virtual deconstructure function for the interface
    */
    virtual ~IChannel() {};

    /**
    * @brief Pop out a message, if there is no message, the action will block until the time out.
    * @param [in] time_out The second to block the pop action.
    * @return A pointer of the Message got from the server. If timed out, got an null pointer.
    * @post If returned null, call the GetStatus to get the channel status.
    * @see GetStatus.
    */
    virtual ExternalMessage* 
        PopMessage(
        UINT32 __in time_out
        ) = 0;

    /**
    * @brief Push an message to the channel.
    * @param [in] msg A pointer of a message which must an external message.
    * @return HRESULT return S_OK if successful, otherwise return error code.
    * @post If failed, call the GetStatus to get the channel status.
    * @see GetStatus.
    */
    virtual HRESULT
        PushMessage(
        ExternalMessage*  __in msg
        ) = 0;

    /**
    * @brief Get the channel status.
    * @return The status of the channel.
    * @see EChannelStatus.
    */
    virtual EChannelStatus 
        GetStatus(
        ) =0;

	/** tian.hu 2014-1-16
	* @brief �����Լ��ķ��ͻ�����г���,���������ͨ��
	* @param  uSize: ���ͻ�����е���󻺳����,����0����ΪĬ��ֵ
	* @return bool: �����Ƿ�ɹ� ����ͨ��һ�ɷ���false .
	*/
	virtual bool SetMaxSendListSize(UINT16 uSize) = 0;

	/** dapeng.li 2014-1-17
	* @brief ��ȡ���Ͷ����д������ĸ�����ΪӦ�ò��ṩ,���ڸ�֪���������,����ѹ����
	* @param  void:
	* @return UINIT16: �ȴ���������,0��ʾ����û�д�������
	*/
	virtual UINT16 GetSendListRemainPacketCount() = 0;

};
#endif // !defined(EA_D7157FF7_5627_4974_A3CC_349F919F7EFC__INCLUDED_)
