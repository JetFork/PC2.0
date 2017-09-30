#include "AudioServiceSink.h"
#include "LogManager.h"

AudioServiceSink::AudioServiceSink(int conferenceId) {

}

AudioServiceSink::~AudioServiceSink() {

}

void AudioServiceSink::onSessionErrorHandle(CGNetTangSessionErrorInfo *pErrorInfo) {
	if (pErrorInfo == NULL || 
		pErrorInfo->description == NULL || 
		pErrorInfo->statusCode == NULL) {
		UC_LOG(INFO) << "onSessionErrorHandle input pErrorInfo is NULL";
		return;
	}

	UC_LOG(INFO) << "AudioServiceSink onSessionErrorHandle, description: " << pErrorInfo->description << ", statusCode: " << pErrorInfo->statusCode;
	if (NULL == m_pDelegate) return;

	if (DoesCallPhoneError(pErrorInfo->description)) {
		m_pDelegate->OnCallPhoneFailed(pErrorInfo->description, pErrorInfo->statusCode, m_conferenceId);
	}
}

void AudioServiceSink::onAudioSessionPropertyChanged(const char *propName,
	const CGNetTangVariant &oldPropValue,
	const CGNetTangVariant &newPropValue) {
	UC_LOG(INFO) << "onAudioPropertyChanged: propName: " << propName;
	if (NULL == m_pDelegate) return;

	if (strcmp(propName, "networkQuality") == 0) {
		//0:�ǳ���;1:�ܺ�;2:��ͨ;3:��;4:�ܲ�
		UC_LOG(INFO) << "networkQuality changed, Quality, old:" << oldPropValue.uintVal 
			<< ", new:" << newPropValue.uintVal;
		m_pDelegate->OnVoipNetworkQuality(newPropValue.uintVal);
	}
	else if (strcmp(propName, "loudSpeakerStatus") == 0) {
		UC_LOG(INFO) << "loudSpeakerStatus changed, Status: " << newPropValue.boolVal;
		m_pDelegate->OnLoudSpeakerStatusChanged((bool)newPropValue.boolVal);
	}
	else if (strcmp(propName, "wizardMicLevel") == 0) {
		//��˷統ǰlevel 0-65535
		UC_LOG(INFO) << "wizardMicLevel changed, Level: " << newPropValue.uintVal;
		m_pDelegate->OnMicrophoneStatusChanged(0, newPropValue.uintVal);
	}
	else if (strcmp(propName, "microVolume") == 0) {
		//��˷統ǰvolume 0-65535
		UC_LOG(INFO) << "microVolume changed, Volume: " << newPropValue.uintVal;
		m_pDelegate->OnMicrophoneStatusChanged(1, newPropValue.uintVal);
	}
	else if (strcmp(propName, "wizardSpeakerLevel") == 0) {
		//��������ǰlevel 0-65535
		UC_LOG(INFO) << "wizardSpeakerLevel changed, Level: " << newPropValue.uintVal;
		m_pDelegate->OnSpeakerStatusChanged(0, newPropValue.uintVal);
	}
	else if (strcmp(propName, "speakerVolume") == 0) {
		//��������ǰvolume 0-65535
		UC_LOG(INFO) << "speakerVolume changed, Volume: " << newPropValue.uintVal;
		m_pDelegate->OnSpeakerStatusChanged(1, newPropValue.uintVal);
	}
	else if (strcmp(propName, "audioDtsReady") == 0) {
		//��Ƶ�豸׼����
		UC_LOG(INFO) << "audioDtsReady, isReady:" << newPropValue.uintVal;
		m_pDelegate->OnAudioServiceReady(newPropValue.uintVal);
	}
	else {
		//TODO
	}
}

void AudioServiceSink::onAudioNetWorkCallBack(UINT32 nJitterMax, UINT32 nJitterAverage, 
	UINT32 nLostRate, UINT32 nUnsortRate, UINT32 nSendSuccessRate) {
	UC_LOG(INFO) << "onAudioNetWorkCallBack, nJitterMax:" << nJitterMax << ", nJitterAverage:" << nJitterAverage
		<< ", nLostRate:" << nLostRate << ", nUnsortRate:" << nUnsortRate << ", nSendSuccessRate:" << nSendSuccessRate;
}

bool AudioServiceSink::DoesCallPhoneError(const std::string &errorDescription) {
	size_t len = strlen(errorDescription.c_str());
	return isdigit(errorDescription[errorDescription.length() - 1]);
}