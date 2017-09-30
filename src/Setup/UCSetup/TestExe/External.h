/******************************************************************************
* Copyright (c) 2009~2010 All Rights Reserved by
*  G-Net Integrated Service co. Ltd. 

******************************************************************************/
/**
* @file External.h
* @brief defined External Object to support browser call native function
* and native call script function.External provides the APIs of inserting,
* removing and getting the adaptor isntance for native implementation.
*
* @author Hongwei Hu (hongwei.hu@gnetis.com)
*
* @date Janu. 26 2010
*
* @version 1.0.0
* 
* @warning 
*
* Revision History
* @if  CR/PR ID    Author          Date            Major Change @endif
* @bug
******************************************************************************/

#pragma once

/*----------------------------HEADER FILE INCLUDES---------------------------*/
#include "ExternalDispatch.h"
#include "IExternal.h"
#include <map>
#include <comdef.h>
#include <comdefsp.h>

//using namespace std;

typedef std::map<_bstr_t, IDispatch*> MODULE_ADAPTOR_MAP;
//daming: 2011-08-09 ���Ӱ�serviceType��ѯ�Ĺ��ܣ����� ServiceList ���԰�ID��ѯ
typedef std::map<int, IDispatch*> MODULE_ADAPTOR_BY_TYPE_MAP;
//typedef std::map<IDispatch*, int> MODULE_TYPE_BY_DISPATCH_MAP;

/**
* @class External
* @brief External is the collection of module adapter.it will be used by UI JS code.
* 
* @author Hongwei Hu
* @version 1.0.0
*/
class External: public IExternal, public ExternalDispatch<External>
{
public:
    External();
    ~External();
	 
	/**< Adaptor���Խӿں���*/
    IDispatch * WINAPI get_ModuleAdaptor(BSTR bstrAdapter);

    /**< IExternal interface*/
	/**< ��adaptor map����ӽӿ�ʵ������native����*/
	HRESULT virtual PutPluginObject(BSTR name, IDispatch* pDis);
	
	/**< IExternal interface*/
	/**< ��adaptor map����ӽӿ�ʵ������native����*/
	//daming: 2011-08-09 ����serviceType���� ServiceList ���԰�ID��ѯ
	HRESULT virtual PutPluginObjectWithType(int serviceType, BSTR name, IDispatch* pDis);

	/**< ͨ�����ֵõ�Adaptorʵ������native����*/
	HRESULT virtual GetPluginObject(BSTR name, IDispatch** pDis);
	
	/**< ɾ������ӵ�adaptorʵ��*/
	HRESULT virtual RemovePluginObject(BSTR name);

protected:

	//daming: 2011-08-09 
	void DeleteByTypeMap( int nServiceType,BSTR bstrServiceName );

protected:
	MODULE_ADAPTOR_MAP m_adaptorMap;            /**< map of the adapter name and object*/
	//daming: 2011-08-09 ���Ӱ�serviceType��ѯ�Ĺ��ܣ����� ServiceList ���԰�ID��ѯ
	MODULE_ADAPTOR_BY_TYPE_MAP m_adaptorByTypeMap;
	//daming: 2011-08-09 ���Ӱ�IDispatch*��ѯ�������͵Ļ��ƣ�Ϊɾ���ṩ����
	//MODULE_TYPE_BY_DISPATCH_MAP	m_typeByDispatchMap;
   // CRITICAL_SECTION   m_adaptorMaplock;        /**< shared resource lock*/
};