#pragma once

#ifdef RECORDERSTARTINFODLL_EXPORTS
#define RECORDERSTARTINFOPORT extern "C" __declspec(dllexport)
#else
#define RECORDERSTARTINFOPORT extern "C" __declspec(dllimport)
#endif

#define SERVICEINFO_COUNT   590               // the service information count

namespace clt
{
	template<typename Ty>
	class StablePtr
	{	// wrap an object pointer to ensure destruction
	public:
		StablePtr(Ty *ptr = 0) throw()
			:m_ptr(0)
		{	// construct from object pointer
			reset(ptr);
		}

		StablePtr(const StablePtr<Ty>& right) throw()
			:m_ptr(right.m_ptr)
		{	
		}

		template<class Other>
		StablePtr(const StablePtr<Other>& right) throw()
			: m_ptr(right.m_ptr)
		{	// construct by assuming pointer from _Right
		}

		StablePtr& operator=( const Ty * ptr) throw()
		{	// construct from object pointer
			reset(ptr);
			return *this;
		}

		template<class Other>
		StablePtr& operator=( const Other * ptr) throw()
		{	// construct from object pointer
			reset(ptr);
			return *this;
		}

		StablePtr<Ty>& operator=( const StablePtr<Ty>& right ) throw()
		{	// assign compatible _Right (assume pointer)
			m_ptr = right.m_ptr;
			return (*this);
		}

		template<class Other>
		StablePtr<Ty>& operator=( const StablePtr<Other>& right ) throw()
		{
			m_ptr = right.m_ptr;
			return (*this);
		}

		~StablePtr()
		{	// destroy the object
			m_ptr = 0;
		}

		Ty& operator*() const throw()
		{	
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			return (*pItem);
		}

		Ty *operator->() const throw()
		{	// return pointer to class object
			return (&**this);
		}

		const Ty* Value() const throw()
		{
			return (&**this);
		}

//  		Ty* Get() const throw()
//  		{
//  			return (&**this);
//  		}

		BOOL operator==( const Ty * ptr) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return TRUE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return TRUE;
			}
			return FALSE;
		}

		template<class Other>
		BOOL operator==( const Other * ptr ) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return TRUE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return TRUE;
			}
			return FALSE;
		}

		template<DWORD>
		BOOL operator==(DWORD ptr) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return TRUE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return TRUE;
			}
			return FALSE;
		}

		BOOL operator==(const StablePtr<Ty>& right) const throw()
		{
			return right.m_ptr == m_ptr;
		}

		BOOL operator!=(const Ty * ptr) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return FALSE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return FALSE;
			}
			return TRUE;
		}

		template<class Other>
		BOOL operator!=(const Other * ptr) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return FALSE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return FALSE;
			}
			return TRUE;
		}

		template<DWORD>
		BOOL operator!=(DWORD ptr) const throw()
		{
			if ( 0 == m_ptr && 0 == ptr)
			{
				return FALSE;
			}
			Ty * pItem = (Ty*)(m_ptr + GetBaseAddress());
			if ( pItem == ptr )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL operator!=(const StablePtr<Ty>& right) const throw()
		{
			return right.m_ptr != m_ptr;
		}

		void Destroy()
		{
			Release<Ty>();
		}

	private:
		template<typename Other>
		void Release()
		{
			if ( 0 != m_ptr )
			{
				Other * pItem = (Other*)((DWORD)m_ptr + GetBaseAddress());
				delete  pItem;
				m_ptr = 0;
			}
		}
		template<> void Release<int>()
		{
			if ( 0 != m_ptr )
			{
				int * pItem = (int*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<UINT16>()
		{
			if ( 0 != m_ptr )
			{
				UINT16 * pItem = (UINT16*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<UINT32>()
		{
			if ( 0 != m_ptr )
			{
				UINT32 * pItem = (UINT32*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<LONG>()
		{
			if ( 0 != m_ptr )
			{
				LONG * pItem = (LONG*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<DWORD>()
		{
			if ( 0 != m_ptr )
			{
				DWORD * pItem = (DWORD*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<double>()
		{
			if ( 0 != m_ptr )
			{
				double * pItem = (double*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<float>()
		{
			if ( 0 != m_ptr )
			{
				float * pItem = (float*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<char>()
		{
			if ( 0 != m_ptr )
			{
				char * pItem = (char*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<WCHAR>()
		{
			if ( 0 != m_ptr )
			{
				WCHAR * pItem = (WCHAR*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		template<> void Release<UCHAR>()
		{
			if ( 0 != m_ptr )
			{
				UCHAR * pItem = (UCHAR*)((DWORD)m_ptr + GetBaseAddress());
				clt::Free( pItem );
				m_ptr = 0;
			}
		}
		

		template<class Other>
		void reset( const Other* ptr = NULL ) throw()
		{	// destroy designated object and store new pointer
			if ( NULL != ptr )
			{
				m_ptr = (DWORD)(ptr) - GetBaseAddress();
			}
			else
			{
				m_ptr = 0;
			}
		}

		void reset( const Ty* ptr = NULL ) throw()
		{	// destroy designated object and store new pointer
			if ( NULL != ptr )
			{
				m_ptr = (DWORD)(ptr) - GetBaseAddress();
			}
			else
			{
				m_ptr = 0;
			}
		}

		DWORD release() throw()
		{	// return wrapped pointer and give up ownership
			DWORD Tmp = m_ptr;
			m_ptr = 0;
			return ( Tmp );
		}

	private:
		DWORD m_ptr;	// the wrapped object pointer
	};
	
    struct AccessInfoView
    {
		AccessInfoView()
			:Ptr(NULL)
		{
		}
        StablePtr<DWORD> Ptr;
    };
	
    struct ServiceInfoView
    {
        ServiceInfoView():
        bIsFilled(FALSE),
        uType(0),
        uKey(0),
        Ptr(NULL)
        {

        }

        /**
        * if the value of bIsFilled is FALSE, the memory of the service information is empty
        */
        BOOL bIsFilled;
        /**
        * The type of the service is 0, means the memory is empty.
        */
        UINT32 uType;

        /**
        * The key of this info.Mainly used group id.
        */
        UINT32 uKey;

        /**
        * the pointer to the buffer of the service information
        */
        StablePtr<DWORD> Ptr;
    };

    /**
    * @brief	<b>Summary:</b>	����ClientInfo�ڴ�ӳ������ͼ
    * @detail   �������н�������ã���һ������ֻ�����һ��
    * @caller   Client���̵���
    * @param 
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT CreateViewMappingOfClientInfo();

    /**
    * @brief	<b>Summary:</b>	��ClientInfo�ڴ�ӳ������ͼ
    * @detail   
    * @caller   Client���̵���
    * @param [in] szMappingName: �ڴ�ӳ��������
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT OpenViewMappingOfClientInfo(LPCTSTR szMappingName);

    /**
    * @brief	<b>Summary:</b>	����Daemon����
    * @detail   
    * @caller   Client���̵���
    * @param
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT StartDaemon();

    /**
    * @brief	<b>Summary:</b>	����Daemon����
    * @detail   
    * @caller   Client���̵���
    * @param
    */
    RECORDERSTARTINFOPORT HRESULT StopDaemon();

    /**
    * @brief	<b>Summary:</b>	��Client���̵Ĵ��ھ����д���ڴ��
    * @detail   ��Client���̵Ĵ��ھ����ͨ��д�빲���ڴ棬���ݸ�Daemon���̣�
                Daemon�����õ�Client���̵Ĵ��ھ��
    * @caller   Client���̵���
    * @param [in] hClientWindow:Client���̵Ĵ��ھ��
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT SetClientWindowHandle(HWND hClientWindow);
    RECORDERSTARTINFOPORT TCHAR* GetClientMappingName();

    RECORDERSTARTINFOPORT DWORD GetDaemonThreadID();

    /**
    * @brief	<b>Summary:</b>	��Session���̵ľ����д���ڴ��
    * @detail   ��Session���̵ľ����ͨ��д�빲���ڴ棬���ݸ�Daemon���̣�
                Daemon�����õ�Session���̵ľ��
    * @caller   Client���̵���
    * @param [in] hSessionProcess : Session���̵ľ��
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT AddClientSessionInfo(DWORD dwSessionProcessID, DWORD dwSessionThreadID);

    /**
    * @brief	<b>Summary:</b>	��Session���̵ľ�����Ƴ��ڴ��
    * @caller   Client���̵���
    * @param [in] hSessionProcess : Session���̵ľ��
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT RemoveClientSessionInfo(DWORD dwSessionProcessID, DWORD dwSessionThreadID);

    /**
    * @brief	<b>Summary:</b>	����Client����
    * @detail   
    * @caller   Client���̵���
    * @param [in] szBatPath:�������ļ�·��
    * @param [in] szClientCommandLine:����Client�����в���
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT UpdateClient(
        __in LPCTSTR szBatPath,
        __in LPCTSTR szClientCommandLine);
    /**
    * @brief	<b>Summary:</b>	֪ͨDaemon���̣�Client���̷�������
    * @detail   
    * @caller   Client���̵���
    * @param
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT TellDaemonClientIsCrashed();

    RECORDERSTARTINFOPORT HRESULT TellDaemonSessionCrashed(UINT32 nSessionID, UINT32 nSessionIDType);

    /**
    * @brief	<b>Summary:</b>	��ȡ������Ϣ�ڴ��
    * @detail   ����ڴ��󣬿���ֱ�Ӷ��ڴ����ж�д����
    * @caller   Client���̵���
    * @param
    * @return   ָ�������Ϣ�ڴ�ӳ������ͼ
    */
	RECORDERSTARTINFOPORT AccessInfoView* GetAccessInfoView();

    /**
    * @brief	<b>Summary:</b>	ɾ��������Ϣ
    * @detail   ��������Ϣ�ڴ������
    * @caller   Client���̵���
    * @param
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT DeleteAccessInfoView();

    /**
    * @brief	<b>Summary:</b>	��ȡ������Ϣ�ڴ��
    * @detail   ���ݷ�����Ϣ��type��key�������ڴ�飬
                ���û���ҵ����򷵻�һ�����õ��ڴ�飬���û�����õ��ڴ���򷵻�NULL
    * @caller   Client���̵���
    * @param [in] uServiceType:��������
    * @param [in] uKey:��ֵ
    * @return   ָ�������Ϣ�ڴ�ӳ������ͼ���������ֵ��uServiceTypeΪ0����ʾ����ͼ�����õ�
    */
    RECORDERSTARTINFOPORT ServiceInfoView* GetServiceInfoView(
        __in UINT32 uServiceType, 
        __in UINT32 uKey);

    /**
    * @brief	<b>Summary:</b>	ɾ��������Ϣ
    * @detail   ���ݷ�����Ϣ��type��key�������ڴ�飬�����ڴ������
    * @caller   Client���̵���
    * @param [in] uServiceType:��������
    * @param [in] uKey:��ֵ
    * @return   S_OK:�ɹ���E_FAIL:ʧ��
    */
    RECORDERSTARTINFOPORT HRESULT DeleteServiceInfoView(
        __in UINT32 uServiceType, 
        __in UINT32 uKey);

    /**
    * @brief	<b>Summary:</b>	��̬��ӳ���ڴ�������ռ�
    * @detail   
    * @caller   Client���̵���
    * @param [in] tSize:�ռ��С
    * @return   
    */
    RECORDERSTARTINFOPORT void * Alloc(unsigned int tSize);
    
    /**
    * @brief	<b>Summary:</b>	�ͷ���ӳ���ڴ�������Ŀռ�
    * @detail   
    * @caller   Client���̵���
    * @param [in] p:ָ����Ҫ�ͷſռ���׵�ַ
    * @return   
    */
    RECORDERSTARTINFOPORT void Free( void * p);

	RECORDERSTARTINFOPORT DWORD GetBaseAddress();
}
