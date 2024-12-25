#pragma once


// CMyPicCtrl 对话框

class CMyPicCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CMyPicCtrl)

public:
	CMyPicCtrl(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMyPicCtrl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CGEXPERIMENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
