#include "StdAfx.h"
#include "SkinEngine.h"
#include "api.h"
#include <output_debug.h>

extern extra_parameters* g_pluginParms;
extern BOOL g_bMSGLoopFlag;
extern DuiLib::CSkinEngine* g_pFrame; 

extern TCHAR g_messageBoxCloseBtnControlName[MAX_PATH]; 
extern  TCHAR g_messageBoxYESBtnControlName[MAX_PATH]; 
extern TCHAR g_messageBoxNOBtnControlName[MAX_PATH]; 

namespace DuiLib {

	CSkinEngine::CSkinEngine()
	{
		this->m_pcodevecs = new std::vector<int>();
		this->m_pnamevecs = new std::vector<CDuiString>();
		DEBUG_INFO("this 0x%p\n",this);
		DEBUG_INFO("codevecs 0x%p namevecs 0x%p\n",this->m_pcodevecs,this->m_pnamevecs);
		DEBUG_INFO("size %d %d\n",this->m_pcodevecs->size(),this->m_pnamevecs->size());
	}
	void CSkinEngine::Notify(TNotifyUI& msg)
	{
		DEBUG_INFO("pSender 0x%p this 0x%p\n",msg.pSender,this);
		CDuiString name = msg.pSender->GetName();
		DEBUG_INFO("after get name\n");
		int i=0 ,findidx=-1;
		DEBUG_INFO("namevec 0x%p \n",this->m_pnamevecs);
		DEBUG_INFO("size %d\n",this->m_pnamevecs->size());
		for (i=0;i< (int) this->m_pnamevecs->size();i++)
		{
			if (name == this->m_pnamevecs->at(i))
			{
				findidx = i;
				break;
			}
		}
		DEBUG_INFO("\n");
		if( _tcsicmp( msg.sType, _T("click") ) == 0 ){
			if( findidx >= 0){
				DEBUG_INFO("\n");
				g_pluginParms->ExecuteCodeSegment( this->m_pcodevecs->at(findidx)- 1, 0 );
				DEBUG_INFO("\n");
			}
		}
		else if( _tcsicmp( msg.sType, _T("textchanged") ) == 0 ){
			if( findidx >= 0){
				DEBUG_INFO("\n");
				g_pluginParms->ExecuteCodeSegment( this->m_pcodevecs->at(findidx)- 1, 0 );
				DEBUG_INFO("\n");
			}
		}
		DEBUG_INFO("\n");
	}

	void CSkinEngine::SaveToControlCallbackMap(CDuiString ctlName, int callback)
	{
	        //m_controlCallbackMap[ctlName] = callback;
	        DEBUG_INFO("this 0x%p pname 0x%p pcode 0x%p\n",this,
	                   this->m_pnamevecs,this->m_pcodevecs);
	        this->m_pnamevecs->push_back(ctlName);
	        this->m_pcodevecs->push_back(callback);
        }

	LRESULT CSkinEngine::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		DEBUG_INFO("\n");
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_pm.Init(m_hWnd);
		DEBUG_INFO("\n");
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create( GetSkinXMLPath(), (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		DEBUG_INFO("pRoot = 0x%p\n",pRoot);
		m_pm.AttachDialog(pRoot);
		DEBUG_INFO("\n");
		m_pm.AddNotifier( this );
		DEBUG_INFO("\n");
		ShowWindow( FALSE );
		return 0;
	}

	LRESULT CSkinEngine::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT CSkinEngine::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CSkinEngine::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CSkinEngine::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CSkinEngine::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
// 		SIZE szRoundCorner = m_pm.GetRoundCorner();
// 		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
// 			CDuiRect rcWnd;
// 			::GetWindowRect(*this, &rcWnd);
// 			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
// 			rcWnd.right++; rcWnd.bottom++;
// 			RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
// 			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
// 			HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
// 			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
// 			::SetWindowRgn(*this, hRgn1, TRUE);
// 			::DeleteObject(hRgn1);
// 			::DeleteObject(hRgn2);
// 		}
// 
// 		bHandled = FALSE;
// 		return 0;
		SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
#endif
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bZoomed = ::IsZoomed(this->GetHWND());
		if ((wParam & 0xFFF0 ) == SC_MAXIMIZE ||(wParam &  0xFFF0) == SC_SIZE)
		{ 
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CSkinEngine::OnTBCIAMinMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return lRes;
	}

	LRESULT CSkinEngine::OnTBCIACloseMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		Close();
		return lRes;
	}

	LRESULT CSkinEngine::OnTBCIABackMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		if( currentIndex == 0 )
			return -1;
		else
			pTab->SelectItem( currentIndex - 1 );

		return 0;
	}

	LRESULT CSkinEngine::OnTBCIANextMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		pTab->SelectItem( currentIndex + 1 );
		return 0;
	}

	LRESULT CSkinEngine::OnTBCIAStartInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		pTab->SelectItem( currentIndex + 1 );

		g_bMSGLoopFlag = FALSE;

		return lRes;
	}

	LRESULT CSkinEngine::OnTBCIAStartUninstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		pTab->SelectItem( currentIndex + 1 );

		g_bMSGLoopFlag = FALSE;

		return lRes;
	}

	LRESULT CSkinEngine::OnTBCIAFinishedInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		Close();
		return lRes;
	}

	LRESULT CSkinEngine::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		DEBUG_INFO("umsg 0x%x:%d wParam 0x%x lParam 0%x\n",uMsg,uMsg,wParam,lParam);
		switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIAMIN:      lRes = OnTBCIAMinMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIACLOSE:   lRes = OnTBCIACloseMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIABACK:     lRes = OnTBCIABackMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIANEXT:     lRes = OnTBCIANextMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIASTARTINSTALL: lRes = OnTBCIAStartInstallMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIASTARTUNINSTALL: lRes = OnTBCIAStartUninstallMSG(uMsg, wParam, lParam, bHandled); break;
	case WM_TBCIAFINISHEDINSTALL: lRes = OnTBCIAFinishedInstallMSG(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		DEBUG_INFO("\n");
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		DEBUG_INFO("\n");
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void  CSkinEngine::SetSkinXMLPath( LPCTSTR path )
	{
		_tcscpy( m_skinXMLPath, path );
	}

	LPCTSTR CSkinEngine::GetSkinXMLPath()
	{
		return m_skinXMLPath;
	}

	CPaintManagerUI& CSkinEngine::GetPaintManager()
	{
		return m_pm;
	}

	//////////////////////////////////////////////////////////////////////////
	///  CTBCIAMessageBox

	void CTBCIAMessageBox::Notify(TNotifyUI& msg)
	{
		CDuiString name = msg.pSender->GetName();
		std::map<CDuiString, int >::iterator iter , finditer  = this->m_controlCallbackMap.end();
		for (iter = this->m_controlCallbackMap.begin();iter != this->m_controlCallbackMap.end();++iter)
		{
			if (name == iter->first)
			{
				finditer = iter;
				break;
			}
		}
		if( _tcsicmp( msg.sType, _T("click") ) == 0 ){
			if( finditer != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( finditer->second - 1, 0 );
			else if( _tcsicmp( msg.pSender->GetName(), g_messageBoxCloseBtnControlName ) == 0 ){
// 				SetParent( this->GetHWND(), NULL );
// 				ShowWindow( SW_HIDE, FALSE );
// 				::EnableWindow(g_pFrame->GetHWND(), TRUE);
// 				::SetFocus(g_pFrame->GetHWND());
				Close();
			}
			else if( _tcsicmp( msg.pSender->GetName(), g_messageBoxYESBtnControlName ) == 0 ){
				PostQuitMessage(0);
			}
			else if( _tcsicmp( msg.pSender->GetName(), g_messageBoxNOBtnControlName ) == 0 ){
// 				SetParent( this->GetHWND(), NULL );
// 				ShowWindow( SW_HIDE, FALSE );
// 				::EnableWindow(g_pFrame->GetHWND(), TRUE);
// 				::SetFocus(g_pFrame->GetHWND());
				Close();
			}
		}
 		else if( _tcsicmp( msg.sType, _T("textchanged") ) == 0 ){
 			if( finditer != m_controlCallbackMap.end() )
 				g_pluginParms->ExecuteCodeSegment( finditer->second - 1, 0 );
 		}
	}

	LRESULT CTBCIAMessageBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		DEBUG_INFO("\n");
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		DEBUG_INFO("\n");
		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create( GetSkinXMLPath(), (UINT)0, NULL, &m_pm);
		DEBUG_INFO("\n");
		ASSERT(pRoot && "Failed to parse XML");
		DEBUG_INFO("pRoot = 0x%p\n",pRoot);
		m_pm.AttachDialog(pRoot);
		DEBUG_INFO("\n");
		m_pm.AddNotifier( this );
		DEBUG_INFO("\n");
		ShowWindow( FALSE );
		DEBUG_INFO("\n");
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT CTBCIAMessageBox::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CTBCIAMessageBox::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
			HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
			::SetWindowRgn(*this, hRgn1, TRUE);
			::DeleteObject(hRgn1);
			::DeleteObject(hRgn2);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT CTBCIAMessageBox::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bZoomed = ::IsZoomed(this->GetHWND());
		if ((wParam & 0xFFF0 ) == SC_MAXIMIZE ||(wParam &  0xFFF0) == SC_SIZE)
		{ 
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CTBCIAMessageBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void  CTBCIAMessageBox::SetSkinXMLPath( LPCTSTR path )
	{
		_tcscpy( m_skinXMLPath, path );
	}

	LPCTSTR CTBCIAMessageBox::GetSkinXMLPath()
	{
		return m_skinXMLPath;
	}

	CPaintManagerUI& CTBCIAMessageBox::GetPaintManager()
	{
		return m_pm;
	}

} // namespace DuiLib 
