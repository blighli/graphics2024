// CBezierDlg.cpp: 实现文件
//

#include "pch.h"
#include "CGExperiment.h"
#include "CBezierDlg.h"
#include "afxdialogex.h"


// CBezierDlg 对话框

IMPLEMENT_DYNAMIC(CBezierDlg, CDialogEx)

CBezierDlg::CBezierDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Bezier, pParent)
{

}

CBezierDlg::~CBezierDlg()
{
}

void CBezierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, OK);
	DDX_Control(pDX, IDC_EDIT, EDIT);
}


BEGIN_MESSAGE_MAP(CBezierDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CBezierDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CBezierDlg 消息处理程序


void CBezierDlg::OnBnClickedOk()
{
	CString str;
	EDIT.GetWindowTextW(str);
	n = _ttoi(str);
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
