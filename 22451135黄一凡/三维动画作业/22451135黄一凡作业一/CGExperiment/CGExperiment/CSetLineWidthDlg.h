#pragma once


// CSetLineWidthDlg 对话框

class CSetLineWidthDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetLineWidthDlg)

public:
	CSetLineWidthDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetLineWidthDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SetLineWidthDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMReleasedcaptureSliderLinewidth(NMHDR* pNMHDR, LRESULT* pResult);
	int lineWidth;
	CSliderCtrl m_lineWidth;
};
