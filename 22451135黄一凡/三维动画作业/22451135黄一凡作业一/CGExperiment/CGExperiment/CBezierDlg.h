#pragma once


// CBezierDlg 对话框

class CBezierDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBezierDlg)

public:
	CBezierDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBezierDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Bezier };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton OK;
	CEdit EDIT;
	afx_msg void OnBnClickedOk();
	int n;
	afx_msg void OnEnChangeEdit();
};
