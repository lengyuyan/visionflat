// EditListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "EditListCtrlEx.h"
//#include "comm.h"

#define CUSTOM_MSG   10000

class CMEdit : public CEdit
{
    DECLARE_DYNAMIC(CMEdit)

public:
    CMEdit();
    virtual ~CMEdit();
    void OnKillFocus(CWnd*);
protected:
    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CMEdit, CEdit)

CMEdit::CMEdit() : CEdit()
{
}

CMEdit::~CMEdit()
{
}

BEGIN_MESSAGE_MAP(CMEdit, CEdit)
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CMEdit::OnKillFocus(CWnd* pwnd)
{
    ::PostMessage(GetParent()->m_hWnd, CUSTOM_MSG, 0, 0);
}

class CMComboBox : public CComboBox
{
    DECLARE_DYNAMIC(CMComboBox)

public:
    CMComboBox();
    virtual ~CMComboBox();
    void OnKillFocus(CWnd*);
protected:
    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNAMIC(CMComboBox, CComboBox)

CMComboBox::CMComboBox() : CComboBox()
{
}

CMComboBox::~CMComboBox()
{
}

BEGIN_MESSAGE_MAP(CMComboBox, CComboBox)
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CMComboBox::OnKillFocus(CWnd* pwnd)
{
    ::PostMessage(GetParent()->m_hWnd, CUSTOM_MSG, 0, 0);
}

// CEditListCtrlEx

IMPLEMENT_DYNAMIC(CEditListCtrlEx, CListCtrl)

CEditListCtrlEx::CEditListCtrlEx() : CListCtrl()
{
    m_pEditCom = NULL;
    m_pEditItem = NULL;
    m_bAutoUpdate = TRUE;   //默认自动更新
    m_selectCol = -1;
    m_selectRow = -1;
    m_vecComContent.clear();
    m_vecEditCell.clear();
    m_vecComCell.clear();
    m_vecComIndex.clear();
}

CEditListCtrlEx::~CEditListCtrlEx()
{
    if (m_pEditCom)
    {
        delete m_pEditCom;
    }
    if (m_pEditItem)
    {
        delete m_pEditItem;
    }
}

BEGIN_MESSAGE_MAP(CEditListCtrlEx, CListCtrl)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_MESSAGE(CUSTOM_MSG, &CEditListCtrlEx::OnMsgKillFocus)
    ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CEditListCtrlEx::OnNMDblclk)
    ON_CBN_SELCHANGE(IDC_LIST_COMBOX, &CEditListCtrlEx::OnSelchangeCombox)
END_MESSAGE_MAP()

// CEditListCtrlEx 消息处理程序
void CEditListCtrlEx::SetUpdateItemModel(BOOL bAutoUpdate)
{
    m_bAutoUpdate = bAutoUpdate;
}

LRESULT CEditListCtrlEx::OnMsgKillFocus(WPARAM wParam, LPARAM lParam)
{
    SetTextToList();
    return 0;
}

void CEditListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    SetTextToList();
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    SetTextToList();
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CEditListCtrlEx::DeleteAllItems()
{
    BOOL res = CListCtrl::DeleteAllItems();
    if (res)
    {
        m_vecComContent.clear();
        m_vecComCell.clear();
        m_vecEditCell.clear();
        m_vecComIndex.clear();
    }
    m_selectRow = -1;
    m_selectCol = -1;
    return res;
}

BOOL CEditListCtrlEx::DeleteItem(_In_ int nItem)
{
    EraseEditCell(nItem);
    return CListCtrl::DeleteItem(nItem);
}

void CEditListCtrlEx::UpdateItemText()
{
    if (m_bAutoUpdate)
    {//自动更新状态下，手动无效
        return;
    }

    if (-1 == m_Row || -1 == m_Col)
    {
        return;
    }

    SetItemText(m_Row, m_Col, m_edtStr);
    m_Col = m_Row = -1;
}

void CEditListCtrlEx::SetTextToList()
{
    CString str;
    if(m_pEditItem && m_pEditItem->IsWindowVisible())
    {
        m_pEditItem->ShowWindow(SW_HIDE);
        if(m_Row != -1)
        {
            str = this->GetItemText(m_Row, m_Col);
            m_pEditItem->GetWindowText(m_edtStr);
            if (str == m_edtStr)
            {
                return;
            }
            if (m_bAutoUpdate)
            {
                this->SetItemText(m_Row, m_Col, m_edtStr);
                m_Col = m_Row = -1;
            }
            else
            {
                ::PostMessage(GetParent()->m_hWnd, MLSM_ITEMCHANGEDEX, (WPARAM)MAKELONG(m_Row, m_Col), (LPARAM)this->m_hWnd);
            }
        }
    }
    else if(m_pEditCom && m_pEditCom->IsWindowVisible())
    {
        m_pEditCom->ShowWindow(SW_HIDE);
        if(m_Row != -1)
        {
            str = this->GetItemText(m_Row, m_Col);
            m_pEditCom->GetLBText(m_pEditCom->GetCurSel(), m_edtStr);
            if (str == m_edtStr)
            {
                return;
            }
            if (m_bAutoUpdate)
            {
                this->SetItemText(m_Row, m_Col, m_edtStr);
                m_Col = m_Row = -1;
            }
            else
            {
                ::PostMessage(GetParent()->m_hWnd, MLSM_ITEMCHANGEDEX, (WPARAM)MAKELONG(m_Row, m_Col), (LPARAM)this->m_hWnd);
            }
        }
    }
}

void CEditListCtrlEx::OnSelchangeCombox()
{
    SetTextToList();
}

BOOL CEditListCtrlEx::PreTranslateMessage(MSG* pMsg)
{
    if (WM_KEYDOWN == pMsg->message && (VK_RETURN == pMsg->wParam || VK_TAB == pMsg->wParam))
    {//enter
        SetTextToList();
        return TRUE;
    }
    else if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
    {//esc
        if(m_pEditCom && m_pEditCom->IsWindowVisible())
        {
            m_pEditCom->ShowWindow(SW_HIDE);
        }
        else if(m_pEditItem && m_pEditItem->IsWindowVisible())
        {
            m_pEditItem->ShowWindow(SW_HIDE);
        }
        m_Col = m_Row = -1;
        return TRUE;
    }
    else if (WM_MOUSEWHEEL == pMsg->message)
    {//滚轮
        SetTextToList();
    }
    return CListCtrl::PreTranslateMessage(pMsg);
}

void CEditListCtrlEx::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    CRect mrc;
    RECT rc;
    CString str;
    int row = 0, col = 0;
    vector<int>::iterator it;
    vector<int>::iterator it1;
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    m_Row = pNMListView->iItem;
    m_Col = pNMListView->iSubItem;
    *pResult = 0;

    if (-1 == m_Row || -1 == m_Col)
    {
        return;
    }

    //查找combox
    for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end(); it++, it1++)
    {
        row = (*it>>16) & 0xffff;
        col = *it & 0xffff;
        if ((row == m_Row || 0xffff == row) &&
            (col == m_Col || 0xffff == col))
        {//find ok
            if(!m_pEditCom)
            {
                rc.left = rc.top = 0;
                rc.bottom = 300;
                rc.right = 100;
                m_pEditCom = new CMComboBox;
                m_pEditCom->Create(WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST | CBS_OEMCONVERT, rc, this, IDC_LIST_COMBOX);
                m_pEditCom->SetFont(this->GetFont(), FALSE);//设置字体,不设置这里的话上面的字会很突兀的感觉
                m_pEditCom->SetParent(this);//将list control设置为父窗口,生成的Ccombobox才能正确定位,这个也很重要
            }

            SetContentToBox(m_vecComContent[*it1]);
            GetSubItemRect(m_Row, m_Col, LVIR_BOUNDS, mrc);
            m_pEditCom->MoveWindow(&mrc, TRUE);
            m_pEditCom->ShowWindow(SW_SHOW);
            m_pEditCom->SetFocus();
            //m_pEditCom->ShowDropDown();
            str = this->GetItemText(m_Row, m_Col);
            m_pEditCom->SelectString(-1, str.GetString());
            return;
        }
    }

    //查找edit
    for(it = m_vecEditCell.begin(); it != m_vecEditCell.end(); it++)
    {
        row = (*it>>16) & 0xffff;
        col = *it & 0xffff;
        if ((row == m_Row || 0xffff == row) &&
            (col == m_Col || 0xffff == col))
        {//find ok
            if (!m_pEditItem)
            {
                rc.left = rc.top = 0;
                rc.bottom = 20;
                rc.right = 100;
                m_pEditItem = new CMEdit;
                m_pEditItem->Create(WS_CHILD | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE, rc, this, IDC_LIST_EDITBOX);
                m_pEditItem->SetFont(this->GetFont(), FALSE);
            }

            GetSubItemRect(pNMListView->iItem, pNMListView->iSubItem, LVIR_BOUNDS, mrc);
            m_pEditItem->SetWindowText(this->GetItemText(pNMListView->iItem, pNMListView->iSubItem));
            m_pEditItem->MoveWindow(&mrc, TRUE);
            m_pEditItem->ShowWindow(SW_SHOW);
            m_pEditItem->SetFocus();
            m_pEditItem->SetSel(0, -1);
        }
    }
}

void CEditListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    SetTextToList();
    CListCtrl::OnLButtonDown(nFlags, point);
}

void CEditListCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    SetTextToList();
    CListCtrl::OnRButtonDown(nFlags, point);
}

int CEditListCtrlEx::AddComboxContent(LPCTSTR pszContent)
{
    if (!pszContent)
    {
        return -1;
    }

    m_vecComContent.push_back(pszContent);
    return (int)m_vecComContent.size() - 1;
}

int CEditListCtrlEx::GetComboxContent(int index, LPTSTR pszContent)
{
    if (!pszContent)
    {
        return -1;
    }

    if (index < 0 || index >= (int)m_vecComContent.size())
    {
        return -1;
    }

    //comm_strcpy(pszContent, m_vecComContent[index]);//不要工具
	char* src = pszContent;
	const char* dest = m_vecComContent[index];
	while(*dest && *dest != '/0')
	{
		*src++ = *dest++;
	}
	*src = 0;

    return 0;
}

CString CEditListCtrlEx::GetComboxContent(int index)
{
    if (index < 0 || index >= (int)m_vecComContent.size())
    {
        return "";
    }
    return m_vecComContent[index];
}

int CEditListCtrlEx::SetComboxCell(int row, int col, int index)
{
    int row1 = 0, col1 = 0;
    vector<int>::iterator it;
    vector<int>::iterator it1;

    if (-1 == row)
    {
        row = 0xffff;
    }

    if (-1 == col)
    {
        col = 0xffff;
    }

    if (index < 0 || index >= (int)m_vecComContent.size())
    {
        return -1;
    }

    if (col < 0 || row < 0)
    {
        return -1;
    }

    for(it = m_vecEditCell.begin(); it != m_vecEditCell.end();)
    {//检查edit重复
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if ((col == col1 || 0xffff == col || 0xffff == col1) &&
            (row == row1 || 0xffff == row || 0xffff == row1))
        {//有则删除
            it = m_vecEditCell.erase(it);
            continue;
        }
        it++;
    }

    for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end(); it++, it1++)
    {//检查com重复
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if (col == col1 && row == row1)
        {//有了这个标识
            *it1 = index;
            return 0;
        }
    }

    if (0xffff == row)
    {//所有列
        for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end();)
        {
            if(col == (*it & 0xffff))
            {
                it = m_vecComCell.erase(it);
                it1 = m_vecComIndex.erase(it1);
                continue;
            }
            it++; it1++;
        }
    }

    if (0xffff == col)
    {//所有行
        for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end();)
        {
            if(row == ((*it>>16) & 0xffff))
            {
                it = m_vecComCell.erase(it);
                it1 = m_vecComIndex.erase(it1);
                continue;
            }
            it++; it1++;
        }
    }
    m_vecComCell.push_back(row<<16 | col);
    m_vecComIndex.push_back(index);
    return 0;
}

int CEditListCtrlEx::SetEditCell(int row, int col)
{
    int row1 = 0, col1 = 0;
    vector<int>::iterator it;
    vector<int>::iterator it1;

    if (-1 == row)
    {
        row = 0xffff;
    }

    if (-1 == col)
    {
        col = 0xffff;
    }

    if (col < 0 || row < 0)
    {
        return -1;
    }

    for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end();)
    {//检查com重复
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if ((col == col1 || 0xffff == col || 0xffff == col1) &&
            (row == row1 || 0xffff == row || 0xffff == row1))
        {//重复
            it = m_vecComCell.erase(it);
            it1 = m_vecComIndex.erase(it1);
            continue;
        }
        it++; it1++;
    }

    for(it = m_vecEditCell.begin(); it != m_vecEditCell.end(); it++)
    {//检查edit重复
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if (col == col1 && row == row1)
        {//有了这个标识
            return 0;
        }
    }

    if (0xffff == row)
    {//所有列
        for(it = m_vecEditCell.begin(); it != m_vecEditCell.end();)
        {
            if(col == (*it & 0xffff))
            {
                it = m_vecEditCell.erase(it);
                continue;
            }
            it++;
        }
    }

    if (0xffff == col)
    {//所有行
        for(it = m_vecEditCell.begin(); it != m_vecEditCell.end();)
        {
            if(row == ((*it>>16) & 0xffff))
            {
                it = m_vecEditCell.erase(it);
                continue;
            }
            it++;
        }
    }
    m_vecEditCell.push_back(row<<16 | col);
    return 0;
}

void CEditListCtrlEx::EraseEditCell(int row/* = 0xffff*/, int col/* = 0xffff*/)
{
    int row1 = 0, col1 = 0;
    vector<int>::iterator it;
    vector<int>::iterator it1;

    if (-1 == row)
    {
        row = 0xffff;
    }

    if (-1 == col)
    {
        col = 0xffff;
    }

    if (col < 0 || row < 0)
    {
        return;
    }

    for(it = m_vecComCell.begin(), it1 = m_vecComIndex.begin(); it != m_vecComCell.end();)
    {//检查com
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if ((col == col1 || 0xffff == col || 0xffff == col1) &&
            (row == row1 || 0xffff == row || 0xffff == row1))
        {//重复
            it = m_vecComCell.erase(it);
            it1 = m_vecComIndex.erase(it1);
            continue;
        }
        it++; it1++;
    }

    for(it = m_vecEditCell.begin(); it != m_vecEditCell.end();)
    {//检查edit
        row1 = (*it>>16) & 0xffff;
        col1 = *it & 0xffff;
        if ((col == col1 || 0xffff == col || 0xffff == col1) &&
            (row == row1 || 0xffff == row || 0xffff == row1))
        {//有则删除
            it = m_vecEditCell.erase(it);
            continue;
        }
        it++;
    }
}

void CEditListCtrlEx::SetContentToBox(CString str)
{
    int index = 0;
    char* tp = NULL;
    char* pNextToken = NULL;
    if (!m_pEditCom)
    {
        return;
    }

    m_pEditCom->ResetContent();
    tp = strtok_s(str.GetBuffer(), ";", &pNextToken);
    while(tp)
    {
        m_pEditCom->InsertString(index++, tp);
        tp = strtok_s(NULL, ";", &pNextToken);
    }
}

void CEditListCtrlEx::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if (m_selectRow != pNMListView->iItem)
    {
        m_selectRow = pNMListView->iItem;
    }

    if (m_selectCol != pNMListView->iSubItem)
    {
        m_selectCol = pNMListView->iSubItem;
    }

    *pResult = 0;
}

CString CEditListCtrlEx::GetCurItemText()
{
    if (-1 == m_selectCol || -1 == m_selectRow)
    {
        return "";
    }
    return GetItemText(m_selectRow, m_selectCol);
}