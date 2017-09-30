#ifndef __GROUPLLISTSTORE_H__
#define __GROUPLLISTSTORE_H__

#include "UCSDK.h"
#include "IGroupService.h"
#include "UCMessage.h"
#include <boost/thread.hpp>
namespace uc {

#define CURRENT_USER_IN_GROUP       0
#define CURRENT_USER_NOT_IN_GROUP   1
#define GROUP_STATE_OPENED          0
#define GROUP_STATE_CLOSED          1

typedef std::list<GroupInfo> GroupList;

enum  GET_STATUS { 
	GROUPLIST_FROM_MEMORY = 0, 
	GROUPLIST_NEED_FROM_SERVER 
}; 

	

class GroupListStore
{
public:
	GroupListStore() :_synced(false), _lastModifyTime(0){};
	~GroupListStore();
public:

	/**
	* �ⲿ���ô�DB���ȡ����
	*/

	void LoadDataFromDB();

	/**
	* ��ȡ�������б�;
	* @param groupType          ���������������;
	* @param page               �����������ҳ����Ĭ��1;
	* @param count              �����������������Ĭ��200;
	* @param lastGroupTime      ��һ�������������б�ʱ�䣬Ĭ��0;
	* @param allData		     �Ƿ���Ҫ������������Ϣ��true ��Ҫ; false: ����Ҫ;
	* @return					 ����״̬;
	*/
	GET_STATUS GetGroupList(GroupStyle groupType, int32_t page, int32_t count, int32_t lastGroupTime, bool allData, bool update, OUT GroupList &groupList);

	/**
	* �洢�������б�;
	* @param groupList          �������б�;
	* @param synced             �Ƿ��Ѿ�ͬ���ˣ�false����Ҫͬ����DB��������������ڴ�
	* @return					
	*/
	void StoreGroupList(GroupList groupList,bool synced);

	/**
	* ��ȡ����޸�ʱ��;
	* @return ����޸�ʱ�� int64_t
	*/
	int64_t GetLastModifyTime();

	/**
	* ���������飬 ���û����cache_list����push_back;
	* @param info        ������;
	* @return
	*/
	void UpdateGroupInfo(GroupInfo & info);

	/**
	* �����ڴ��е�Group logo(Avatar), ͬʱдDB�������������±�����Ŀ��ͷ���;
	* @param groupId          ������Id;
	* @param localPath        logo·��;
	* @return
	*/
	int32_t UpdateGroupLogo(int64_t groupId, const std::string& localPath);

	/**
	* �����ڴ��е�Group ��Ϣ;
	* @param groupid					id;
	* @param name						��������Ϣget_group_name;
	* @param pinyiName					��������Ϣget_name_pinyin;
	* @return �Ƿ������ true ���£� false δ����
	*/
	bool UpdateGroupName(int64_t groupId, const std::string &name,const std::string & pinyiName);


	/**
	* �����ڴ��е�Group Avatar Index ��Ϣ;
	* @param groupid					id;
	* @param avatarIndex			������Ϣ��get_avatar
	* @return �Ƿ������ true ���£� false δ����
	*/
	bool UpdateGroupAvatarIndex(int64_t groupId,const std::string & avatarIndex);

	/**
	* ����groupid��memberId,ɾ���û�
	* @param groupid			id;
	* @param memberId			��������Ϣget_memberId;
	* @param isSelf				�Ƿ��Լ�
	* @return
	*/
	void DeleteMember(int64_t groupId, const int32_t & memberId,bool isSelf);

	/**
	* ����groupidɾ��group;
	* @param groupid					id;
	* @return
	*/
	void CloseGroup(int64_t groupId);

	/**
	* ���user��group (�����ǰgroup�����ڣ������);
	* @param groupid					id;
	* @param memberInfo				    �û���Ϣ;
	* @param isSelf				        �Ƿ��Լ�
	* @return
	*/
	void AddMemberInGroup(int64_t groupId, const MemberInfo& memberInfo, bool isSelf);

	/**
	* ���������
	* @param groupid					id;
	* @param state				������get_state;
	* @return
	*/
	void SetGroupDisturb(int64_t groupId, int32_t state);

	/**
	* ����Top
	* @param groupid					id;
	* @param state				������get_isTop;
	* @return
	*/
	void SetGroupTop(int64_t groupId, int32_t isTop);

	/**
	* ����Alert����
	* @param groupid					id;
	* @param state				������get_state;
	* @param rule				������get_rule;
	* @param time				������get_time;
	* @return
	*/
	void SetGroupAlertCondition(int64_t groupId, int8_t state, int32_t rule, int32_t time);

	/**
	* ����ɾ��group member
	* @param groupid					id;
	* @param userList				��Ҫɾ�����б�;
	* @return
	*/
	void RemoveMemberInfoById(int64_t groupId, std::vector<int32_t> userList);

	/**
	* ��ȡGroupInfo
	* @param groupid					id;
	* @param groupInfo				���Σ�groupInfo;
	* @return							�Ƿ������ݣ� �����ڴ�cache�������ݣ���Ӧ��û��
	*/
	bool GetGroupInfo(int64_t groupId, OUT GroupInfo &groupInfo);

	/**
	* ����group call״̬
	* @param groupid					id;
	* @param status				group call״̬;
	* @return					
	*/
	bool UpdateStatusInfo(int64_t groupId, const std::string& status);

	/**
	* ��ȡGetGroupInfoByMountId
	* @param mountId					���̿�id;
	* @param groupInfo				���Σ�groupInfo;
	* @return							�Ƿ������ݣ� �����ڴ�cache�������ݣ���Ӧ��û��
	*/
	bool GetGroupInfoByMountId(int32_t mountId, OUT GroupInfo &groupInfo);
	
   /**
	* ͨ���ⲿID��ȡ������ID
	* @param externalId					�ⲿID;
	* @param groupInfo				���Σ�groupInfo;
	* @return							�Ƿ������ݣ� �����ڴ�cache�������ݣ���Ӧ��û��;
	*/
	bool GetGroupInfoByExternalId(const string& externalId, GroupInfo& info);

	/**
	* Clear message cache in memory
	*/
	void Clear();

	/**
	* reset _sync flag
	*/
	void ResetSync();

private:
	/*
	* ��DB�н�grouplist��Ϣload���ڴ���
	*/
	void _LoadGroupListFromDB();

	void _UpdateLastModifyTime();

private:

	GroupInfo* _GetGroupInfoById(int64_t groupId);
	GroupInfo* _GetGroupInfoByMountId(int32_t mountId);
	void _RemoveGroupInfoById(int64_t groupId);

	MemberList* _GetMemberListById(int64_t groupId);
	MemberInfo _GetMemberInfoById(int64_t groupId, int32_t userId);

	void _AddGroupMember(int64_t groupId, const MemberInfo& memberInfo);
	void _RemoveMemberListById(int64_t groupId);
	void _RemoveMemberInfoById(int64_t groupId, int32_t userId);
	void _RemoveMemberInfoById(int64_t groupId, std::vector<int32_t> userList);
	void _RemoveMemberInfoById(const GroupInfo& groupInfo, int32_t userId);
	void _SaveGroup(GroupList groupList);

private:
	GroupList					_group_list_cache;
	boost::recursive_mutex		_group_list_mutex;

	GET_STATUS _get_status;
	bool _synced; // �Ƿ�ͷ�����ͬ����

	int64_t						_lastModifyTime;

};




}

#endif
