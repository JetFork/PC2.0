/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file InternalMessage.h
* @brief Definites a base class for all the internal messages in Tang Client.
* 
* @author Xiaogang Yu (xiaogang.yu@gnetis.com)
*
* @date Feb. 4 2010
*
* @version 1.0.0
* 
* @warning null
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug 

******************************************************************************/

#ifndef GNET_TANG_CLIENT_INTERNALMESSAGE_H
#define GNET_TANG_CLIENT_INTERNALMESSAGE_H

/*----------------------------HEADER FILE INCLUDES---------------------------*/
#include <Message.h>
#include <conferencedef.h>

/**<Channel managerment��0x0000-0x00FF*/
#define INTERNEL_CHANNEL_AUTH_RESULT                (0x0000)    /**<Channel authentication result. */
#define INTERNEL_CHANNEL_EVENT                      (0x0001)    /**<Channel event: connected, disconnected. */
#define INTERNEL_CHANNEL_STATISTICS                 (0x0002)    /**<Channel networking statistics. */
#define INTERNEL_VIDEO_CHANNEL_STATISTICS           (0x0008)    /**<Channel VEDIO networking statistics. */
#define INTERNEL_AUDIO_CHANNEL_STATISTICS           (0x0009)    /**<Channel AUDIO networking statistics. */
#define INTERNEL_CHANNEL_TIMEOUTEVENT               (0x0003)    /**<Channel CtrlMessage Timeout Notify */
#define INTERNEL_CHANNEL_BWADJUSTMENTEVENT          (0x0004)    /**<Channel bandwidth adjust Notify */
#define INTERNEL_CHANNEL_RECVINFOEVENT              (0x0005)    /**<Channel Receive msg info Notify */
#define INTERNEL_CHANNEL_QUCKLOSTRATENOTIFY         (0x0007)    /**<Channel quick lost rate Notify */

/**<��������ڲ���ϢID��0x0100-0x01FF*/
#define INTERNAL_MESSAGE_ID_CONFERENCE_SYSTEM_LOCK  (0x0100)    /**<����ϵͳ���ڲ���ϢID*/
#define INTERNAL_MESSAGE_ID_USER_EXIT_CONFERENCE    (0x0101)    /**<�û��˻��ڲ���ϢID*/
#define INTERNAL_MESSAGE_ID_USER_STATUS_CHANGED     (0x0102)    /**<�û�״̬����ڲ���ϢID*/
#define INTERNAL_MESSAGE_ID_USER_ROLE_CHANGED       (0x0103)    /**<�û���ɫ����ڲ���ϢID*/
#define INTERNAL_MESSAGE_ID_DTS_CHANNEL_INFO		(0x0104)    /**<DTSͨ����Ϣ�ڲ���Ϣ*/
#define INTERNAL_MESSAGE_ID_USER_TDC_READY			(0x0105)    /**<DTSͨ����Ϣ�ڲ���Ϣ*/

#define INTERNAL_MESSAGE_ID_DOWNLOAD_PLUGIN_FAILED	(0x0106)    /**<���ز��ʧ���ڲ���Ϣ*/
#define INTERNAL_MESSAGE_ID_SETTING_CHANGE	        (0x0107)    /**<�������øı��ڲ���Ϣ*/
#define INTERNAL_MESSAGE_ID_SCREENLOCK_CHANGE	    (0x0108)    /**<����״̬�ı��ڲ���Ϣ*/

#define INTERNAL_MESSAGE_ID_UPLOADLOGSTATUS         (0x0151)	/**<��־�ϴ�״̬*/
#define INTERNAL_MESSAGE_ID_UPLOADLOGPROGRESS       (0x0152)	/**<��־�ϴ������ڲ���Ϣ*/
#define INTERNAL_MESSAGE_ID_LOGFILEURL				(0x0153)	/**<��־�ϴ��ɹ����ڷ������ϵ�url*/


#ifdef _DEBUG
#define INTERNAL_MESSAGE_ID_SEND_MESSAGE_REPORT     (0x0014)
#endif




/**<�װ��ڲ���ϢID��0x0200-0x02FF*/
#define INTERNAL_MESSAGE_ID_STARTCOMMENT                    (0x0201)    // ������ע
#define INTERNAL_MESSAGE_ID_STARTCOMMENT_NOTIFY             (0x0202)    // �ɹ�������ע
#define INTERNAL_MESSAGE_ID_STARTCOMMENT_ERR_NOTIFY         (0x0203)    // ������עʧ��

#define INTERNAL_MESSAGE_ID_STOPCOMMENT                     (0x0205)    // �ر���ע
#define INTERNAL_MESSAGE_ID_STOPCOMMENT_NOTIFY              (0x0206)    // �ɹ��ر���ע
#define INTERNAL_MESSAGE_ID_STOPCOMMENT_ERR_NOTIFY          (0x0207)    // �ر���עʧ��

#define INTERNAL_MESSAGE_ID_TURNOVERCOMMENT                 (0x0209)    // ��ҳ��ע
#define INTERNAL_MESSAGE_ID_TURNOVERCOMMENT_NOTIFY          (0x0210)    // �ɹ���ҳ��ע
#define INTERNAL_MESSAGE_ID_TURNOVERCOMMENT_ERR_NOTIFY      (0x0211)    // ��ҳ��עʧ��

#define INTERNAL_MESSAGE_ID_SAVECOMMENT                     (0x0213)    // ������ע
#define INTERNAL_MESSAGE_ID_SAVECOMMENT_NOTIFY              (0x0214)    // ������ע�ɹ�
#define INTERNAL_MESSAGE_ID_SAVECOMMENT_ERR_NOTIFY          (0x0215)    // ������עʧ��

#define INTERNAL_MESSAGE_ID_PRINTCOMMENT                    (0x0217)    // ��ӡ��ע
#define INTERNAL_MESSAGE_ID_PRINTCOMMENT_NOTIFY             (0x0218)    // ��ӡ��ע�ɹ�
#define INTERNAL_MESSAGE_ID_PRINTCOMMENT_ERR_NOTIFY         (0x0219)    // ��ӡ��עʧ��

#define INTERNAL_MESSAGE_ID_SHOWCOMMENT                     (0x0221)    // ��ʾ��ע
#define INTERNAL_MESSAGE_ID_SHOWCOMMENT_NOTIFY              (0x0222)    // ��ʾ��ע�ɹ�
#define INTERNAL_MESSAGE_ID_SHOWCOMMENT_ERR_NOTIFY          (0x0223)    // ��ʾ��עʧ��

#define INTERNAL_MESSAGE_ID_CHECKCOMMENTFIGURE              (0x0225)    // ��ѯ��ע�Ƿ���ͼ��
#define INTERNAL_MESSAGE_ID_CHECKCOMMENTFIGURE_NOTIFY       (0x0226)    // ��ѯ��ע�Ƿ���ͼ�γɹ�
#define INTERNAL_MESSAGE_ID_CHECKCOMMENTFIGURE_ERR_NOTIFY   (0x0227)    // ��ѯ��ע�Ƿ���ͼ��ʧ��

#define INTERNAL_MESSAGE_ID_CHECKCOMMENTEXIST               (0x0229)    // ��ѯ��ע�Ƿ����

// �ĵ������ڲ�����ID��0x0300-0x03FF
#define INTERNAL_MESSAGE_ID_FSS_ONPAGE              (0x0300)    // onpage
#define INTERNAL_MESSAGE_ID_FSS_ONPAGECOUNT         (0x0301)    //OnPageCount
#define INTERNAL_MESSAGE_ID_FSS_ONCONVERTSTATUS     (0x0302)    //OnConvertStatus
#define INTERNAL_MESSAGE_ID_FSS_ONUPLOADPROCESS            (0x0303)    //�ϴ��ĵ�����


/**<���������ڲ���ϢID��0x0400-0x04FF*/
#define INTERNAL_MESSAGE_CHECK_UPDATE_PROGRESS		(0x0400)	// ����°汾����
#define INTERNAL_MESSAGE_DOWNLOAD_PROGRESS			(0x0401)	// ���ؽ���
#define INTERNAL_MESSAGE_INSTALL_PROGRESS			(0x0402)	// ��װ����
#define INTERNAL_MESSAGE_UPDATE_ERROR				(0x0403)	// ������Ϣ
#define INTERNAL_MESSAGE_NEED_UPDATE				(0x0404)	// �Ƿ���Ҫ����

/**<���湲����ڲ���Ϣ��0x500-0x5FF*/
#define INTERNAL_MESSAGE_SHARE_MOUSE_POS            (0x0501) // ����˵����λ��
#define INTERNAL_MESSAGE_SHARE_RFB_HEADER            (0x0502) // ����˵����λ��
#define INTERNAL_MESSAGE_SHARE_DISPLAY_VIEW			(0x0503)	//�������ʾ���ڵ���Ϣ
#define INTERNAL_MESSAGE_RESET_VIEW_SIZE			(0x0504)	//�ֱ��ʸ��ĺ�������ʾ�����С
#define INTERNAL_MESSAGE_REMOVE_INSTANCE			(0x0505)	//�����ع��ڵĹ���ʵ��
#define INTERNAL_MESSAGE_VNC_CONN_TIMEOUT			(0x0506)	//�ȴ�VNC���ӽ�����ʱ��֪ͨ��Ϣ

/**<����Ƶģ����ڲ���Ϣ��0x600-0x6FF*/
#define INTERNAL_MESSAGE_AUDIOENGINE_NOTIFY            (0x0601) // ��Ƶ֪ͨ�¼�
#define INTERNAL_MESSAGE_VIDEOENGINE_NOTIFY            (0x0602) // ��Ƶ֪ͨ�¼�
#define INTERNAL_MESSAGE_TRANSBAD_NOTIFY			   (0x0603) //���������ڲ���Ϣ
#define INTERNAL_MESSAGE_VIDEO_RTCP					   (0x0604) //RTCP�����ڲ���Ϣ
#define INTERNAL_MESSAGE_VIDEO_RENDER				   (0x0605) //render window�ڲ���Ϣ
#define INTERNAL_MESSAGE_VIDEO_DELAYRENDER			   (0x0606) //��ʱrender window�ڲ���Ϣ
#define INTERNAL_MESSAGE_AUDIO_ACTIVESPEAKING_NOTIFY   (0x0607) //�Ƿ�����˵��֪ͨ�ڲ���Ϣ
#define INTERNAL_MESSAGE_AUDIO_MICSPEAKERLEVEL_NOTIFY  (0x0608) //�����仯֪ͨ�ڲ���Ϣ
#define INTERNAL_MESSAGE_AUDIO_MICLEVEL_NOTIFY		   (0x0609) //�����仯֪ͨ�ڲ���Ϣ
#define INTERNAL_MESSAGE_AUDIO_SPEAKERLEVEL_NOTIFY	   (0x060A) //�����仯֪ͨ�ڲ���Ϣ

/**<��Ϣ���з����ڲ���Ϣ��0x700-0x7FF*/
// #define INTERNAL_MESSAGE_MQ_LOGIN_RESULT				(0x0701) //��¼MQ���
#define INTERNAL_MESSAGE_MQ_SENDMSG_FAIL				(0x0702) //����MQ��Ϣʧ��
// #define INTERNAL_MESSAGE_MQ_DISCONNECT					(0x0703) //MQ����
#define INTERNAL_MESSAGE_MQ_NEWMSG_NOTIFY				(0x0704) //MQ����Ϣ֪ͨ
#define INTERNAL_MESSAGE_JS_NEWMSG_NOTIFY				(0x0705) //JS����Ϣ֪ͨ
#define INTERNAL_MESSAGE_UPDATE_SESSIONDATA				(0x0706) //���»Ự����

/**< �Ự�����ڲ���Ϣ�� */
#define INTERNAL_MESSAGE_CREATESESSION_REQUEST          (0x0711)    // �����Ự����
#define INTERNAL_MESSAGE_CREATESESSION_RESPONSE         (0x0712)    // �����Ự��Ӧ
#define INTERNAL_MESSAGE_CREATESESSION_RESULT_NOTIFY    (0x0713)    // �����Ự���֪ͨ
#define INTERNAL_MESSAGE_CLOSESESSION_NOTIFY            (0x0714)    // ���ٻỰ֪ͨ
#define INTERNAL_MESSAGE_HIDESESSION_NOTIFY             (0x0715)     // ���ػỰ֪ͨ

#define INTERNAL_MESSAGE_P2P_TRANS_PROGRESS             (0x0721) //�ļ�����ص��ڲ���Ϣ
#define INTERNAL_MESSAGE_UPLOAD_TRANS_PROGRESS          (0x0722) //�ϴ����Ȼص��ڲ���Ϣ
#define INTERNAL_MESSAGE_DOWNLOAD_TRANS_PROGRESS        (0x0723) //���ؽ��Ȼص��ڲ���Ϣ

#define INTERNAL_MESSAGE_AJAXREQUEST_CALLBACK           (0x0731) // JS Ajax����ص���Ϣ

#define INTERNAL_MESSAGE_SYS_OSLEAVE_CHANGE             (0x0801) //OS�뿪״̬֪ͨ
#define INTERNAL_MESSAGE_SYS_HASFULLSCREEN_CHANGE       (0x0802) //ȫ�����״̬֪ͨ
#define INTERNAL_MESSAGE_CATCHSCREEN_NOTIFY             (0x0803) //��ͼ���֪ͨ
#define INTERNAL_MESSAGE_ZIPLOG_NOTIFY                  (0x0804) //ѹ����־���֪ͨ

#define INTERNAL_MESSAGE_MEETING_REMIND_NOTIFY		    (0x0805) //��������������Ϣ֪ͨ
#define INTERNAL_MESSAGE_GROUP_REMIND_NOTIFY			(0x0806) //����������������Ϣ֪ͨ

#define INTERNLA_MESSAGE_HTTP_RESPONSE                  (0x0807) // http ��Ӧ��Ϣ

#define INTERNAL_MESSAGE_CONNECT_NOTIFY                 (0x0808) //UCAS����״̬��Ϣ
/*
* @class InternalMessage
* @brief Definite a base class for all the internal messages in Tang Client.
* 
* All the internal messages should inherit from this class in Tang Client.
* 
* @author Xiaogang Yu
* @version 1.0.0
*/
class InternalMessage : public Message
{
public:
    InternalMessage()
    {
        // this field identify as tang internal message.
        msgType = TANG_CLIENT_MESSAGE_TYPE_INTERNAL;
    }

    virtual ~InternalMessage(){}

    UINT32          internalMsgType;   /**< The internal message unique identification in client. */
};

/*
* @class ConferenceSystemLockMessage ����ϵͳ�����ڲ���Ϣ
* @brief when the administrator add a system for the conference,this message will be used to notify other
*  modules that a system lock is added.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class ConferenceSystemLockMessage:public InternalMessage
{
public:
    ConferenceSystemLockMessage()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_CONFERENCE_SYSTEM_LOCK;

        confID = 0x0000;
    }

    UINT32               confID;
};

/*
* @class UserExitConferenceMessage �û��˻��ڲ���Ϣ
* @brief this message used to notify other business modules that
*  user is exited the conference.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class UserExitConferenceMessage:public InternalMessage
{
public:
    UserExitConferenceMessage()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_USER_EXIT_CONFERENCE;

        userIdList.clear();
    }

    std::vector<UINT32>               userIdList;
};

/*
* @class UserStatusChangedMessage �û�״̬����ڲ���Ϣ
* @brief this message used to notify other business modules that
*  the status of user is changed.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class UserStatusChangedMessage:public InternalMessage
{
public:
    UserStatusChangedMessage()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_USER_STATUS_CHANGED;

        userID = 0x0000;
    }

    UINT32               userID;
    UserStatus           status;
};

/*
* @class UserRoleChangedMessage �û���ɫ����ڲ���Ϣ
* @brief this message used to notify other business modules that
*  user's role is changed.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class UserRoleChangedMessage:public InternalMessage
{
public:
    UserRoleChangedMessage()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_USER_ROLE_CHANGED;

        uRoleIDList.clear();
    }

    UINT32                          userID;
    std::vector<UINT32>             uRoleIDList;
};

/*
* @class DtsChannelInfoNotify Dtsͨ����Ϣ�ڲ���Ϣ
* @brief this message used to notify other business modules that
*  Dts channel info is ready which module required.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class DtsChannelInfoNotify:public InternalMessage
{
public:
	DtsChannelInfoNotify()
	{
		//message content.
		internalMsgType = INTERNAL_MESSAGE_ID_DTS_CHANNEL_INFO;

		uGroupId = 0x0000;
		uChannelId = 0x0000;
	}

	UINT32                          uGroupId;
	UINT32							uChannelId;
};

/*
* @class UserListTdcReadyNotify �û��б�tdc��ʼ���ڲ���Ϣ
* @brief this message used to notify other business modules that
*  user list tdc is initialized.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class UserListTdcReadyNotify:public InternalMessage
{
public:
	UserListTdcReadyNotify()
	{
		//message content.
		internalMsgType = INTERNAL_MESSAGE_ID_USER_TDC_READY;
	}
};

/*
* @class DownloadPluginFailedNotify �û��б�tdc��ʼ���ڲ���Ϣ
* @brief this message used to notify conference Manager that
*  the specific plugin download failed.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class CreateServiceFailedNotify:public InternalMessage
{
public:
    CreateServiceFailedNotify()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_DOWNLOAD_PLUGIN_FAILED;

        uServiceType = 0;
        hRet = 0;
    }

    UINT32                          uServiceType;
    HRESULT                         hRet;
};

/*
* @class DesktopSettingChangeNotify �û��б�tdc��ʼ���ڲ���Ϣ
* @brief this message used to notify conference Manager that
*  desktop setting is changed.
* 
* This message is inherited from InternaMessage base class .
* 
* @author hongwei.hu
* @version 1.0.0
*/
class DesktopSettingChangeNotify:public InternalMessage
{
public:
    DesktopSettingChangeNotify()
    {
        //message content.
        internalMsgType = INTERNAL_MESSAGE_ID_SETTING_CHANGE;
    }
};

/*
* @class ScreenLockChangeNotify ����״̬�ı��ڲ���Ϣ
* @brief this message used to notify conference Manager that
*  ScreenLock status is changed.
* 
* This message is inherited from InternaMessage base class .
* 
* @author huajian.yin
* @version 1.0.0
*/
class ScreenLockChangeNotify:public InternalMessage
{
public:
	ScreenLockChangeNotify()
	{
		//message content.
		m_iStatus = 0;//lockstatus��1��lock��0��unlock
		internalMsgType = INTERNAL_MESSAGE_ID_SCREENLOCK_CHANGE;
	}
public:
	UINT32 m_iStatus;
};

class LogUploadStatus:public InternalMessage
{
public:
	LogUploadStatus()
	{
		m_iStatus = 0;
		msgType = TANG_CLIENT_MESSAGE_TYPE_INTERNAL;
		internalMsgType = INTERNAL_MESSAGE_ID_UPLOADLOGSTATUS;
	}

public:
	UINT32 m_iStatus;
};

class LogUploadProgress:public InternalMessage
{
public:
	LogUploadProgress()
	{
		m_iProgress = 0;
		msgType = TANG_CLIENT_MESSAGE_TYPE_INTERNAL;
		internalMsgType = INTERNAL_MESSAGE_ID_UPLOADLOGPROGRESS;
	}

public:
	UINT32 m_iProgress;
};

class LogFileUrl:public InternalMessage
{
public:
	LogFileUrl()
	{
		m_strUrl = L"";
		msgType = TANG_CLIENT_MESSAGE_TYPE_INTERNAL;
		internalMsgType = INTERNAL_MESSAGE_ID_LOGFILEURL;
	}

public:
	std::wstring m_strUrl;
};

#ifdef _DEBUG
class SendReportMessage:public InternalMessage
{
public:
    SendReportMessage(BYTE* lpSendBuffer, UINT32 uBufferSize)
    {
        internalMsgType = INTERNAL_MESSAGE_ID_SEND_MESSAGE_REPORT;

		m_lpSendBuffer = lpSendBuffer;
		m_uBufferSize = uBufferSize;
    }

	~SendReportMessage()
	{
        if (NULL != m_lpSendBuffer)
        {
            delete m_lpSendBuffer;
            m_lpSendBuffer = NULL;
        }
	}

	UINT32 m_uBufferSize;
	BYTE* m_lpSendBuffer;
};
#endif

#endif //GNET_TANG_CLIENT_INTERNALMESSAGE_H