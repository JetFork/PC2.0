/*
 * SharingService.h
 *
 *  Created on: May 6, 2015
 *      Author: ethan
 */

#ifndef SHARINGSERVICE_H_
#define SHARINGSERVICE_H_

#include "ISharingService.h"
#include "conferencesdk/ConferenceWrapper.h"

namespace uc {

	enum ShareType {
		NO_SHARE = 0, 
		SHARE_USER, //������
		VIEW_USER //�ۿ���
	};

	class SharingService : public ISharingService, ISharingServiceEvent {
public:
	SharingService(IUCEngine *engine);
	virtual ~SharingService();

	//��ʼ��������
	virtual int StartDesktopShare();
	//ֹͣ��������
	virtual int StopDesktopShare();

	//��ʼ�ۿ�����
	virtual int StartDesktopView(VIEW view);
	//ֹͣ�ۿ�����
	virtual int StopDesktopView();

	int Close();

	//�ص��¼�
	virtual void OnErrorHandle(int statusCode);
	virtual void OnDesktopShared(int conferenceId, int userId);
	virtual void OnDesktopShareStoped(int conferenceId, int userId);
	virtual void OnDesktopViewerStarted(int conferenceId);
	virtual void OnDesktopViewerStopped(int conferenceId);

	private:
		ShareType _sharingType;
};

} /* namespace uc */

#endif /* SHARINGSERVICE_H_ */
