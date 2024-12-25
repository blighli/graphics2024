#pragma once


// CBSampleDlg 对话框

class CBSampleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBSampleDlg)

public:
	CBSampleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBSampleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BSample };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	CEdit EDIT;
	CButton OK;
	int pointsNum;
};
