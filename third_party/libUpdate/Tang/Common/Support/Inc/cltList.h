#ifndef TANG_List_h__
#define TANG_List_h__
#include <stdexcept>
#include "RecorderForClient.h"
namespace clt
{    

	
    template<class T>
    class MList   //����������
    {
	public:
		template<class T>
		struct Element;
		typedef StablePtr< Element<T> > ElementPtr;

		template<class T>
		struct Element
		{
			T data;                         //��ŵ�����
			ElementPtr next;                  //ָ���¸�Ԫ�ص�ָ��
			ElementPtr previous;              //ָ��ǰһ��Ԫ�ص�ָ��
			Element(T const& d,const ElementPtr & n,const ElementPtr & p)
				:data(d),
				next(n),
				previous(p)
			{
			}
			void *(operator new)(unsigned int uiSize)
			{
				return clt::Alloc(uiSize);
			}
			void  (operator delete)(void * p)
			{
				clt::Free(p);
			}
		};

    public:
        MList();  
        ~MList(); 
        MList( MList const& );
        MList& operator=( MList const& );
        T& operator[]( unsigned int index );                     //���������±��ȡֵ
        T &  front()const;                             //ȡ������ͷ��ַ
        T &  back()const;                              //ȡ������β��ַ
        bool empty()const;                                  //�ж������Ƿ�Ϊ��
        unsigned int size();                                //��ȡ�����Ԫ�ظ���
        bool push_front( T const & );                          //�������ͷԪ��ǰ�����һ��Ԫ��
        bool push_back( T const & );                           //������βԪ�غ����һ��Ԫ��
        void remove( T const &);
        void erase( T const &);                               //��������ɾ��ĳһ�ض���
        void clear();                                       //������������Ԫ��,��һһɾ��֮
        bool Insert( unsigned int index,T const &);           //��ָ��λ�ò���ڵ�
        int find( T const& );                               //����ֵ�������е�λ�� 

    protected:
        ElementPtr m_head;          //����ͷ��ַ
        ElementPtr m_tail;          //����β��ַ
    private:
        unsigned int m_size;
        ElementPtr  m_cursor;
        unsigned int m_cursorindex;
    };

    template<class T>
    MList<T>::MList():
    m_head(0),
        m_tail(0),
        m_size(0),
        m_cursor(0),
        m_cursorindex(0)
    {

    }

    template<class T>
    MList<T>::~MList()
    {
        clear();
    }

    template<class T>
    MList<T>::MList( MList<T> const& lst ):
    m_head(0),
        m_tail(0),
        m_size(0)
    {
        ElementPtr ptr;
        for( ptr=lst.m_head; ptr!=0; ptr=ptr->next )
        {
            push_back( ptr->data );
        }

    }

    template<class T>
    MList<T>& MList<T>::operator=( MList<T> const& lst )
    {
        if( &lst != this )
        {
            clear();
			ElementPtr ptr;
            for( ptr = lst.m_head; ptr != 0; ptr = ptr->next )
            {
                push_back( ptr->data );
            }
        }
        return *this;
    }
    template<class T>
    T&  MList<T>::operator[]( unsigned int index )
    {	
        if (empty())
        {
            throw std::invalid_argument("the MList is empty!");
        }
		if ( index >= m_size )
		{
			 throw std::invalid_argument("the index is out of list size!");
		}
 		if ( index == m_cursorindex )
 		{
 			 return m_cursor->data;
 		}
         else if ( index > m_cursorindex )
         {
             while (m_cursorindex != index)
             {
                 m_cursor = m_cursor->next;
                 m_cursorindex++;
             }
             return m_cursor->data;
         }
         else
         {
             while(m_cursorindex != index )
             {
                 m_cursor = m_cursor->previous;
 				
                 m_cursorindex--;
             }
             return m_cursor->data;
         }
    }

    template<class T>
    void MList<T>::clear()
    {
        while( m_head != 0 )
        {
            ElementPtr tmp = m_head;
            m_head = m_head->next;
            tmp.Destroy();
            m_size--;
        }
        m_tail = 0;
        m_cursor = 0;
        m_cursorindex = 0;
    }

    template<class T>
    T &  MList<T>::front() const
    {
        if ( !empty() )
        {
            return m_head->data;
        }
        else
        {
            throw std::invalid_argument("the MList is empty!");
        }
    }

    template<class T>
    T &  MList<T>::back() const
    {
        if (!empty())
        {
            return m_tail->data;
        }
        else
        {
            throw std::invalid_argument("the MList is empty!");
        }
    }

    template<class T>
    bool MList<T>::empty() const
    {
        return m_head == 0;
    }

    template<class T>
    unsigned int MList<T>::size()
    {
        return m_size;
    }

    template<class T>
    bool MList<T>::push_front( T const & item )
    {
        ElementPtr tmp = new Element<T>( item,m_head,0 );
        if ( tmp == 0 )
        {
            return false;
        }
        if( m_head == 0 )
        {
            m_tail = tmp;
            m_cursor = tmp;
            m_cursorindex = 0;
        }
        else
        {
            m_cursorindex++;
        }
        m_head = tmp;
        m_size++;
        return true;
    }

    template<class T>
    bool MList<T>::push_back( T const & item )
    {
        ElementPtr tmp = new Element<T>( item,0,m_tail );
        if ( tmp == 0 )
        {
            return false;
        }
        if( m_head == 0 )
        {
            m_head = tmp;
            m_cursor = tmp;
            m_cursorindex = 0;
        }
        else
            m_tail->next = tmp;
        m_tail = tmp;
        m_size++;
        return true;
    }

    template<class T>
    void MList<T>::remove( T const & item)
    {
        erase(item);
    }

    template<class T>
    void MList<T>::erase( T const & item )
    {
        ElementPtr ptr = m_head;
        while( ptr!=0 && ptr->data != item )
        {
            ptr = ptr->next;
        }
        if( ptr == 0 )
        {
            //throw std::invalid_argument("item not found!");
            return;
        }
        if( ptr == m_head )
        {
            m_head = ptr->next;
            if (m_head != NULL)
            {
                m_head->previous = 0;
            }
            else
            {
                m_tail = m_head;
            }
            
        }
        else if( ptr == m_tail )
        {
            m_tail = ptr->previous;
            if (m_tail != NULL)
            {
                m_tail->next = 0;
            }
            else
            {
                m_head = m_tail;
            }
            
        }
        else
        {
            ptr->previous->next=ptr->next;
            ptr->next->previous = ptr->previous;
        }
        ptr.Destroy();
        m_size--;
        m_cursor = m_head;
        m_cursorindex = 0;
    }

    template<class T>
    bool MList<T>::Insert( unsigned int index,T const & item )
    {
        if ( index>= m_size )
        {
            return false;
        }
        if ( 0 == index)
        {
            return push_front(item);
        }
        else if ( m_size == (index +1) )
        {
            return push_back(item);
        }
        else
        {
            int i = 0;
            ElementPtr ptr = 0;
            for (ptr = m_head; ptr!=0; ptr = ptr->next )
            {
                if ( i== (index -1) )
                {
                    break;
                }
                i++;
            }
            if ( ptr == 0 )
            {
                return false;
            }
            ElementPtr tmp = new Element<T>(item,ptr->next,ptr);
            if ( tmp == 0 )
            {
                return false;
            }
            ptr->next->previous = tmp;
            ptr->next = tmp;
            m_size++;
            m_cursor = m_head;
            m_cursorindex = 0;
            return true;
        }
    }

    template<class T>
    int MList<T>::find( T const& item)
    {
        int i = 0;
        ElementPtr ptr = m_head;
        while (  ptr != 0 && ptr->data != item )
        {
            ptr = ptr->next;
            i++;
        }
        if ( ptr == 0 )
        {
            return -1;
        }
        else
        {
            return i;
        }
    }
}
#endif //TANG_List_h__