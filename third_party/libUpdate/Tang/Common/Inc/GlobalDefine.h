/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd.

******************************************************************************/
/**
* @file GlobalDefine.h
* @brief Defined all the global variable for Tang client.
*
* @author Hongwei Hu (hongwei.hu@gnetis.com)
*
* @date Janu. 25 2010
*
* @version 1.0.0
*
* @warning
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug

******************************************************************************/

#ifndef GNET_TANG_CLIENT_GLOBALDEFINE_H
#define GNET_TANG_CLIENT_GLOBALDEFINE_H

/*----------------------------HEADER FILE INCLUDES---------------------------*/
//#include <IConference.h>
#include <IMessageProvider.h>
#include <IChannelManagement.h>
#include <IConfiguration.h>

/**
* @struct GlobalPointer
* @brief Global Pointer struct includes the instance of all the interfaces
* which business will be used.
*/
struct GlobalPointer
{
   // IConference* pConference;               /**< pointer t the conference instance*/
    IChannelManagement* pChannelMgr;        /**< pointer to the channel manager instance*/
    IMessageProvider* pMsgDispatcher;       /**< pointer to the message dispatcher*/
    IConfiguration* pConfiguration;         /**< pointer to the client configuration instance*/
    IDispatch* pIEInstance;                 /**< pointer to the IE engine instance*/

    GlobalPointer(void)
    {
        //pConference         =    NULL;
        pChannelMgr         =    NULL;
        pMsgDispatcher      =    NULL;
        pConfiguration      =    NULL;
        pIEInstance         =    NULL;
    }

    ~GlobalPointer(void)
    {

    }
};
typedef GlobalPointer* GlobalPtr;

/**
* @enum TangServiceType
* @brief Tang service type definition.
* which business will be used.
*/
enum TangServiceType
{
    NetServiceType = 0,         /**< Web */
    ConferenceServiceType = 1,  /**< Conference */
	MaxServiceType = 10,		/**< max�����ڴ�ʹ�� */
    AudioServiceType = 258,     /**< Audio */
    VideoServiceType = 259,     /**< Video */
    WhiteboardServiceType = 775,/**< White board */
    DocumentServiceType = 776,  /**< �ĵ����� */
    DesktopServiceType = 777,   /**< ���湲�� */
    ChatServiceType = 516,   /**< ���� */
    QaServiceType = 517,   /**< �ʴ� */
    SurveyServiceType = 518,   /**< ���� */
	UcServiceType = 800,   /**< UC */
    FileTransportServiceType = 801, /**< UC-�ļ����� */

    AllServiceType = -1         /**< ȫ�� */
};

/** 
* @enum TANG_SERVICE_STATUS
* @brief The status of service.
*/
typedef enum
{
    SERVICE_STATUS_IDLE = 0,        /**< Service is not start. */
    SERVICE_STATUS_OPEN_PENDING,	/**< Service is starting. */
    SERVICE_STATUS_OPEN_FAILED,     /**< Service start failed. */
    SERVICE_STATUS_OPEN,			/**< Service started successfully and ready to use. */
    SERVICE_STATUS_ERROR,			/**< Service can not work yet. */
    SERVICE_STATUS_CLOSED			/**< Service is already closed. */
} TANG_SERVICE_STATUS;/**< The status of service. */

/**
* @enum TangChannelType
* @brief Tang channel type definition.
* which business will be used.
*/
enum TangChannelType
{
    TANG_CHANNEL_TYPE_CONTROL    = 0,
    TANG_CHANNEL_TYPE_DISTRIBUTE = 1
};

/**
* @define adapter name macros
* @brief This is adapter name string.
* @{ */
#define ADAPTOR_NAME_MAINWND                   L"MainWnd"				   /**< name string of main frame adapter */
#define ADAPTOR_NAME_UTILITY                   L"Util"					   /**< name string of Utility adapter */
//daming: 2011-08-10 ������Session�ĳ�sessions
#define ADAPTOR_NAME_SESSION                   L"sessions"				   /**< name string of session adapter */
#define ADAPTOR_NAME_CONFMGR                   L"ConferenceAdaptor"        /**< name string of conference adapter */
#define ADAPTOR_NAME_DOCSHARING                L"DocSharingAdaptor"        /**< name string of document sharing adapter */
#define ADAPTOR_NAME_DESKTOPSHARING            L"DesktopSharingAdaptor"    /**< name string of desktop sharing adapter */
#define ADAPTOR_NAME_WHITEBOARD                L"WhiteBoardAdaptor"        /**< name string of white board adapter */
#define ADAPTOR_NAME_AUDIO					   L"AudioAdaptor"			   /**< name string of audio service adapter */
#define ADAPTOR_NAME_VIDEO                     L"VideoAdaptor"             /**< name string of video service adapter */
#define ADAPTOR_NAME_PHONE                     L"PhoneAdaptor"             /**< name string of phone module adapter */
#define ADAPTOR_NAME_CHAT                      L"ChatAdaptor"              /**< name string of chat module adapter */
#define ADAPTOR_NAME_QA                        L"QaAdaptor"                /**< name string of qa module adapter */
#define ADAPTOR_NAME_SURVEY                    L"SurveyAdaptor"            /**< name string of survey module adapter */
#define ADAPTOR_NAME_UPDATE                    L"UpdateAdaptor"            /**< name string of update module adapter */

#define ADAPTOR_NAME_FILETRANSPORT             L"FileTranspost"            /**< name string of filetransport module adapter */
#define ADAPTOR_NAME_AJAXWRAPPER               L"AjaxWrapper"              /**< name string of AjaxWrapper module adapter */

#define DISPATCH_NAME_PHONELIST            	   L"PhoneList"                /**< name string of phone user list dispatch object */

/**<define service dispatch object name for UI rebuild on 2011-5-18 */
#define  DISPATCH_NAME_CONFMGR                    L"conference"
#define  DISPATCH_NAME_DOC_SHARE                  L"docShare"
#define  DISPATCH_NAME_DESKTOP_SHARE              L"desktopShare"
#define  DISPATCH_NAME_WHITEBOARD                 L"whiteBoard"
#define  DISPATCH_NAME_AUDIO                      L"audio"
#define  DISPATCH_NAME_VIDEO                      L"video"
#define  DISPATCH_NAME_POHONE                     L"phone"
#define  DISPATCH_NAME_CHAT                       L"chat"
#define  DISPATCH_NAME_QA                         L"QA"
#define  DISPATCH_NAME_SURVEY                     L"survey"
#define  DISPATCH_NAME_USERLIST                   L"userList"               /**< name string of user list dispatch object */
#define  DISPATCH_NAME_PHONE_LIST		          L"phoneList"
#define	 DISPATCH_NAME_EVENTCENTER                L"eventCenter"			/**< name string of event center */
#define	 DISPATCH_NAME_UC_MAIN				      L"uc"				        /**< name string of UC */
#define	 DISPATCH_NAME_UC_CHAT				      L"uc"				        /**< name string of UC */
#define	 DISPATCH_NAME_UCCLIENT_SESSIONLIST       L"ucclientsessionlist"    /**< session list of ucclient */
#define  DISPATCH_NAME_AUDIOWIZARD                L"audioWizard"
#define  DISPATCH_NAME_VIDEOWIZARD                L"videoWizard"
#define  DISPATCH_NAME_CATCHSCREEN                L"catchScreen"
#define  DISPATCH_NAME_CALL						  L"call"
#define	 DISPATCH_NAME_PRE_SESSIONLIST            L"ucPreSessionlist"    /**< Pre session list of ucclient */
/**
* @define module name macros for native dispatch implementation
* @brief This is adapter name string.
* @{ */
#define MODULE_NAME_CLIENT_HOST                L"TangClient"				/**< module name string for client host */
#define MODULE_NAME_WHITEBOARD				   L"plugin\\Whiteboard"		/**< module name string for whiteboard */
#define MODULE_NAME_AVMODULE	               L"plugin\\AVModule"			/**< module name string for audio and video */
#define MODULE_NAME_DOC_SHARE	               L"plugin\\DocSharing"		/**< module name string for doc share */
#define MODULE_NAME_TEXT		               L"plugin\\Text"				/**< module name string for chat qa and suery */
#define MODULE_NAME_DESKTOP_SHARE		       L"plugin\\DesktopSharing"	/**< module name string for desktop share */
#define MODULE_NAME_UPDATE				       L"Update"	                /**< module name string for desktop share */
#define	MODULE_NAME_UC						   L"uc"				        /**< module name string forUC */
#define	MODULE_NAME_CONFERENCE				   L"plugin\\Conference"		/**< module name string for conference */
#define MODULE_NAME_AVWIZARD	               L"plugin\\AVWizard"			/**< module name string for audio and video Wizard */
#define MODULE_NAME_CATCHSCREEN	               L"plugin\\CatchScreen"			/**< module name string for audio and video Wizard */
#define MODULE_NAME_CALL					   L"plugin\\Call"				/**< module name string for audio and video Wizard */
#define MODULE_NAME_FILETRANSPORT              L"plugin\\FileTransport"     /**< module name string for uc filetransport */

enum TangStatus
{
	TANG_STATUS_CHANNEL_AUTH_SUCCESSED = 200,
	TANG_STATUS_CHANNEL_AUTH_FAILED = 400,
};

/**
* @enum UCChatLoadType
* @brief UCChat�������������.
*/
enum UCChatLoadType
{
    UCChatLoadType_Chat = 1,      /**< �������� */
    UCChatLoadType_Meeting = 2,   /**< �������� */
    UCChatLoadType_Phone = 3,     /**< �绰���� */
};

/** 
* @enum SessionIDType
* @brief �ỰID���Ͷ���
*/
enum SessionIDType
{
    SessionIDType_Invalid       = 0,
    SessionIDType_UserID        = 1,    /**< �û�ID����*/
    SessionIDType_SeminarID     = 2,    /**< ������ID����*/
    SessionIDType_GroupID       = 3,    /**< ȺID����*/
    SessionIDType_ConferenceID  = 4,    /**< ����ID����*/
};

/**
 * UC�ڲ���Ϣ(����MQ��Ϣ��JS��Ϣ����������Ϣ)Ͷ�ݵ���Ϣ���ߵ�
 * Channel��Group����
 * ��Ϣ�������֮�����Ǹ�����ģ��ʹ�õģ�����ģ���Channel��
 * Group�ɷ��������ɵģ���MQ��Ϣ�ͻ����޹�������Ҫ���ض���Channel
 * ��Group��Ϊ�˱�֤�ͻ���ģ�鲻���ͻ��Ŀǰֻ�ܶ����magic number��
 * �Ժ��ع���Ϣ����ʱ�����޸�
 */
#define UC_INTERNAL_MESSAGE_CONFERENCE_ID   ((UINT32)12345)     /**< ����ID */
#define UC_INTERNAL_MESSAGE_GROUP_ID        ((UINT32)54321)     /**< ��ID */

/**
* @brief filter the hresult value to common error code to application layer.
* @param [in] void.
* @return HRESULT return common error code if the hresult is defined in tangerrorcode.h,
*  otherwise return the original value.
*/
inline HRESULT HResult_To_App_Layer(HRESULT hr)
{
	HRESULT hrRes = 0;
	if ( SUCCEEDED(hr) )
	{
		hrRes = 200;
	}
	else
	{
		// ȥ�����λ����Ϊjs����ֵ�������޷��ŵģ�����js�ᱨ��
		hrRes = hr&(0x0FFFFFFF);
	}
	return hrRes;
};

#endif  //GNET_TANG_CLIENT_GLOBALDEFINE_H