#pragma once
#ifndef _INCLUDE_CEDITLISTCTRLEX_H_
#define _INCLUDE_CEDITLISTCTRLEX_H_
#include <vector>

using std::vector;


#define EDITLISTCTRLEX_VERSION "editlistctrlex_1.1.1.20170122_beta"


//增加2个接口
/*
主要方便用户判断编辑框内数据是否合法，可手动、自动更新到list中
若要手动更新，则需要在窗口类中接收 MLSM_ITEMCHANGED消息,
并且接收消息时就必须处理并决定是否更新，否则可能出错(删除行或者添加后会出错)
方法步骤:
1. 声明 afx_msg LRESULT OnMsgChangeItem(WPARAM wParam, LPARAM lParam);
2. 实现 LRESULT CxxDlg::OnMsgChangeItem(WPARAM wParam, LPARAM lParam)
{
    int row = wParam & 0xFF;
    int col = wParam>>16;
}

3. 添加消息映射 ON_MESSAGE(MLSM_ITEMCHANGED, &CxxDlg::OnMsgChangeItem)
*/

//增加combox控件嵌入，增加5个接口
/* 可设置combox内容，并按索引顺序存储如：Client;Server;这样一组，则有2个
选项Client/Server,combox 为dropdown模式，不可自己输入，只能选择
内容设置有1个设置接口，2个读取接口

可通过SetComboxCell、SetEditCell来设置某个单元格采用哪种控件输入，当参数
row == -1时，表示某列全部采用该控件输入，col同理
combox 改变消息映射类型为 MLSM_ITEMCHANGEDCOMBOX 想要接收内容改变消息，则
需要添加消息映射 ON_MESSAGE(MLSM_ITEMCHANGEDCOMBOX, &CxxDlg::OnMsgChangeItem)*/


#define IDC_LIST_EDITBOX    0xfff1
#define IDC_LIST_COMBOX     0xfff2
#define MLSM_ITEMCHANGEDEX  (WM_USER + 201)

// CEditListCtrlEx
class CMEdit;
class CMComboBox;
class _declspec(dllexport) CEditListCtrlEx : public CListCtrl
{
    DECLARE_DYNAMIC(CEditListCtrlEx)

public:
    CEditListCtrlEx();
    virtual ~CEditListCtrlEx();

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSelchangeCombox();

    afx_msg LRESULT OnMsgKillFocus(WPARAM wParam, LPARAM lParam);

    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    BOOL DeleteAllItems();
    BOOL DeleteItem(_In_ int nItem);

    //手动更新编辑框值到list
    void UpdateItemText();

    //设置item更新模式，默认为自动更新
    void SetUpdateItemModel(BOOL bAutoUpdate = TRUE);

    //获取待更改的值，手动模式下才能获取到
    CString GetEditText() { return m_edtStr; }

    CString GetCurItemText();

    //index 序号，从0开始,按顺序存储
    int AddComboxContent(LPCTSTR pszContent);    //设置combox内容，以分号分割
    int GetComboxContent(int index, LPTSTR pszContent);
    CString GetComboxContent(int index);

    //设置某个单元格为combox选项, index combox可选内容索引
    //row = -1时表示某列所有， col= -1时,表示某行所有
    int SetComboxCell(int row = 0xffff, int col = 0xffff, int index = 0);
    int SetEditCell(int row = 0xffff, int col = 0xffff);    //设置某个单元格可编辑
    void EraseEditCell(int row = 0xffff, int col = 0xffff);  //删除可编辑单元格，包含（combox）

protected:
    DECLARE_MESSAGE_MAP()

    void SetTextToList();
    void SetContentToBox(CString str);

private:

    int         m_Row;
    int         m_Col;
    int         m_selectRow;
    int         m_selectCol;
    CMEdit*     m_pEditItem;
    CMComboBox* m_pEditCom;

    BOOL        m_bAutoUpdate;  //是否自动更新
    CString     m_edtStr;       //编辑的值

    vector<CString>     m_vecComContent;    //内容
    vector<int>         m_vecEditCell;      //edit单元格
    vector<int>         m_vecComCell;       //com单元格
    vector<int>         m_vecComIndex;      //com单元格对应内容index
};

#endif  //重复编译