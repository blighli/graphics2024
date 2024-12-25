// CSetLineWidthDlg.cpp: 实现文件
//

#include "pch.h"
#include "CGExperiment.h"
#include "CSetLineWidthDlg.h"
#include "afxdialogex.h"


// CSetLineWidthDlg 对话框

IMPLEMENT_DYNAMIC(CSetLineWidthDlg, CDialogEx)

CSetLineWidthDlg::CSetLineWidthDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SetLineWidthDlg, pParent)
{

}

CSetLineWidthDlg::~CSetLineWidthDlg()
{
}

void CSetLineWidthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_LineWidth, m_lineWidth);
}


BEGIN_MESSAGE_MAP(CSetLineWidthDlg, CDialogEx)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LineWidth, &CSetLineWidthDlg::OnNMReleasedcaptureSliderLinewidth)
END_MESSAGE_MAP()


// CSetLineWidthDlg 消息处理程序


void CSetLineWidthDlg::OnNMReleasedcaptureSliderLinewidth(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_lineWidth.SetRange(0, 10);
	lineWidth = m_lineWidth.GetPos();
	*pResult = 0;
}
