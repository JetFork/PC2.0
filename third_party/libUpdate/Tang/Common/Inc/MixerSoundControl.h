#pragma once

#include "ISoundControl.h"

class MixerSoundControl :
	public ISoundControl
{
public:
	MixerSoundControl(void);
public:
	virtual ~MixerSoundControl(void);

public:
	HRESULT SetOutVolume(DWORD dwOutVolume);					/**< ���ò�������ֵ */	
	HRESULT IsSupportRecordInAndOutSound(UINT32 & bSupport);		/**< �Ƿ�֧��ͬʱ¼����˷�������������� */
	HRESULT EnableStereoMix();									/**< �������������� */
	HRESULT DisableStereoMix();									/**< �������������� */
};
