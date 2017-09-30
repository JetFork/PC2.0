/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file IChannelManagement.h
* @brief Definite an interface class for the channel management
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

#if !defined(EA_1628FC35_9739_4a2d_B6AB_34EFAC517908__INCLUDED_)
#define EA_1628FC35_9739_4a2d_B6AB_34EFAC517908__INCLUDED_

/*----------------------------HEADER FILE INCLUDES---------------------------*/
#include <windows.h>
#include <IChannel.h>


/** @brief the transport type of the channel.The internal type should not be set outside. */
/** The value should be set between the lowest to the highest.*/
typedef enum E_CHANNEL_PRIORITY_TYPE
{
    TRANPSORT_PRIORITY_HEARTBEAT = 0,       /**<Internal type: ��ͼ���ģ�Ϊ���������ļ���*/
    TRANSPORT_PRIORITY_LOW = 14,            /**<�ͼ����ʺϴ������ݴ��䣬�����ĵ��ϴ���*/
    TRANSPORT_PRIORITY_NORMAL = 16,         /**<��ͨ�����ȼ�*/
    TRANSPORT_PRIORITY_REALTIME = 18,       /**<�ʺ�ʵʱ�����ݴ���*/
    TRANSPORT_PRIORITY_HIGH = 20,           /**<�ϸߵ����ȼ����ʺϿ���ͨ��*/
    TRANPSORT_PRIORITY_AUTH = 31            /**<Internal type: ��߼���ģ�Ϊ������֤��Ϣ�ļ���*/
}EChannelPriorityType;


/**
* @class IChannelManagement
* @brief The interface for the service plug-in to create channel or destroy channel
* 
* @author Limin.hao
* @version 1.0.0
*/
class IChannelManagement
{
public:
    /**
    * @brief Create a channel by the seven field at least.
    * @param [in] channel_id: The unique channel id for the channel.
    * @param [in] group_id: The unique group id for channel.
    * @param [in] conf_id: The unique id for the conference
    * @param [in] service_type: The service type for the owner of the channel.    
    * @param [in] transport_type: The transport type for the channel: control, reliable transport, unreliable channel.
    * @param [in] priority: The base priority channel for the channel.
    * @param [in] self_manage: The flag to manage the read and write to the channel by self.
    * @param [in] chn: The interface for the channel read and write.
    * @return HRESULT return S_OK if successful, otherwise return error code.
    * @pre To managed the channel yourself, the self_manage flag must be set and input a pointer point to the pointer of IChannel.
    * @see ChannelMgr.
    */
    virtual HRESULT CreateChannel(
        UINT32 __in channel_id,
        UINT32 __in group_id,
        UINT32 __in conf_id, 
        UINT32 __in service_type,
        EChannelTransportType __in transport_type,
        EChannelPriorityType __in priority,
        bool __in self_manage,
        IChannel** __inout chn
        ) = 0;

    /**
    * @brief Destroy a channel by its id, or its managed interface
    * @param [in] channel_id: The unique channel id.
    * @return HRESULT return S_OK if successful, otherwise return error code.
    * @post The pointer of the channel or the channel can't used again.
    * @see CreateChannel.
    */
    virtual HRESULT DestroyChannel(
        IChannel* __in chn
        ) = 0;

protected:
    /**
    * @brief The empty constructor function for the interface.
    */
    IChannelManagement() {};

    /**
    * @brief The virtual destructor function for the interface
    */
    virtual ~IChannelManagement() {};
};
#endif // !defined(EA_1628FC35_9739_4a2d_B6AB_34EFAC517908__INCLUDED_)
