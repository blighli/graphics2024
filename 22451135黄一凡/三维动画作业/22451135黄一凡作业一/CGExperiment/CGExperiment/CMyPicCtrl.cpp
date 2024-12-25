// CMyPicCtrl.cpp: 实现文件
//

#include "pch.h"
#include "CGExperiment.h"
#include "CMyPicCtrl.h"
#include "afxdialogex.h"


// CMyPicCtrl 对话框

IMPLEMENT_DYNAMIC(CMyPicCtrl, CDialogEx)

CMyPicCtrl::CMyPicCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CGEXPERIMENT_DIALOG, pParent)
{

}

CMyPicCtrl::~CMyPicCtrl()
{
}

void CMyPicCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyPicCtrl, CDialogEx)
END_MESSAGE_MAP()


// CMyPicCtrl 消息处理程序
