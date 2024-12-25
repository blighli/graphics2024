// CBSampleDlg.cpp: 实现文件
//

#include "pch.h"
#include "CGExperiment.h"
#include "CBSampleDlg.h"
#include "afxdialogex.h"


// CBSampleDlg 对话框

IMPLEMENT_DYNAMIC(CBSampleDlg, CDialogEx)

CBSampleDlg::CBSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BSample, pParent)
{

}

CBSampleDlg::~CBSampleDlg()
{
}

void CBSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, EDIT);
	DDX_Control(pDX, IDC_BUTTON1, OK);
}


BEGIN_MESSAGE_MAP(CBSampleDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CBSampleDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CBSampleDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBSampleDlg 消息处理程序


void CBSampleDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CBSampleDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	EDIT.GetWindowTextW(str);
	pointsNum = _ttoi(str);
	CDialogEx::OnOK();
}
