#pragma once
#include <MsHTML.h>
class ElementHelper
{
private:
    ElementHelper(void);
    ~ElementHelper(void);
public:
    static HRESULT GetDocument(
        IHTMLElement * pElement,
        IHTMLDocument2 ** pDocument);

    static HRESULT GetWindow(
        IHTMLElement * pElement,
        IHTMLWindow2 ** pWindow);

    static HRESULT GetScreen(
        IHTMLElement * pElement,
        IHTMLScreen ** pScreen);

    static HRESULT CreateElement(
        LPCTSTR szTag,
        LPCTSTR szID,
        IHTMLElement * pParentElement,
        IHTMLElement ** pNewElement,
        LPCTSTR szStyle = NULL);

    static HRESULT CreateElement(
        LPCTSTR szTag,
        IHTMLElement * pParentElement,
        IHTMLElement ** pNewElement);

    static HRESULT GetElement(
        LPCTSTR szID,
        IHTMLElement **pElement,
        IHTMLElement * pParentElement);

    static HRESULT AppendElement(
        IHTMLElement *pElement,
        IHTMLElement * pParentElement);

    static HRESULT GetFirstChild(
        IHTMLElement **pElement,
        IHTMLElement * pParentElement);

    static HRESULT RemoveFirstChild(
        IHTMLElement * pParentElement);

    static HRESULT RemoveElement(IHTMLElement * pElement);

    static HRESULT RemoveChild(
        IHTMLElement * pParentElement, 
        IHTMLElement * pElement);

    static HRESULT SetSytleWidthAndHeight(
        IHTMLElement * pElement,
        long  lWidth,
        long  lHeight);

	static HRESULT SetSytleFilter(
		IHTMLElement * pElement,
		LPCTSTR lpFilter);

    static HRESULT GetSytleWidthAndHeight(
        IHTMLElement * pElement,
        long & lWidth,
        long & lHeight);

    static HRESULT GetRealWidthAndHeight(
        IHTMLElement * pElement,
        long & lWidth,
        long & lHeight);

    static HRESULT GetViewClientArea(IHTMLElement * pElement,
        long & lWidth,
        long & lHeight,
        long & lScrollLeft,// ���û��ȡλ�ڶ�����߽�ʹ�����Ŀǰ�ɼ����ݵ������֮��ľ���
        long & lScrollTop);// ���û��ȡλ�ڶ�����˺ʹ����пɼ����ݵ����֮��ľ���

	static HRESULT SetAttribute(IHTMLElement * pElement,LPCTSTR lpName,LPCTSTR lpValue);
	
	static HRESULT RemoveAttribute( IHTMLElement * pElement,LPCTSTR lpName );

    static HRESULT GetScrollBarWidth(
        IHTMLElement * pElement,
        long & lWidth);

    static HRESULT GetAbsolutePosition(
        IHTMLElement * pElement,
        long * pLeft,
        long * pTop,
        long * pRight = NULL,
        long * pBottom = NULL);

    static HRESULT GetRelativePosition(
        IHTMLElement * pElement,
        long * pLeft,
        long * pTop,
        long * pRight = NULL,
        long * pBottom = NULL);

    static HRESULT Zoom(IHTMLElement * pElement,UINT16 uScalling);

    static HRESULT GetZoomValue(IHTMLElement * pElement,UINT16& uScalling);

    static HRESULT Show(IHTMLElement * pElement);
     
    static HRESULT Hide(IHTMLElement * pElement);

	static BOOL IsDisplay(IHTMLElement * pElement);

    /** @brief ���¹���˵����λ��, Ĭ�������ţ�scale/100�� */
    static HRESULT ScrollIntoView(IUnknown *pView, int scale, UINT16 x, UINT16 y);

	static HRESULT SetBackgroundImage( IHTMLElement * pElement,LPCTSTR lpSrc );
private:
    static long m_lScrollBarWidth;

};
