
// CGExperimentDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CGExperiment.h"
#include "CGExperimentDlg.h"
#include "afxdialogex.h"
#include "CSetLineWidthDlg.h"
#include "CBezierDlg.h"
#include "time.h"
#include<iostream>
#include "CBSampleDlg.h"

#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8
#define PI acos(-1.0)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Edge
{
	int x;
	double step;
	int y_min;
	int y_max;
};

struct CPoint3D
{
	float x;
	float y;
	float z;
	CPoint3D(float x0, float y0, float z0) :
		x(x0), y(y0), z(z0) {
	}
};

typedef struct EdgeNode {
	Edge edge;
	struct EdgeNode* next;
	EdgeNode(Edge x) :
		edge(x), next(NULL) {
	}
};

struct ColorPixel
{
	CPoint point;
	int color;
	ColorPixel() :
		point(CPoint(0,0)), color(0) {
	}
	ColorPixel(CPoint x, int y) :
		point(x), color(y) {
	}
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCGExperimentDlg 对话框



CCGExperimentDlg::CCGExperimentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CGEXPERIMENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCGExperimentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl1);
	DDX_Control(pDX, IDC_STATIC_RECT, m_picCtrl);
	DDX_Control(pDX, IDC_BUTTON1, m_clearBtn);
	DDX_Control(pDX, IDC_SetColor, m_setColor);
}

BEGIN_MESSAGE_MAP(CCGExperimentDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CCGExperimentDlg::OnTvnSelchangedTree1)
	ON_WM_LBUTTONUP()
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RED, &CCGExperimentDlg::OnNMCustomdrawSliderRed)
//ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_RED, &CCGExperimentDlg::OnTRBNThumbPosChangingSliderRed)
//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_RED, &CCGExperimentDlg::OnNMReleasedcaptureSliderRed)
//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GREEN, &CCGExperimentDlg::OnNMReleasedcaptureSliderGreen)
//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BLUE, &CCGExperimentDlg::OnNMReleasedcaptureSliderBlue)
ON_BN_CLICKED(IDC_BUTTON1, &CCGExperimentDlg::OnBnClickedButton1)
//ON_WM_CREATE()
//ON_WM_DESTROY()
ON_BN_CLICKED(IDC_SetColor, &CCGExperimentDlg::OnBnClickedSetcolor)
ON_BN_CLICKED(IDC_SetLineWidth, &CCGExperimentDlg::OnBnClickedSetlinewidth)
ON_BN_CLICKED(IDC_BUTTON3, &CCGExperimentDlg::OnBnClickedButton3)
ON_WM_MOUSEMOVE()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_TIMER()
END_MESSAGE_MAP()


// CCGExperimentDlg 消息处理程序

BOOL CCGExperimentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//选择画什么形状的树状控件
	HTREEITEM graphic = m_treeCtrl1.InsertItem(TEXT("形状"),NULL);
	HTREEITEM line = m_treeCtrl1.InsertItem(TEXT("直线"), graphic);
	HTREEITEM line_DDA = m_treeCtrl1.InsertItem(TEXT("数值微分法"), line);
	HTREEITEM line_MidPoint = m_treeCtrl1.InsertItem(TEXT("中点画线法"), line);
	HTREEITEM line_Bresenham = m_treeCtrl1.InsertItem(TEXT("Bresenham"), line);
	HTREEITEM circle = m_treeCtrl1.InsertItem(TEXT("圆形"), graphic);
	HTREEITEM circle_MidPoint = m_treeCtrl1.InsertItem(TEXT("中点画圆法"), circle);
	HTREEITEM circle_Bresenham = m_treeCtrl1.InsertItem(TEXT("Bresenham"), circle);
	HTREEITEM fill = m_treeCtrl1.InsertItem(TEXT("多边形填充"), graphic);
	HTREEITEM fill_seed = m_treeCtrl1.InsertItem(TEXT("种子填充"), fill);
	HTREEITEM fill_seed_digui = m_treeCtrl1.InsertItem(TEXT("种子填充递归"), fill_seed);
	HTREEITEM fill_seed_stack = m_treeCtrl1.InsertItem(TEXT("种子填充栈"), fill_seed);
	HTREEITEM fill_poly = m_treeCtrl1.InsertItem(TEXT("多边形填充"), fill);
	HTREEITEM name = m_treeCtrl1.InsertItem(TEXT("姓名艺术字"), NULL);
	HTREEITEM gossip = m_treeCtrl1.InsertItem(TEXT("八卦"), NULL);
	HTREEITEM smallYellow = m_treeCtrl1.InsertItem(TEXT("小黄人(扫描线)"), NULL);
	HTREEITEM smallCat = m_treeCtrl1.InsertItem(TEXT("小猫(动态填充)"), NULL);
	HTREEITEM smallFox = m_treeCtrl1.InsertItem(TEXT("小狐狸(动态填充)"), NULL);
	HTREEITEM polygon = m_treeCtrl1.InsertItem(TEXT("多边形绘制"), NULL);
	HTREEITEM pentagon = m_treeCtrl1.InsertItem(TEXT("五边形"), polygon);
	HTREEITEM hexagon = m_treeCtrl1.InsertItem(TEXT("六边形"), polygon);
	HTREEITEM Geometric_transformation = m_treeCtrl1.InsertItem(TEXT("二维几何变换"), NULL);
	HTREEITEM transform = m_treeCtrl1.InsertItem(TEXT("平移变换"), Geometric_transformation);
	HTREEITEM rotate = m_treeCtrl1.InsertItem(TEXT("旋转变换"), Geometric_transformation);
	HTREEITEM scale = m_treeCtrl1.InsertItem(TEXT("缩放变换"), Geometric_transformation);
	HTREEITEM symmetrical_design = m_treeCtrl1.InsertItem(TEXT("对称图案设计"), Geometric_transformation);
	HTREEITEM cutout = m_treeCtrl1.InsertItem(TEXT("裁剪"), NULL);
	HTREEITEM line_cutout = m_treeCtrl1.InsertItem(TEXT("直线段裁剪"), cutout);
	HTREEITEM circle_cutout = m_treeCtrl1.InsertItem(TEXT("圆形裁剪"), cutout);
	HTREEITEM graph3D = m_treeCtrl1.InsertItem(TEXT("三维图形"), NULL);
	HTREEITEM cube3D = m_treeCtrl1.InsertItem(TEXT("长方体"), graph3D);
	HTREEITEM cube3D_transform = m_treeCtrl1.InsertItem(TEXT("平移"), graph3D);
	HTREEITEM cube3D_scale = m_treeCtrl1.InsertItem(TEXT("缩放"), graph3D);
	HTREEITEM cube3D_rotate = m_treeCtrl1.InsertItem(TEXT("旋转"), graph3D);
	HTREEITEM cube3D_three_view_drawing = m_treeCtrl1.InsertItem(TEXT("三视图"), graph3D);
	HTREEITEM curve = m_treeCtrl1.InsertItem(TEXT("曲线"), NULL);
	HTREEITEM bezier = m_treeCtrl1.InsertItem(TEXT("Bezier曲线"), curve);
	HTREEITEM Byangtiao = m_treeCtrl1.InsertItem(TEXT("B样条曲线"), curve);
	HTREEITEM bezier_dynamic = m_treeCtrl1.InsertItem(TEXT("Bezier曲线动态生成"), curve);
	HTREEITEM Byangtiao_dynamic = m_treeCtrl1.InsertItem(TEXT("B样条曲线动态生成"), curve);
	HTREEITEM curved_surface = m_treeCtrl1.InsertItem(TEXT("曲面"), NULL);
	HTREEITEM bezier_curved_surface = m_treeCtrl1.InsertItem(TEXT("Bezier曲面"), curved_surface);
	HTREEITEM Bsp_curved_surface = m_treeCtrl1.InsertItem(TEXT("B样条曲面"), curved_surface);

	//设置默认画直线
	m_treeCtrl1.SelectItem(line);
	//获取画图区的位置
	CWnd* mainWnd = GetDlgItem(IDC_STATIC_RECT);
	mainWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	//获取设备画图工具
	my_pDC = GetWindowDC();
	//颜色初始化
	curColor = RGB(0,0,0);
	//线宽初始化
	curLineWidth = 0;
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCGExperimentDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCGExperimentDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//myPen.CreatePen(PS_SOLID, 3, RGB(curRed, curGreen, curBlue));
		//CClientDC mydc(&m_picCtrl);
		//my_pDC->SelectObject(&myPen);
		//my_pDC->MoveTo(700, 100);
		//my_pDC->LineTo(400, 400);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCGExperimentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CCGExperimentDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_LBUTTONDOWN && GetDlgItem(IDC_STATIC_RECT)->GetSafeHwnd() == pMsg->hwnd)

		OnLButtonDown(MK_LBUTTON, pMsg->pt);   //在此传递点击部位在对话框中的坐标
	else if (pMsg->message == WM_RBUTTONDOWN && GetDlgItem(IDC_STATIC_RECT)->GetSafeHwnd() == pMsg->hwnd)
	{
		OnRButtonDown(MK_RBUTTON, pMsg->pt);
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'X':
			xFlag = true;
			break;
		case 'Y':
			yFlag = true;
			break;
		case 'Z':
			zFlag = true;
			break;
		default:
			break;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
		case 'X':
			xFlag = false;
			break;
		case 'Y':
			yFlag = false;
			break;
		case 'Z':
			zFlag = false;
			break;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CCGExperimentDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((point.x >= rect.left && point.x <= rect.right) && (point.y >= rect.top && point.y <= rect.bottom))
	{

		//  通过对point的处理，获得实际在picture控件中的点击位置（坐标），完成。
		//point.x -= rect.left;
		//point.y -= rect.top;
		//point.x -= 100;
		point.y += 38;
		point.x += 8;

		CString str = cs_parent+cs;
		if (str == "直线数值微分法"||"直线中点画线法"||"直线Bresenham"|| "圆形中点画圆法" || "圆形Bresenham")
			point_Down = point;
		if (str == "多边形绘制五边形")
			clickedFlag = 1;
		if (str == "多边形绘制六边形")
			clickedFlag = 1;
		if ((cs == "平移变换"|| cs == "旋转变换") && point.x>left && point.x<right && point.y>top && point.y<bottom)
		{
			lastPoint = point;
			flag = 1;
		}
		if (cs == "缩放变换")
		{
			lastPoint = point;
			xyFlag = 1;
		}
		if (cs == "直线段裁剪")
			lastPoint = point;
		if (cs == "圆形裁剪")
			lastPoint = point;
		if (cs == "平移" && point.x>left3D && point.x<right3D && point.y>top3D && point.y<bottom3D)
		{
			lastPoint3D = point;
			flag3D = 1;
		}
		if (cs_parent + cs == "三维图形缩放")
			lastPoint3D = point;
		if (cs_parent + cs == "三维图形旋转")
			lastPoint3D = point;
		if (cs_parent + cs == "曲线Bezier曲线")
			clickedFlag = 1;
		if (cs_parent + cs == "曲线B样条曲线")
			clickedFlag = 1;
		if (cs_parent + cs == "曲面Bezier曲面")
			clickedFlag = 1;
		if (cs_parent + cs == "曲面B样条曲面")
			clickedFlag = 1;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CCGExperimentDlg::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	HTREEITEM selItem, selItem_parent, selItem_grandparent;
	//获得选择项
	selItem = m_treeCtrl1.GetSelectedItem();
	//获取选中的内容
	cs = m_treeCtrl1.GetItemText(selItem);
	//获取选中内容的父节点
	selItem_parent = m_treeCtrl1.GetParentItem(selItem);
	//获取父节点的内容
	cs_parent = m_treeCtrl1.GetItemText(selItem_parent);
	selItem_grandparent = m_treeCtrl1.GetParentItem(selItem_parent);
	cs_grandparent = m_treeCtrl1.GetItemText(selItem_grandparent);
	if (cs == "八卦")
	{
		DrawGossip();
	}
	else if (cs == "小黄人(扫描线)")
	{
		SmallYellowHuamn();
	}
	else if (cs == "小猫(动态填充)")
	{
		SmallCat();
	}
	else if (cs == "小狐狸(动态填充)")
	{
		SmallFox();
	}
	else if (cs=="姓名艺术字")
	{
		MySpecialName();
	}
	else if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充递归")
	{
		points = { {400,150},{420,150},{440,170},{440,190},{400,190} };
		int pointsNum = points.size();
		for (size_t i = 0; i < pointsNum - 1; i++)
		{
			my_pDC->MoveTo(points[i].x, points[i].y);
			my_pDC->LineTo(points[i + 1].x, points[i + 1].y);
		}
		my_pDC->MoveTo(points[pointsNum - 1].x, points[pointsNum - 1].y);
		my_pDC->LineTo(points[0].x, points[0].y);
	}
	else if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充栈")
	{
		points = { {400,150},{420,150},{440,170},{440,190},{400,190} };
		int pointsNum = points.size();
		for (size_t i = 0; i < pointsNum - 1; i++)
		{
			my_pDC->MoveTo(points[i].x, points[i].y);
			my_pDC->LineTo(points[i + 1].x, points[i + 1].y);
		}
		my_pDC->MoveTo(points[pointsNum - 1].x, points[pointsNum - 1].y);
		my_pDC->LineTo(points[0].x, points[0].y);
	}
	else if (cs_parent + cs == "多边形填充多边形填充")
	{
		points = { {400,200},{550,150},{850,250},{850,500},{550,350},
			{400,426} };
		Polygonfill(points, curColor);
	}
	else if (cs_parent + cs == "多边形绘制五边形")
	{
		my_pDC->Polygon(fivePoints, 5);
	}
	else if (cs_parent + cs == "多边形绘制六边形")
	{
		my_pDC->Polygon(sixPoints, 6);
	}
	else if (cs_parent + cs == "二维几何变换平移变换")
	{
		//T2d[0][0] = 1;
		//T2d[1][1] = 1;
		//T2d[2][2] = 1;
		CPen penRed(PS_SOLID, 3, RGB(255, 0, 0));
		CPen* pOldPen = my_pDC->SelectObject(&penRed);

		myStartPoints[0] = { 600,170 };
		myStartPoints[1] = { 700,500 };
		myStartPoints[2] = { 425,290 };
		myStartPoints[3] = { 775,290 };
		myStartPoints[4] = { 500,500 };
		myStartPoints[5] = { 600,170 };
		my_pDC->Polyline(myStartPoints, 6);
		left = 530, right = 670, top = 290, bottom = 390;
	}
	else if (cs_parent + cs == "二维几何变换旋转变换")
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				T2d[i][j] == 0;
			}
			pos[i] = 0;
		}
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
		CPen penRed(PS_SOLID, 3, RGB(255, 0, 0));
		CPen* pOldPen = my_pDC->SelectObject(&penRed);

		myStartPoints[0] = { 600,170 };
		myStartPoints[1] = { 700,500 };
		myStartPoints[2] = { 425,290 };
		myStartPoints[3] = { 775,290 };
		myStartPoints[4] = { 500,500 };
		myStartPoints[5] = { 600,170 };
		my_pDC->Polyline(myStartPoints, 6);
		left = 425, right = 775, top = 170, bottom = 500;
	}
	else if (cs_parent + cs == "二维几何变换缩放变换")
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				T2d[i][j] == 0;
			}
			pos[i] = 0;
		}
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
		CPen penRed(PS_SOLID, 3, RGB(255, 0, 0));
		CPen* pOldPen = my_pDC->SelectObject(&penRed);

		myStartPoints[0] = { 600,170 };
		myStartPoints[1] = { 700,500 };
		myStartPoints[2] = { 425,290 };
		myStartPoints[3] = { 775,290 };
		myStartPoints[4] = { 500,500 };
		myStartPoints[5] = { 600,170 };
		my_pDC->Polyline(myStartPoints, 6);
	}
	else if (cs_parent + cs == "裁剪直线段裁剪")
	{
		CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
		CPen* pOldblue = my_pDC->SelectObject(&penBlue);
		my_pDC->MoveTo(500,500);
		my_pDC->LineTo(800,200);
		my_pDC->SelectObject(pOldblue);
	}
	else if (cs_parent + cs == "裁剪圆形裁剪")
	{
		CPen penBlue(PS_SOLID, 1, RGB(0, 0, 255));
		CPen* pOldblue = my_pDC->SelectObject(&penBlue);
		my_pDC->MoveTo(500, 500);
		my_pDC->LineTo(800, 200);
		my_pDC->SelectObject(pOldblue);
	}
	else if (cs_parent + cs == "二维几何变换对称图案设计")
	{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			T2d[i][j] = 0;
		}
	}
		T2d[0][0] = -1;
		T2d[1][1] = 1;
		T2d[2][2] = 1;
		T2d[2][0] = 2 * 600;
		
		CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
		CPen* pOldPen = my_pDC->SelectObject(&penRed);
		CBrush* brushRed = new CBrush(RGB(255, 0, 0));
		CBrush* pOldBrush = my_pDC->SelectObject(brushRed);
		CPoint temppoints[6];
		//********************1******************
		temppoints[0] = { 600,170 };
		temppoints[1] = { 627,190 };
		temppoints[2] = { 627,218 };
		temppoints[3] = { 600,238 };
		temppoints[4] = { 573,218 };
		temppoints[5] = { 573,190 };
		my_pDC->Polygon(temppoints, 6);
		//********************2******************对称
		temppoints[0] = { 570,220 };
		temppoints[1] = { 597,240 };
		temppoints[2] = { 597,268 };
		temppoints[3] = { 570,288 };
		temppoints[4] = { 543,268 };
		temppoints[5] = { 543,240 };
		my_pDC->Polygon(temppoints, 6);
		Symmetrical(T2d, temppoints);
		//对称来咯
		/*for (size_t i = 0; i < 6; i++)
		{
			temp[0] = temppoints[i].x;
			temp[1] = temppoints[i].y;
			temppoints[i].x = MyMatrixMultiply(temp, T2d)[0];
			temppoints[i].y = MyMatrixMultiply(temp, T2d)[1];
		}
		my_pDC->Polygon(temppoints, 6);*/
		//********************3******************
		temppoints[0] = { 600,270 };
		temppoints[1] = { 627,290 };
		temppoints[2] = { 627,318 };
		temppoints[3] = { 600,338 };
		temppoints[4] = { 573,318 };
		temppoints[5] = { 573,290 };
		my_pDC->Polygon(temppoints, 6);
		//********************4******************对称
		temppoints[0] = { 570,320 };
		temppoints[1] = { 597,340 };
		temppoints[2] = { 597,368 };
		temppoints[3] = { 570,388 };
		temppoints[4] = { 543,368 };
		temppoints[5] = { 543,340 };
		my_pDC->Polygon(temppoints, 6);
		Symmetrical(T2d, temppoints);
		//********************5******************对称
		temppoints[0] = { 540,370 };
		temppoints[1] = { 567,390 };
		temppoints[2] = { 567,418 };
		temppoints[3] = { 540,438 };
		temppoints[4] = { 513,418 };
		temppoints[5] = { 513,390 };
		my_pDC->Polygon(temppoints, 6);
		Symmetrical(T2d, temppoints);
		//********************6******************对称
		temppoints[0] = { 512,220 };
		temppoints[1] = { 539,240 };
		temppoints[2] = { 539,268 };
		temppoints[3] = { 512,288 };
		temppoints[4] = { 485,268 };
		temppoints[5] = { 485,240 };
		my_pDC->Polygon(temppoints, 6);
		Symmetrical(T2d, temppoints);
		//********************7******************对称
		temppoints[0] = { 482,270 };
		temppoints[1] = { 509,290 };
		temppoints[2] = { 509,318 };
		temppoints[3] = { 480,338 };
		temppoints[4] = { 455,318 };
		temppoints[5] = { 455,290 };
		my_pDC->Polygon(temppoints, 6);
		Symmetrical(T2d, temppoints);
	}
	else if (cs_parent + cs == "三维图形长方体")
	{
		CPoint3D cubePoint0 = CPoint3D(100.0, 100.0, 100.0);
		CPoint3D cubePoint1 = CPoint3D(100.0, 100.0, 200.0);
		CPoint3D cubePoint2 = CPoint3D(300.0, 100.0, 200.0);
		CPoint3D cubePoint3 = CPoint3D(300.0, 100.0, 100.0);
		CPoint3D cubePoint4 = CPoint3D(100.0, 200.0, 100.0);
		CPoint3D cubePoint5 = CPoint3D(100.0, 200.0, 200.0);
		CPoint3D cubePoint6 = CPoint3D(300.0, 200.0, 200.0);
		CPoint3D cubePoint7 = CPoint3D(300.0, 200.0, 100.0);

		float cubeCoord[8][4] =
		{
			{cubePoint0.x,cubePoint0.y,cubePoint0.z,1.0},
			{cubePoint1.x,cubePoint1.y,cubePoint1.z,1.0},
			{cubePoint2.x,cubePoint2.y,cubePoint2.z,1.0},
			{cubePoint3.x,cubePoint3.y,cubePoint3.z,1.0},
			{cubePoint4.x,cubePoint4.y,cubePoint4.z,1.0},
			{cubePoint5.x,cubePoint5.y,cubePoint5.z,1.0},
			{cubePoint6.x,cubePoint6.y,cubePoint6.z,1.0},
			{cubePoint7.x,cubePoint7.y,cubePoint7.z,1.0},
		};
		float isometricProjection[4][4] =
		{
			{0.707,0,-0.408,0},{-0.707,0,-0.408,0},
			{0,0,0.816,0},{0,0,0,1}
		};
		finalCubeCoord = new int[8][4];
		finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				finalCubeCoord[i][j] += 400;
			}

		}
		DrawCube(finalCubeCoord);
	}
	else if (cs_parent + cs == "三维图形缩放")
	{
		CPoint3D cubePoint0 = CPoint3D(100.0, 100.0, 100.0);
		CPoint3D cubePoint1 = CPoint3D(100.0, 100.0, 200.0);
		CPoint3D cubePoint2 = CPoint3D(300.0, 100.0, 200.0);
		CPoint3D cubePoint3 = CPoint3D(300.0, 100.0, 100.0);
		CPoint3D cubePoint4 = CPoint3D(100.0, 200.0, 100.0);
		CPoint3D cubePoint5 = CPoint3D(100.0, 200.0, 200.0);
		CPoint3D cubePoint6 = CPoint3D(300.0, 200.0, 200.0);
		CPoint3D cubePoint7 = CPoint3D(300.0, 200.0, 100.0);

		cubeCoord[0][0] = cubePoint0.x;
		cubeCoord[0][1] = cubePoint0.y;
		cubeCoord[0][2] = cubePoint0.z;
		cubeCoord[0][3] = 1.0;
		cubeCoord[1][0] = cubePoint1.x;
		cubeCoord[1][1] = cubePoint1.y;
		cubeCoord[1][2] = cubePoint1.z;
		cubeCoord[1][3] = 1.0;
		cubeCoord[2][0] = cubePoint2.x;
		cubeCoord[2][1] = cubePoint2.y;
		cubeCoord[2][2] = cubePoint2.z;
		cubeCoord[2][3] = 1.0;
		cubeCoord[3][0] = cubePoint3.x;
		cubeCoord[3][1] = cubePoint3.y;
		cubeCoord[3][2] = cubePoint3.z;
		cubeCoord[3][3] = 1.0;
		cubeCoord[4][0] = cubePoint4.x;
		cubeCoord[4][1] = cubePoint4.y;
		cubeCoord[4][2] = cubePoint4.z;
		cubeCoord[4][3] = 1.0;
		cubeCoord[5][0] = cubePoint5.x;
		cubeCoord[5][1] = cubePoint5.y;
		cubeCoord[5][2] = cubePoint5.z;
		cubeCoord[5][3] = 1.0;
		cubeCoord[6][0] = cubePoint6.x;
		cubeCoord[6][1] = cubePoint6.y;
		cubeCoord[6][2] = cubePoint6.z;
		cubeCoord[6][3] = 1.0;
		cubeCoord[7][0] = cubePoint7.x;
		cubeCoord[7][1] = cubePoint7.y;
		cubeCoord[7][2] = cubePoint7.z;
		cubeCoord[7][3] = 1.0;

		//float isometricProjection[4][4] =
		//{
		//	{0.707,0,-0.408,0},{-0.707,0,-0.408,0},
		//	{0,0,0.816,0},{0,0,0,1}
		//};
		isometricProjection[0][0] = 0.707;
		isometricProjection[0][1] = 0;
		isometricProjection[0][2] = -0.408;
		isometricProjection[0][3] = 0;
		isometricProjection[1][0] = -0.707;
		isometricProjection[1][1] = 0;
		isometricProjection[1][2] = -0.408;
		isometricProjection[1][3] = 0;
		isometricProjection[2][0] = 0;
		isometricProjection[2][1] = 0;
		isometricProjection[2][2] = 0.816;
		isometricProjection[2][3] = 0;
		isometricProjection[3][0] = 0;
		isometricProjection[3][1] = 0;
		isometricProjection[3][2] = 0;
		isometricProjection[3][3] = 1;
		finalCubeCoord = new int[8][4];
		finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (j == 0)
				{
					finalCubeCoord[i][j] += 500;
				}
				else
				{
					finalCubeCoord[i][j] += 300;
				}
			}
		}
		DrawCube(finalCubeCoord);
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (i == j)
				{
					T3d[i][j] = 1.0;
				}
				else
				{
					T3d[i][j] = 0.0;
				}
			}
		}
	}
	else if (cs_parent + cs == "三维图形旋转")
	{
		CPoint3D cubePoint0 = CPoint3D(100.0, 100.0, 100.0);
		CPoint3D cubePoint1 = CPoint3D(100.0, 100.0, 200.0);
		CPoint3D cubePoint2 = CPoint3D(300.0, 100.0, 200.0);
		CPoint3D cubePoint3 = CPoint3D(300.0, 100.0, 100.0);
		CPoint3D cubePoint4 = CPoint3D(100.0, 200.0, 100.0);
		CPoint3D cubePoint5 = CPoint3D(100.0, 200.0, 200.0);
		CPoint3D cubePoint6 = CPoint3D(300.0, 200.0, 200.0);
		CPoint3D cubePoint7 = CPoint3D(300.0, 200.0, 100.0);

		cubeCoord[0][0] = cubePoint0.x;
		cubeCoord[0][1] = cubePoint0.y;
		cubeCoord[0][2] = cubePoint0.z;
		cubeCoord[0][3] = 1.0;
		cubeCoord[1][0] = cubePoint1.x;
		cubeCoord[1][1] = cubePoint1.y;
		cubeCoord[1][2] = cubePoint1.z;
		cubeCoord[1][3] = 1.0;
		cubeCoord[2][0] = cubePoint2.x;
		cubeCoord[2][1] = cubePoint2.y;
		cubeCoord[2][2] = cubePoint2.z;
		cubeCoord[2][3] = 1.0;
		cubeCoord[3][0] = cubePoint3.x;
		cubeCoord[3][1] = cubePoint3.y;
		cubeCoord[3][2] = cubePoint3.z;
		cubeCoord[3][3] = 1.0;
		cubeCoord[4][0] = cubePoint4.x;
		cubeCoord[4][1] = cubePoint4.y;
		cubeCoord[4][2] = cubePoint4.z;
		cubeCoord[4][3] = 1.0;
		cubeCoord[5][0] = cubePoint5.x;
		cubeCoord[5][1] = cubePoint5.y;
		cubeCoord[5][2] = cubePoint5.z;
		cubeCoord[5][3] = 1.0;
		cubeCoord[6][0] = cubePoint6.x;
		cubeCoord[6][1] = cubePoint6.y;
		cubeCoord[6][2] = cubePoint6.z;
		cubeCoord[6][3] = 1.0;
		cubeCoord[7][0] = cubePoint7.x;
		cubeCoord[7][1] = cubePoint7.y;
		cubeCoord[7][2] = cubePoint7.z;
		cubeCoord[7][3] = 1.0;

		isometricProjection[0][0] = 0.707;
		isometricProjection[0][1] = 0;
		isometricProjection[0][2] = -0.408;
		isometricProjection[0][3] = 0;
		isometricProjection[1][0] = -0.707;
		isometricProjection[1][1] = 0;
		isometricProjection[1][2] = -0.408;
		isometricProjection[1][3] = 0;
		isometricProjection[2][0] = 0;
		isometricProjection[2][1] = 0;
		isometricProjection[2][2] = 0.816;
		isometricProjection[2][3] = 0;
		isometricProjection[3][0] = 0;
		isometricProjection[3][1] = 0;
		isometricProjection[3][2] = 0;
		isometricProjection[3][3] = 1;
		finalCubeCoord = new int[8][4];
		finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (j == 0)
				{
					finalCubeCoord[i][j] += 500;
				}
				else
				{
					finalCubeCoord[i][j] += 300;
				}
			}
		}
		DrawCube(finalCubeCoord);
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				T3d[i][j] = 0.0;
			}
		}
	}
	else if (cs_parent + cs == "三维图形三视图")
	{
		myCoord[0][0] = 0; myCoord[0][1] = 0; myCoord[0][2] = 0; myCoord[0][3] = 1;
		myCoord[1][0] = 0; myCoord[1][1] = 0; myCoord[1][2] = 100; myCoord[1][3] = 1;
		myCoord[2][0] = 100; myCoord[2][1] = 0; myCoord[2][2] = 100; myCoord[2][3] = 1;
		myCoord[3][0] = 100; myCoord[3][1] = 0; myCoord[3][2] = 0; myCoord[3][3] = 1;
		myCoord[4][0] = 0; myCoord[4][1] = 150; myCoord[4][2] = 0; myCoord[4][3] = 1;
		myCoord[5][0] = 0; myCoord[5][1] = 150; myCoord[5][2] = 20; myCoord[5][3] = 1;
		myCoord[6][0] = 100; myCoord[6][1] = 150; myCoord[6][2] = 20; myCoord[6][3] = 1;
		myCoord[7][0] = 100; myCoord[7][1] = 150; myCoord[7][2] = 0; myCoord[7][3] = 1;
		myCoord[8][0] = 0; myCoord[8][1] = 100; myCoord[8][2] = 20; myCoord[8][3] = 1;
		myCoord[9][0] = 0; myCoord[9][1] =30; myCoord[9][2] = 100; myCoord[9][3] = 1;
		myCoord[10][0] = 100; myCoord[10][1] = 30; myCoord[10][2] = 100; myCoord[10][3] = 1;
		myCoord[11][0] = 100; myCoord[11][1] = 100; myCoord[11][2] = 20; myCoord[11][3] = 1;
		/* **********************轴侧图************************** */
		float theta_x = (125 * PI) / 180;
		float theta_y = (45 * PI) / 180;
		isometricProjection[0][0] = cos(theta_y); isometricProjection[0][1] = sin(theta_x)*sin(theta_y); isometricProjection[0][2] = 0; isometricProjection[0][3] = 0;
		isometricProjection[1][0] = 0; isometricProjection[1][1] = cos(theta_x); isometricProjection[1][2] = 0; isometricProjection[1][3] = 0;
		isometricProjection[2][0] = sin(theta_y); isometricProjection[2][1] = -sin(theta_x)*cos(theta_y); isometricProjection[2][2] = 0; isometricProjection[2][3] = 0;
		isometricProjection[3][0] = 0; isometricProjection[3][1] = 0; isometricProjection[3][2] = 0; isometricProjection[3][3] = 1;
		finalMyCoord = new int[12][4];
		finalMyCoord = MyMatrixMultiply3(myCoord, isometricProjection);
		for (size_t i = 0; i < 12; i++)
		{
			finalMyCoord[i][0] += 500;
			finalMyCoord[i][1] += 300;
		}
		Draw3DDrawing(finalMyCoord);
		/* **********************正视图************************** */
		theta_x = (180 * PI) / 180;
		theta_y = (0 * PI) / 180;
		isometricProjection[0][0] = cos(theta_y); isometricProjection[0][1] = sin(theta_x) * sin(theta_y); isometricProjection[0][2] = 0; isometricProjection[0][3] = 0;
		isometricProjection[1][0] = 0; isometricProjection[1][1] = cos(theta_x); isometricProjection[1][2] = 0; isometricProjection[1][3] = 0;
		isometricProjection[2][0] = sin(theta_y); isometricProjection[2][1] = -sin(theta_x) * cos(theta_y); isometricProjection[2][2] = 0; isometricProjection[2][3] = 0;
		isometricProjection[3][0] = 0; isometricProjection[3][1] = 0; isometricProjection[3][2] = 0; isometricProjection[3][3] = 1;
		finalMyCoord = new int[12][4];
		finalMyCoord = MyMatrixMultiply3(myCoord, isometricProjection);
		for (size_t i = 0; i < 12; i++)
		{
			finalMyCoord[i][0] += 750;
			finalMyCoord[i][1] += 250;
		}
		Draw3DDrawing(finalMyCoord);
		/* **********************侧视图************************** */
		theta_x = (180 * PI) / 180;
		theta_y = (90 * PI) / 180;
		isometricProjection[0][0] = cos(theta_y); isometricProjection[0][1] = sin(theta_x) * sin(theta_y); isometricProjection[0][2] = 0; isometricProjection[0][3] = 0;
		isometricProjection[1][0] = 0; isometricProjection[1][1] = cos(theta_x); isometricProjection[1][2] = 0; isometricProjection[1][3] = 0;
		isometricProjection[2][0] = sin(theta_y); isometricProjection[2][1] = -sin(theta_x) * cos(theta_y); isometricProjection[2][2] = 0; isometricProjection[2][3] = 0;
		isometricProjection[3][0] = 0; isometricProjection[3][1] = 0; isometricProjection[3][2] = 0; isometricProjection[3][3] = 1;
		finalMyCoord = new int[12][4];
		finalMyCoord = MyMatrixMultiply3(myCoord, isometricProjection);
		for (size_t i = 0; i < 12; i++)
		{
			finalMyCoord[i][0] += 750;
			finalMyCoord[i][1] += 430;
		}
		Draw3DDrawing(finalMyCoord);
	}
	else if (cs_parent + cs == "曲线Bezier曲线")
	{
		CBezierDlg dlg;
		if (dlg.DoModal()==IDOK)
		{
			n = dlg.n;
		}
		ctrlPoints = new CPoint[n+1];
		CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
		my_pDC->SelectObject(&penRed);
		for (size_t i = 0; i < n+1; i++)
		{
			ctrlPoints[i].x = 300 + 50 * i;
			ctrlPoints[i].y = 300;
			my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
			my_pDC->LineTo(ctrlPoints[i].x, ctrlPoints[i].y);
		}
		CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
		my_pDC->SelectObject(&penBlue);
		for (size_t i = 0; i < n; i++)
		{
			my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
			my_pDC->LineTo(ctrlPoints[i+1].x, ctrlPoints[i+1].y);
		}
	}
	else if (cs_parent + cs == "曲线Bezier曲线动态生成")
	{
		SetTimer(1, 10, NULL);
	}
	else if (cs_parent + cs == "曲线B样条曲线")
	{
		CBSampleDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			pointsNum = dlg.pointsNum;
		}
		ctrlPoints = new CPoint[pointsNum];
		CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
		my_pDC->SelectObject(&penRed);
		for (size_t i = 0; i < pointsNum; i++)
		{
			ctrlPoints[i].x = 300 + 50 * i;
			ctrlPoints[i].y = 300;
			my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
			my_pDC->LineTo(ctrlPoints[i].x, ctrlPoints[i].y);
		}
		CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
		my_pDC->SelectObject(&penBlue);
		for (size_t i = 0; i < pointsNum - 1; i++)
		{
			my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
			my_pDC->LineTo(ctrlPoints[i + 1].x, ctrlPoints[i + 1].y);
		}
	}
	else if (cs_parent + cs == "曲线B样条曲线动态生成")
	{
		int nodeVector[12] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5 };
		float basis_i[1000], U[1000];
		U[0] = 0.0;
		for (size_t i = 1; i < 1000; i++)
			U[i] = U[i - 1] + 5.0/999;
		for (size_t i = 0; i < 1000; i++)
		{
			bsp[i].x = 0;
			bsp[i].y = 0;
		}
		for (size_t i = 0; i < 8; i++)
		{
			int j = 0;
			for (size_t m = 0; m < 1000; m++)
			{
				basis_i[j] = bSampleBasis(i, 3, U[m]);
				j += 1;
			}
			for (size_t m = 0; m < 1000; m++)
			{
				bsp[m].x = bsp[m].x + prefabPoints[i].x * basis_i[m];
				bsp[m].y = bsp[m].y + prefabPoints[i].y * basis_i[m];
			}
		}
		SetTimer(2, 10, NULL);
	}
	else if (cs_parent + cs == "曲面Bezier曲面")
	{
		CPen penRed(PS_SOLID, 8, RGB(255, 0, 0));
		my_pDC->SelectObject(&penRed);
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				P[i][j] = CPoint(400 + 100 * i, 150 + 100 * j);
				my_pDC->MoveTo(P[i][j].x, P[i][j].y);
				my_pDC->LineTo(P[i][j].x, P[i][j].y);
			}
		}
		DrawBezierCurvedSurface(P);
	}
	else if (cs_parent + cs == "曲面B样条曲面")
	{
		CPen penRed(PS_SOLID, 8, RGB(255, 0, 0));
		my_pDC->SelectObject(&penRed);
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				P[i][j] = CPoint(400 + 100 * i, 150 + 100 * j);
				my_pDC->MoveTo(P[i][j].x, P[i][j].y);
				my_pDC->LineTo(P[i][j].x, P[i][j].y);
			}
		}
		DrawBspCurvedSurface(P);
	}
}

void CCGExperimentDlg::DrawLine1(CPoint point1, CPoint point2, int color)
{
	float dx, dy, x, y, k, b;
	dx = point2.x - point1.x;
	dy = point2.y - point1.y;
	k = dy / dx;
	if (abs(k)<=1)
	{
		y = point1.y;
		for (x = point1.x; x <= point2.x; x++) {
			my_pDC->SetPixel(x, int(y + 0.5), color);
			y += k;
		}
	}
	else if (k>1)
	{
		x = point1.x;
		for (y = point1.y ; y <= point2.y; y++)
		{
			my_pDC->SetPixel(int(x + 0.5), y, color);
			x += 1.0 / k;
		}
	}
	else if (k < -1)
	{
		x = point1.x;
		for ( y = point1.y ; y >= point2.y; y--)
		{
			my_pDC->SetPixel(int(x + 0.5), y, color);
			x -= 1.0 / k;
		}
	}
	
}

void CCGExperimentDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if ((point.x >= rect.left && point.x <= rect.right) && (point.y >= rect.top && point.y <= rect.bottom))
	{

		//  通过对point的处理，获得实际在picture控件中的点击位置（坐标），完成。
		//point.x -= rect.left;
		//point.y -= rect.top;
		point.y += 38;
		point.x += 8;

		CString str = cs_parent + cs;
		point_Up = point;
		if (str == "直线数值微分法" && curLineWidth == 0)
		{
			if (point_Up.x > point_Down.x)
				DrawLine1(point_Down, point_Up, curColor);
			else
			{
				DrawLine1(point_Up, point_Down, curColor);
			}
		}
		else if (str == "直线中点画线法" && curLineWidth == 0)
		{
			if (point_Down.x<point_Up.x&&point_Down.y<point_Up.y)
			{
				DrawLine2(point_Down, point_Up, curColor);
			}
			else
			{
				DrawLine2(point_Up, point_Down, curColor);
			}
		}
		else if (str == "直线Bresenham" && curLineWidth == 0)
		{
			if (point_Up.x > point_Down.x)
			{
				DrawLine3(point_Down, point_Up, curColor);
			}
			else
			{
				DrawLine3(point_Up, point_Down, curColor);
			}
		}
		else if (cs_parent == "直线" &&curLineWidth != 0)
		{
			DrawLineByCPen(point_Down, point_Up, curColor, curLineWidth);
		}
		else if (cs_parent == "圆形")
		{
			int r;
			r = (int)sqrt((point_Down.x - point_Up.x)* (point_Down.x - point_Up.x) + (point_Down.y - point_Up.y)* (point_Down.y - point_Up.y));
			if (cs == "中点画圆法" && curLineWidth == 0)
			{
				//CString str;
				//str.Format(TEXT("x1 = %d, y1 = %d,x2 = %d, y2 = %d, radius = %d"), point_Down.x, point_Down.y, point_Up.x, point_Up.y, r);
				//MessageBox(str);
				DrawCircle1(point_Down, r, curColor);
			}
			else if(cs == "Bresenham" && curLineWidth == 0)
			{
				DrawCircle2(point_Down, r, curColor);
			}
			else
			{
				DrawCircleByCPen(point_Down, r, curColor, curLineWidth);
			}
		}
		else if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充递归")
		{
			//seedPoint = CPoint(420,170);
			seedPoint = point;
			double startTime, endTime, costTime;
			startTime = clock();
			SeedFill_digui(seedPoint, my_pDC->GetPixel(seedPoint), curColor);
			endTime = clock();
			costTime = (endTime - startTime) / 1000;
			CString tip;
			tip.Format(TEXT("时间为%f"), costTime);
			MessageBox(tip);
		}
		else if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充栈")
		{
			//seedPoint = CPoint(420, 170);
			seedPoint = point;
			int oldColor = my_pDC->GetPixel(seedPoint);
			SeedFill_stack(seedPoint, oldColor, curColor);
		}
		if (cs == "五边形")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
		if (cs == "六边形")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
		if (cs == "平移变换")
		{
			flag = 0;
		}
		if (cs == "平移")
		{
			flag3D = 0;
		}
		if (cs == "旋转变换")
		{
			float theta1 = atan(float(lastPoint.y) / lastPoint.x);
			float theta2 = atan(float(point.y) / point.x);
			float theta = theta2 - theta1;
			T2d[0][0] = cos(theta);
			T2d[0][1] = sin(theta);
			T2d[1][0] = -sin(theta);
			T2d[1][1] = T2d[0][0];
			T2d[2][0] = (1- T2d[0][0])*600+335* T2d[0][1];
			T2d[2][1] = (1- T2d[0][0])*335-600* T2d[0][1];
			T2d[2][2] = 1;
			lastPoint = point;
			for (size_t i = 0; i < 6; i++)
			{
				pos[0] = myStartPoints[i].x;
				pos[1] = myStartPoints[i].y;
				pos[2] = 1;
				int* result = MyMatrixMultiply(pos,T2d);
				myStartPoints[i].x = result[0];
				myStartPoints[i].y = result[1];
			}
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			my_pDC->Polyline(myStartPoints, 6);
			//flag = 0;
		}
		if (cs == "缩放变换")
		{
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					T2d[i][j] = 0;
				}
			}
			if (point.x > lastPoint.x)
				{
				T2d[0][0] = float(point.x - lastPoint.x) / 1000+1.0;
				T2d[1][1] = 1;
			}
			else
				{
				T2d[0][0] = 1 - float(lastPoint.x - point.x) / 1000;
				T2d[1][1] = 1;
			}
			T2d[2][2] = 1;
			T2d[2][0] = (1 - T2d[0][0]) * 600;
			T2d[2][1] = (1 - T2d[1][1]) * 335;
			lastPoint = point;
			for (size_t i = 0; i < 6; i++)
			{
				pos[0] = myStartPoints[i].x;
				pos[1] = myStartPoints[i].y;
				pos[2] = 1;
				int* result = MyMatrixMultiply(pos, T2d);
				myStartPoints[i].x = result[0];
				myStartPoints[i].y = result[1];
			}
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			my_pDC->Polyline(myStartPoints, 6);
		}
		if (cs == "直线段裁剪")
		{
			CPen penGreen(PS_SOLID, 3, RGB(0, 255, 0));
			CPen* pOldgreen = my_pDC->SelectObject(&penGreen);
			CPoint dots[5] = { {lastPoint.x,lastPoint.y},{lastPoint.x,point.y},{point.x,point.y},{point.x,lastPoint.y},{lastPoint.x,lastPoint.y} };
			//my_pDC->Polyline(dots, 5);
			int XL = lastPoint.x < point.x ? lastPoint.x : point.x;
			int XR = lastPoint.x > point.x ? lastPoint.x : point.x;
			int YB = lastPoint.y < point.y ? lastPoint.y : point.y;
			int YT = lastPoint.y > point.y ? lastPoint.y : point.y;
			int code1 = EnCode(500,500,XL,XR,YB,YT);
			int code2 = EnCode(800,200,XL,XR,YB,YT);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			my_pDC->Polyline(dots, 5);
			LineClip(500, 500, 800, 200, code1, code2, XL, XR, YB, YT);
		}
		if (cs == "圆形裁剪")
		{
			CPen penBlue(PS_SOLID, 1, RGB(0, 0, 255));
			CPen* pOldblue = my_pDC->SelectObject(&penBlue);
			my_pDC->MoveTo(500, 500);
			my_pDC->LineTo(800, 200);
			my_pDC->SelectObject(pOldblue);
			int x, y, d, radius;
			int tempI = 0;
			CPoint intersectPoints[2];
			radius = sqrt(pow((point.x - lastPoint.x), 2) + pow((point.y - lastPoint.y), 2));
			x = lastPoint.x;
			y = lastPoint.y - radius;
			d = 1 - radius;
			my_pDC->SetPixel(x, y, RGB(0, 255, 0));
			while (x + y < lastPoint.x + lastPoint.y)
			{
				if (d < 0)
				{
					d += 2 * x + 3 - 2 * lastPoint.x;
					x++;
				}
				else
				{
					d += 2 * (x + y) + 5 - 2 * (lastPoint.x + lastPoint.y);
					x++, y++;
				}
				if (my_pDC->GetPixel(x, y) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = x;
					intersectPoints[tempI].y = y;
					tempI += 1;
				}
				if (my_pDC->GetPixel(lastPoint.x + lastPoint.y - y, lastPoint.x + lastPoint.y - x) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = lastPoint.x + lastPoint.y - y;
					intersectPoints[tempI].y = lastPoint.x + lastPoint.y - x;
					tempI += 1;
				}
				if (my_pDC->GetPixel(2 * lastPoint.x - x, y) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = 2 * lastPoint.x - x;
					intersectPoints[tempI].y = y;
					tempI += 1;
				}
				if (my_pDC->GetPixel(lastPoint.x + y - lastPoint.y, lastPoint.x + lastPoint.y - x) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = lastPoint.x + y - lastPoint.y;
					intersectPoints[tempI].y = lastPoint.x + lastPoint.y - x;
					tempI += 1;
				}
				if (my_pDC->GetPixel(2 * lastPoint.x - x, 2 * lastPoint.y - y) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = 2 * lastPoint.x - x;
					intersectPoints[tempI].y = 2 * lastPoint.y - y;
					tempI += 1;
				}
				if (my_pDC->GetPixel(lastPoint.x + y - lastPoint.y, x + lastPoint.y - lastPoint.x) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = lastPoint.x + y - lastPoint.y;
					intersectPoints[tempI].y = x + lastPoint.y - lastPoint.x;
					tempI += 1;
				}
				if (my_pDC->GetPixel(lastPoint.x + lastPoint.y - y, x + lastPoint.y - lastPoint.x) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = lastPoint.x + lastPoint.y - y;
					intersectPoints[tempI].y = x + lastPoint.y - lastPoint.x;
					tempI += 1;
				}
				if (my_pDC->GetPixel(x, 2 * lastPoint.y - y) == RGB(0, 0, 255))
				{
					intersectPoints[tempI].x = x;
					intersectPoints[tempI].y = 2 * lastPoint.y - y;
					tempI += 1;
				}
				//my_pDC->SetPixel(x, y, RGB(0, 255, 0));
				//my_pDC->SetPixel(point.x + point.y - y, point.x + point.y - x, RGB(0, 255, 0));
				//my_pDC->SetPixel(2 * point.x - x, y, RGB(0, 255, 0));
				//my_pDC->SetPixel(point.x + y - point.y, point.x + point.y - x, RGB(0, 255, 0));
				//my_pDC->SetPixel(2 * point.x - x, 2 * point.y - y, RGB(0, 255, 0));
				//my_pDC->SetPixel(point.x + y - point.y, x + point.y - point.x, RGB(0, 255, 0));
				//my_pDC->SetPixel(point.x + point.y - y, x + point.y - point.x, RGB(0, 255, 0));
				//my_pDC->SetPixel(x, 2 * point.y - y, RGB(0, 255, 0));
			}
			tempI = 0;
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			DrawCircle1(lastPoint, radius, RGB(0, 255, 0));
			my_pDC->SelectObject(&penBlue);
			my_pDC->MoveTo(intersectPoints[0].x, intersectPoints[0].y);
			my_pDC->LineTo(intersectPoints[1].x, intersectPoints[1].y);
			intersectPoints[0].x = 0;
			intersectPoints[0].y = 0;
			intersectPoints[1].x = 0;
			intersectPoints[1].y = 0;
		}
		if (cs_parent + cs == "三维图形缩放")
		{
			if (xFlag)
			{
				// MessageBox(TEXT("X键被按下"));
				if (point.x > lastPoint3D.x)
				{
					T3d[0][0] = float(point.x - lastPoint3D.x) / 1000 + 1.0;
				}
				else
				{
					T3d[0][0] = 1 - float(lastPoint3D.x - point.x) / 1000;
				}
				lastPoint3D = point;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][0] = result[i][0];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
			if (yFlag)
			{
				// MessageBox(TEXT("Y键被按下"));
				if (point.x > lastPoint3D.x)
				{
					T3d[1][1] = float(point.x - lastPoint3D.x) / 1000 + 1.0;
				}
				else
				{
					T3d[1][1] = 1 - float(lastPoint3D.x - point.x) / 1000;
				}
				lastPoint3D = point;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][1] = result[i][1];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
			if (zFlag)
			{
				// MessageBox(TEXT("Z键被按下"));
				if (point.x > lastPoint3D.x)
				{
					T3d[2][2] = float(point.x - lastPoint3D.x) / 1000 + 1.0;
				}
				else
				{
					T3d[2][2] = 1 - float(lastPoint3D.x - point.x) / 1000;
				}
				lastPoint3D = point;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][2] = result[i][2];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
		}
		if (cs_parent + cs == "三维图形旋转")
		{
			float theta = 0.0;
			if (xFlag)
			{
				theta = ((float(point.x - lastPoint3D.x) / 10) * PI)/180;
				lastPoint3D = point;
				T3d[1][1] = cos(theta);
				T3d[1][2] = sin(theta);
				T3d[2][1] = -T3d[1][2];
				T3d[2][2] = T3d[1][1];
				T3d[0][0] = 1.0;
				T3d[3][3] = 1.0;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][1] = result[i][1];
					cubeCoord[i][2] = result[i][2];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
			if (yFlag)
			{
				theta = ((float(point.x - lastPoint3D.x) / 10) * PI) / 180;
				lastPoint3D = point;
				T3d[0][0] = cos(theta);
				T3d[0][2] = -sin(theta);
				T3d[2][0] = -T3d[0][2];
				T3d[2][2] = T3d[0][0];
				T3d[1][1] = 1.0;
				T3d[3][3] = 1.0;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][0] = result[i][0];
					cubeCoord[i][2] = result[i][2];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
			if (zFlag)
			{
				theta = ((float(point.x - lastPoint3D.x) / 10) * PI) / 180;
				lastPoint3D = point;
				T3d[0][0] = cos(theta);
				T3d[0][1] = sin(theta);
				T3d[1][0] = -T3d[0][1];
				T3d[1][1] = T3d[0][0];
				T3d[2][2] = 1.0;
				T3d[3][3] = 1.0;
				int(*result)[4];
				result = MyMatrixMultiply2(cubeCoord, T3d);
				for (size_t i = 0; i < 8; i++)
				{
					cubeCoord[i][0] = result[i][0];
					cubeCoord[i][1] = result[i][1];
				}
				finalCubeCoord = MyMatrixMultiply2(cubeCoord, isometricProjection);
				for (size_t i = 0; i < 8; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (j == 0)
						{
							finalCubeCoord[i][j] += 500;
						}
						else
						{
							finalCubeCoord[i][j] += 300;
						}
					}
				}
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				DrawCube(finalCubeCoord);
			}
		}
		if (cs_parent + cs == "曲线Bezier曲线")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
		if (cs_parent + cs == "曲线B样条曲线")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
		if (cs_parent + cs == "曲面Bezier曲面")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
		if (cs_parent + cs == "曲面B样条曲面")
		{
			clickedFlag = 0;
			pointMove = 0;
		}
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CCGExperimentDlg::DrawLine2(CPoint point1, CPoint point2, int color)
{
	int a, b, delta1, delta2, d, x, y;
	a = point1.y - point2.y;
	b = point2.x - point1.x;
	x = point1.x;
	y = point1.y;
	my_pDC->SetPixel(x, y, color);
	if (abs(a)>abs(b))
	{
		if (b > 0 && a < 0)
		{
			d = a + b * 2;
			delta1 = b * 2;
			delta2 = (a + b) * 2;
			while (y < point2.y)
			{
				if (d < 0)
				{
					y++, x++;
					d += delta1;
				}
				else
				{
					y++;
					d += delta2;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
		else if (b > 0 && a > 0)
		{
			d = a - b * 2;
			delta1 = -b * 2;
			delta2 = (a - b) * 2;
			while (y > point2.y)
			{
				if (d < 0)
				{
					y--, x++;
					d += delta2;
				}
				else
				{
					y--;
					d += delta1;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
		else
		{
			d = -a + b * 2;
			delta1 = b * 2;
			delta2 = (-a + b) * 2;
			while (y < point2.y)
			{
				if (d < 0)
				{
					y++, x--;
					d += delta2;
				}
				else
				{
					y++;
					d += delta1;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
		
	}
	else
	{
		if (a < 0 && b < 0)
		{
			d = -2 * a + b;
			delta1 = -2 * a;
			delta2 = (b - a) * 2;
			/*
			CString str;
			str.Format(TEXT("a=%d,b=%d,d=%d,delta1=%d,delta2=%d,x1=%d,y1=%d,x2=%d,y2=%d"),
				a, b, d, delta1, delta2, x, y, point2.x, point2.y);
			MessageBox(str);
			*/
			while (x > point2.x)
			{
				if (d < 0)
				{
					x--, y++;
					d += delta1;
				}
				else
				{
					x--;
					d += delta2;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
		else if (a > 0 && b > 0)
		{
			d = (a << 2) - b;
			delta1 = a << 2;
			delta2 = (a - b) << 2;
			
			//CString str;
			//str.Format(TEXT("a=%d,b=%d,d=%d,delta1=%d,delta2=%d,x1=%d,y1=%d,x2=%d,y2=%d"),
			//	a, b, d, delta1, delta2, x, y, point2.x, point2.y);
			//MessageBox(str);
			
			while (x < point2.x)
			{
				if (d < 0)
				{
					x++, y--;
					d += delta1;
				}
				else
				{
					x++;
					d += delta2;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
		else
		{
			d = a << 2 + b;
			delta1 = a << 2;
			delta2 = (a + b) << 2;
			while (x < point2.x)
			{
				if (d < 0)
				{
					x++, y++;
					d += delta2;
				}
				else
				{
					x++;
					d += delta1;
				}
				my_pDC->SetPixel(x, y, color);
			}
		}
	}
	
}

void CCGExperimentDlg::DrawLine3(CPoint point1, CPoint point2, int color)
{
	int x, y, dx, dy, e;
	dx = point2.x - point1.x;
	dy = point2.y - point1.y;
	e = -dx;
	x = point1.x;
	y = point1.y;
	for (size_t i = 0; i <= dx; i++)
	{
		my_pDC->SetPixel(x, y, color);
		x += 1;
		e += 2 * dy;
		if (e >= 0)
		{
			y += 1;
			e -= 2 * dx;
		}
	}
}

void CCGExperimentDlg::DrawCircle1(CPoint point, int radius, int color)
{
	int x, y, d;
	x = point.x;
	y = point.y - radius;
	d = 1 - radius;
	my_pDC->SetPixel(x, y, color);
	while (x+y < point.x+point.y)
	{
		if (d < 0)
		{
			d += 2 * x + 3 - 2 * point.x;
			x++;
		}
		else
		{
			d += 2 * (x + y) + 5 - 2 * (point.x + point.y);
			x++, y++;
		}
		my_pDC->SetPixel(x, y, color);
		my_pDC->SetPixel(point.x + point.y - y, point.x + point.y - x, color);
		my_pDC->SetPixel(2 * point.x - x, y, color);
		my_pDC->SetPixel(point.x + y - point.y, point.x + point.y - x, color);
		my_pDC->SetPixel(2 * point.x - x, 2 * point.y - y, color);
		my_pDC->SetPixel(point.x + y - point.y, x + point.y - point.x, color);
		my_pDC->SetPixel(point.x + point.y - y, x + point.y - point.x, color);
		my_pDC->SetPixel(x, 2 * point.y - y, color);
	}
}

void CCGExperimentDlg::DrawCircle2(CPoint point, int radius, int color)
{
	int x, y, d;
	x = point.x;
	y = point.y - radius;
	d = 3 - 2 * radius;
	while (x + y < point.x + point.y)
	{
		if (d < 0)
		{
			d += 4 * x + 6 - 4 * point.x;
			x++;
		}
		else
		{
			d += 4 * (x + y) - 4 * (point.x + point.y)+ 9;
			x++, y++;
		}
		my_pDC->SetPixel(x, y, color);
		my_pDC->SetPixel(point.x + point.y - y, point.x + point.y - x, color);
		my_pDC->SetPixel(2 * point.x - x, y, color);
		my_pDC->SetPixel(point.x + y - point.y, point.x + point.y - x, color);
		my_pDC->SetPixel(2 * point.x - x, 2 * point.y - y, color);
		my_pDC->SetPixel(point.x + y - point.y, x + point.y - point.x, color);
		my_pDC->SetPixel(point.x + point.y - y, x + point.y - point.x, color);
		my_pDC->SetPixel(x, 2 * point.y - y, color);
	}
}

void CCGExperimentDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int num = MessageBox(TEXT("确定清屏吗?"),TEXT("Tips"), MB_OKCANCEL);
	if (num == 1)
	{
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
	}
}

void CCGExperimentDlg::DrawGossip()
{
	CPen myPen;
	myPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	CBrush* brush1 = new CBrush(RGB(0, 0, 0));
	CBrush* brush2 = new CBrush(RGB(255, 255, 255));
	CPen* oldPen1 = my_pDC->SelectObject(&myPen);
	my_pDC->FillRect(CRect(600, 200, 800, 400),brush1);
	CPoint points[74] = {
		//one
		{685,217},{715,217},{685,221},{715,221},{685,225},{715,225},
		//two
		{685,385},{698,385},{685,380},{698,380},{685,375},{698,375},
		{702,385},{715,385},{702,380},{715,380},{702,375},{715,375},
		//three
		{742,228},{765,248},{739,232},{763,252},{736,236},{747,245},
		{751,249},{761,257},
		//four
		{767,290},{767,303},{767,307},{767,320},{772,290},{772,320},
		{777,290},{777,303},{777,307},{777,320},
		//five
		{745,365},{763,345},{742,362},{750,352},{753,349},{761,340},
		{738,358},{746,348},{749,345},{757,336},
		//six
		{625,290},{625,303},{625,307},{625,320},{630,290},{630,320},
		{635,290},{635,303},{635,307},{635,320},
		//seven
		{645,345},{666,365},{643,349},{653,359},{655,361},{663,369},
		{641,353},{651,363},{653,365},{661,373},
		//eight
		{643,263},{664,243},{640,260},{661,240},{637,257},{648,246},{649,245},{657,237}
	};
	BYTE types[74] = {
		//No.1
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.2
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.3
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},
		//No.4
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.5
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.6
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.7
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		//No.8
		{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},{PT_MOVETO} ,{PT_LINETO},
		{PT_MOVETO} ,{PT_LINETO}
	};
	my_pDC->PolyDraw(points, types, 74);
	my_pDC->MoveTo(755, 300);
	//my_pDC->SelectObject(&oldPen1);
	CPen myPen2;
	myPen2.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen* oldPen2 = my_pDC->SelectObject(&myPen2);
	my_pDC->AngleArc(700, 300, 55, 0, 360);
	my_pDC->Chord(645, 245, 755, 355, 700, 355, 700, 245);
	//my_pDC->MoveTo(645, 245);
	//my_pDC->LineTo(755, 355);
	my_pDC->Chord(673, 245, 728, 300, 700, 245, 700, 300);
	CRgn myRgn1, myRgn2, myRgn3;
	myRgn1.CreateEllipticRgn(673, 300, 728, 355);
	my_pDC->FillRgn(&myRgn1, brush1);
	myRgn2.CreateEllipticRgn(680, 315, 700, 335);
	my_pDC->FillRgn(&myRgn2, brush2);
	myRgn3.CreateEllipticRgn(700, 260, 720, 280);
	my_pDC->FillRgn(&myRgn3, brush1);
	my_pDC->SelectObject(&oldPen2);

}

void CCGExperimentDlg::SmallYellowHuamn()
{
	//画笔集合
	CPen myPen;
	myPen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	my_pDC->SelectObject(&myPen);
	//画刷集合
	CBrush* brush_yellow = new CBrush(RGB(255,255,0));
	CBrush* brush_blue = new CBrush(RGB(135,206,250));
	CBrush* brush_blue2 = new CBrush(RGB(0, 191, 255));
	CBrush* brush_black = new CBrush(RGB(0,0,0));
	CBrush* brush_white = new CBrush(RGB(255, 255, 255));
	CBrush* brush_yellow2 = new CBrush(RGB(247, 186, 11));
	CBrush* brush_brown = new CBrush(RGB(160, 82, 45));
	CBrush* brush_mouse = new CBrush(RGB(250, 128, 114));
	//顶点集合
	CPoint points_1[4] = { {599,295},{650,317},{647,320},{599,300} };
	CPoint points_2[4] = { {751,295},{751,300},{703,320},{700,317} };
	CPoint points_3[4] = { {674,351},{683,360},{675,368},{667,360} };
	CPoint points_4[4] = { {630,220},{670,220},{670,240},{630,240} };
	CPoint points_5[4] = { {680,220},{720,220},{720,240},{680,240} };
	//Rgn集合
	CRgn rgn1, rgn2, rgn3, rgn4, rgn5, rgn6, rgn7, rgn8, rgn9, rgn10, rgn11, rgn12, rgn13,
		 rgn14, rgn15, rgn16, rgn17, rgn18, rgn19, rgn20, rgn21, rgn22, rgn23, rgn24, rgn25;
	rgn1.CreateRoundRectRgn(600, 150, 750, 400, 127, 127);
	rgn2.CreateEllipticRgn(598, 275, 752, 405);
	rgn3.CreateRectRgn(600, 275, 750, 340);
	rgn4.CreateRectRgn(645, 315, 705, 340);
	rgn5.CreatePolygonRgn(points_1, 4, ALTERNATE);
	rgn6.CreatePolygonRgn(points_2, 4, ALTERNATE);
	rgn7.CreateRoundRectRgn(660, 330, 690, 370,30,30);
	rgn8.CreateRectRgn(660, 330, 690, 350);
	rgn9.CreateEllipticRgn(665, 350, 685, 370);
	rgn10.CreatePolygonRgn(points_3, 4, ALTERNATE);
	rgn11.CreateEllipticRgn(670, 355, 680, 365);
	rgn12.CreateEllipticRgn(645, 315, 650, 320);
	rgn13.CreateEllipticRgn(700, 315, 705, 320);
	rgn14.CreateEllipticRgn(620, 200, 680, 260);
	rgn15.CreateEllipticRgn(670, 200, 730, 260);
	rgn16.CreateEllipticRgn(680, 210, 720, 250);
	rgn17.CreateEllipticRgn(630, 210, 670, 250);
	rgn18.CreatePolygonRgn(points_4, 4, ALTERNATE);
	rgn19.CreatePolygonRgn(points_5, 4, ALTERNATE);
	rgn20.CreateEllipticRgn(690, 220, 710, 240);
	rgn21.CreateEllipticRgn(640, 220, 660, 240);
	rgn22.CreateEllipticRgn(695, 225, 705, 235);
	rgn23.CreateEllipticRgn(645, 225, 655, 235);
	rgn24.CreateEllipticRgn(660, 280, 690, 300);
	rgn25.CreateRectRgn(660, 280, 690, 290);

	//画刷涂色
	my_pDC->FillRgn(&rgn1, brush_yellow);
	my_pDC->FillRgn(&rgn2, brush_blue);
	my_pDC->FillRgn(&rgn3, brush_yellow);
	my_pDC->FillRgn(&rgn4, brush_blue);
	my_pDC->FillRgn(&rgn5, brush_blue);
	my_pDC->FillRgn(&rgn6, brush_blue);
	my_pDC->FillRgn(&rgn7, brush_blue2);
	my_pDC->FillRgn(&rgn8, brush_blue);
	my_pDC->FillRgn(&rgn9, brush_black);
	my_pDC->FillRgn(&rgn10, brush_white);
	my_pDC->FillRgn(&rgn11, brush_black);
	my_pDC->FillRgn(&rgn12, brush_black);
	my_pDC->FillRgn(&rgn13, brush_black);
	my_pDC->FillRgn(&rgn14, brush_white);
	my_pDC->FillRgn(&rgn15, brush_white);
	my_pDC->Arc(620, 200, 680, 260, 653, 228, 653, 232);
	my_pDC->Arc(670, 200, 730, 260, 697, 232, 697, 228);
	my_pDC->FillRgn(&rgn16, brush_yellow2);
	my_pDC->FillRgn(&rgn17, brush_yellow2);
	my_pDC->FillRgn(&rgn18, brush_white);
	my_pDC->FillRgn(&rgn19, brush_white);
	my_pDC->Arc(630, 210, 670, 250, 670, 230, 670, 230);
	my_pDC->Arc(680, 210, 720, 250, 720, 230, 720, 230);
	my_pDC->FillRgn(&rgn20, brush_brown);
	my_pDC->FillRgn(&rgn21, brush_brown);
	my_pDC->FillRgn(&rgn22, brush_black);
	my_pDC->FillRgn(&rgn23, brush_black);
	my_pDC->FillRgn(&rgn24, brush_mouse);
	my_pDC->FillRgn(&rgn25, brush_yellow);
}

void CCGExperimentDlg::SmallCat()
{
	CPen myPen;
	CBrush* myBrush =new CBrush(RGB(0, 0, 0));
	myPen.CreatePen(PS_SOLID, 1, RGB(184, 12, 123));
	CPoint points_1[4] = { {600,200},{590,240},{610,240},{600,200} };
	CPoint points_2[4] = { {650,200},{640,240},{660,240},{650,200} };
	CPoint points_3[4] = { {590,240},{625,270},{590,300},{590,240} };
	CPoint points_4[4] = { {660,241},{626,270},{660,299},{660,241} };
	CPoint points_5[4] = { {590,300},{660,300},{625,335},{590,300} };
	CPoint points_6[6] = { {589,300},{625,336},{661,300},{661,435},{589,435},{589,300} };
	CPoint points_7[4] = { {662,368},{680,435},{662,435},{662,368} };
	//绘制线框
	my_pDC->Polyline(points_1, 4);
	my_pDC->Polyline(points_2, 4);
	my_pDC->Polyline(points_5, 4);
	CPen* oldPen = my_pDC->SelectObject(&myPen);
	my_pDC->Polyline(points_3, 4);
	my_pDC->Polyline(points_4, 4);
	my_pDC->Polyline(points_6, 6);
	my_pDC->SelectObject(oldPen);
	my_pDC->Polyline(points_7, 4);
	my_pDC->Arc(608, 246, 620, 258, 613, 246, 613, 246);
	my_pDC->Arc(632, 246, 644, 258, 637, 246, 637, 246);
	SeedFill_stack(CPoint(600, 220), my_pDC->GetPixel(600, 220), RGB(0, 0, 0));
	SeedFill_stack(CPoint(650, 220), my_pDC->GetPixel(650, 220), RGB(0, 0, 0));
	SeedFill_stack(CPoint(625, 320), my_pDC->GetPixel(625, 320), RGB(0, 0, 0));
	SeedFill_stack(CPoint(665, 400), my_pDC->GetPixel(665, 400), RGB(0, 0, 0));
	SeedFill_stack(CPoint(613, 250), my_pDC->GetPixel(613, 250), RGB(0, 0, 0));
	SeedFill_stack(CPoint(637, 250), my_pDC->GetPixel(637, 250), RGB(0, 0, 0));
	SeedFill_stack(CPoint(600, 270), my_pDC->GetPixel(600, 270), RGB(184, 12, 123));
	SeedFill_stack(CPoint(650, 270), my_pDC->GetPixel(650, 270), RGB(184, 12, 123));
	SeedFill_stack(CPoint(625, 380), my_pDC->GetPixel(625, 380), RGB(184, 12, 123));
	my_pDC->FrameRect(CRect(570,283,600,288), myBrush);
	my_pDC->FrameRect(CRect(570,275,600,280), myBrush);
	my_pDC->FrameRect(CRect(570,291,600,296), myBrush);
	my_pDC->FrameRect(CRect(650, 283, 680, 288), myBrush);
	my_pDC->FrameRect(CRect(650, 275, 680, 280), myBrush);
	my_pDC->FrameRect(CRect(650, 291, 680, 296), myBrush);
	SeedFill_stack(CPoint(585, 278), my_pDC->GetPixel(585, 278), RGB(0, 0, 0));
	SeedFill_stack(CPoint(585, 285), my_pDC->GetPixel(585, 285), RGB(0, 0, 0));
	SeedFill_stack(CPoint(585, 293), my_pDC->GetPixel(585, 293), RGB(0, 0, 0));
	SeedFill_stack(CPoint(665, 278), my_pDC->GetPixel(665, 278), RGB(0, 0, 0));
	SeedFill_stack(CPoint(665, 285), my_pDC->GetPixel(665, 285), RGB(0, 0, 0));
	SeedFill_stack(CPoint(665, 293), my_pDC->GetPixel(665, 293), RGB(0, 0, 0));
}

void CCGExperimentDlg::SmallFox()
{
	CPen pen1, pen2, pen3;
	pen1.CreatePen(PS_SOLID, 1, RGB(135,51,26));
	pen2.CreatePen(PS_SOLID, 1, RGB(199, 74, 36));
	pen3.CreatePen(PS_SOLID, 1, RGB(236, 225, 222));
	CPoint points_4[4] = { {600,198},{535,150},{540,234},{600,198} };
	CPoint points_5[4] = { {660,198},{725,150},{720,234},{660,198} };
	CPoint points_1[4] = { {600,200},{660,200},{630,400},{600,200} };
	CPoint points_6[4] = { {540,237},{580,281},{510,300},{540,237} };
	CPoint points_7[4] = { {720,237},{680,281},{750,300},{720,237} };
	CPoint points_2[4] = { {599,200},{617,320},{540,235},{599,200} };
	CPoint points_3[4] = { {661,200},{643,320},{720,235},{661,200} };
	CPoint points_8[4] = { {580,282},{615,321},{630,405},{580,282} };
	CPoint points_9[4] = { {680,282},{645,321},{630,405},{680,282} };
	CPoint points_10[4] = { {580,283},{615,370},{510,301},{580,283} };
	CPoint points_11[4] = { {680,283},{645,370},{750,301},{680,283} };
	my_pDC->Polyline(points_4, 4);
	my_pDC->Polyline(points_5, 4);
	my_pDC->SelectObject(&pen1);
	my_pDC->Polyline(points_1, 4);
	my_pDC->Polyline(points_6, 4);
	my_pDC->Polyline(points_7, 4);
	my_pDC->SelectObject(&pen2);
	my_pDC->Polyline(points_2, 4);
	my_pDC->Polyline(points_3, 4);
	my_pDC->SelectObject(&pen3);
	my_pDC->Polyline(points_8, 4);
	my_pDC->Polyline(points_9, 4);
	my_pDC->Polyline(points_10, 4);
	my_pDC->Polyline(points_11, 4);
	SeedFill_stack(CPoint(630, 300), my_pDC->GetPixel(630, 300), RGB(135, 51, 26));
	SeedFill_stack(CPoint(590, 215), my_pDC->GetPixel(610, 210), RGB(199, 74, 36));
	SeedFill_stack(CPoint(660, 215), my_pDC->GetPixel(660, 210), RGB(199, 74, 36));
	SeedFill_stack(CPoint(570, 200), my_pDC->GetPixel(570, 200), RGB(0,0,0));
	SeedFill_stack(CPoint(690, 200), my_pDC->GetPixel(690, 200), RGB(0,0,0));
	SeedFill_stack(CPoint(540, 280), my_pDC->GetPixel(540, 280), RGB(135, 51, 26));
	SeedFill_stack(CPoint(720, 280), my_pDC->GetPixel(720, 280), RGB(135, 51, 26));
	SeedFill_stack(CPoint(540, 300), my_pDC->GetPixel(540, 300), RGB(236, 225, 222));
	SeedFill_stack(CPoint(720, 300), my_pDC->GetPixel(720, 300), RGB(236, 225, 222));
	SeedFill_stack(CPoint(660, 310), my_pDC->GetPixel(660, 310), RGB(236, 225, 222));
	SeedFill_stack(CPoint(600, 310), my_pDC->GetPixel(600, 310), RGB(236, 225, 222));
}

void CCGExperimentDlg::SeedFill_digui(CPoint point, int oldColor, int newColor)
{
	if (int(my_pDC->GetPixel(point))==oldColor)
	{
		my_pDC->SetPixel(point, newColor);
		SeedFill_digui(CPoint(point.x - 1, point.y), oldColor, newColor);
		SeedFill_digui(CPoint(point.x + 1, point.y), oldColor, newColor);
		SeedFill_digui(CPoint(point.x, point.y + 1), oldColor, newColor);
		SeedFill_digui(CPoint(point.x, point.y - 1), oldColor, newColor);
	}
}

void CCGExperimentDlg::SeedFill_stack(CPoint point, int oldColor, int newColor)
{
	double startTime, endTime;
	double costTime;
	if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充栈")
	{
		startTime = clock();
	}
	std::stack<ColorPixel> pixelStack;
	pixelStack.push(ColorPixel(point, oldColor));
	ColorPixel top;
	while (!pixelStack.empty())
	{
		top = pixelStack.top();
		pixelStack.pop();
		my_pDC->SetPixel(top.point, newColor);
		if (my_pDC->GetPixel(top.point.x,top.point.y-1)!=newColor)
		{
			pixelStack.push(ColorPixel(CPoint(top.point.x, top.point.y - 1), oldColor));
		}
		if (my_pDC->GetPixel(top.point.x, top.point.y + 1) != newColor)
		{
			pixelStack.push(ColorPixel(CPoint(top.point.x, top.point.y + 1), oldColor));
		}
		if (my_pDC->GetPixel(top.point.x - 1, top.point.y) != newColor)
		{
			pixelStack.push(ColorPixel(CPoint(top.point.x - 1, top.point.y), oldColor));
		}
		if (my_pDC->GetPixel(top.point.x + 1, top.point.y) != newColor)
		{
			pixelStack.push(ColorPixel(CPoint(top.point.x + 1, top.point.y), oldColor));
		}
	}
	if (cs_grandparent + cs_parent + cs == "多边形填充种子填充种子填充栈")
	{
		endTime = clock();
		costTime = (endTime - startTime) / 1000;
		CString tip;
		tip.Format(TEXT("时间为%f"), costTime);
		MessageBox(tip);
	}
}

void CCGExperimentDlg::Polygonfill(std::vector<CPoint> points, int color)
{
	int points_num = points.size();
	std::vector<Edge> edges;
	//for (size_t i = 0; i < points_num - 1; i++)
	//{
	//	Edge edge = { points[i].y<points[i+1].y ? points[i].x : points[i+1].x,(points[i+1].x - points[i].x) / (points[i+1].y - points[i].y),points[i].y>points[i+1].y ? points[i].y : points[i+1].y };
	//	edges.push_back(edge);
	//}
	std::vector<int> ys;
	for (size_t i = 0; i < points_num; i++)
	{
		ys.push_back(int(points[i].y));
	}
	std::sort(ys.begin(), ys.end());
	std::vector<std::vector<EdgeNode>> NETS;
	std::vector<EdgeNode> link;
	//Edge null_edge;
	//null_edge.x = 0;
	//null_edge.step = 0.0;
	//null_edge.y_min = 0;
	//null_edge.y_max = 0;
	//std::vector<EdgeNode> my_null;
	//my_null.push_back(EdgeNode(null_edge));
	int i = 0;
	//std::vector<int> xs;
	//初始化新边表
	for (size_t y = ys[0]; y < ys[points_num - 1]; y++)
	{
		if (y == ys[i])
		{
			for (size_t j = 0; j < points_num; j++)
			{
				if (points[j].y == y && j == 0 && points[points_num - 1].y > y)
				{
					Edge edge = {points[j].x,((double)(points[j].x - points[points_num - 1].x))/(points[j].y - points[points_num - 1].y),y,points[points_num - 1].y };
					edges.push_back(edge);
				}
				if (points[j].y == y && j == 0 && points[1].y > y)
				{
					Edge edge = { points[j].x,((double)(points[j].x - points[1].x)) / (points[j].y - points[1].y),y,points[1].y };
					edges.push_back(edge);
				}
				if (points[j].y == y && j != 0 && j != points_num - 1 && points[j - 1].y > y)
				{
					Edge edge = { points[j].x,((double)(points[j].x - points[j - 1].x)) / (points[j].y - points[j - 1].y),y,points[j - 1].y };
					edges.push_back(edge);
				}
				if (points[j].y == y && j != 0 && j != points_num - 1 && points[j + 1].y > y)
				{
					Edge edge = { points[j].x,((double)(points[j].x - points[j + 1].x)) / (points[j].y - points[j + 1].y),y,points[j + 1].y };
					edges.push_back(edge);
				}
				//if (points[j].y == y && j != points_num - 1 && points[j + 1].y > y)
				//{
				//	Edge edge = { points[j].x,((double)(points[j].x - points[j + 1].x)) / (points[j].y - points[j + 1].y),y,points[j + 1].y };
				//	edges.push_back(edge);
				//}
				//if (points[j].y == y && j != points_num - 1 && points[j - 1].y > y)
				//{
				//	Edge edge = { points[j].x,((double)(points[j].x - points[j - 1].x)) / (points[j].y - points[j - 1].y),y,points[j - 1].y };
				//	edges.push_back(edge);
				//}
				if (points[j].y == y && j == points_num - 1 && points[0].y > y)
				{
					Edge edge = { points[j].x,((double)(points[j].x - points[0].x)) / (points[j].y - points[0].y),y,points[0].y };
					edges.push_back(edge);
				}
				if (points[j].y == y && j == points_num - 1 && points[points_num - 2].y > y)
				{
					Edge edge = { points[j].x,((double)(points[j].x - points[points_num - 2].x)) / (points[j].y - points[points_num - 2].y),y,points[points_num - 2].y };
					edges.push_back(edge);
				}
			}
			int new_edges_num = edges.size();
			if (new_edges_num!=0)
			{
				for (size_t c = 0; c <= new_edges_num - 1; c++)
				{
					EdgeNode edgeNode = EdgeNode(edges[c]);
					link.push_back(edgeNode);
				}
				//for (size_t c = 0; c <= new_edges_num-2; c++)
				//{
				//	if (link[c+1]!=NULL)
				//	{

				//	}
				//	link[c].next = &link[c + 1];
				//}
				NETS.push_back(link);
				link.swap(std::vector<EdgeNode>());
				edges.swap(std::vector<Edge>());
			}
			//else
			//{
			//	NETS.push_back(my_null);
			//	edges.swap(std::vector<Edge>());
			//}
			i++;
		}
	}
	//CString str1;
	//str1.Format(TEXT("%d"),NETS[0].size());
	//MessageBox(str1);
	//CString str;
	//str.Format(TEXT("%d %f %d %d %f %d"), NETS[0][0].edge.x, NETS[0][0].edge.step, NETS[0][0].edge.y_max, NETS[0][1].edge.x, NETS[0][1].edge.step, NETS[0][1].edge.y_max);
	//MessageBox(str);
	int current_y = ys[0];
	std::vector<EdgeNode> AET;
	int i_2 = 0;
	for (current_y; current_y < ys[points_num-1]; current_y++)
	{
		//if (current_y==ys[i_2])
		//{
		//	//AET.swap(std::vector<EdgeNode>());
		//	if (i_2==0)
		//	{
		//		for (int j = 0; j < NETS[i_2].size(); j++)
		//		{
		//			if (NETS[i_2][j].edge.y_max > current_y)
		//			{
		//				AET.push_back(NETS[i_2][j]);
		//			}
		//		}
		//	}
		//	else
		//	{
		//		for (int j = 1+ NETS[i_2].size() - NETS[i_2 - 1].size(); j < NETS[i_2].size(); j++)
		//		{
		//			if (NETS[i_2][j].edge.y_max > current_y)
		//			{
		//				AET.push_back(NETS[i_2][j]);
		//			}
		//		}
		//	}
		//	//std::vector<int> xs;
		//	//if (i==0)
		//	//{
		//	//	for (size_t j = 0; j < NETS[0].capacity(); j++)
		//	//	{
		//	//		xs.push_back(NETS[0][j].edge.x);
		//	//	}
		//	//}
		//	//else
		//	//{
		//	//	for (size_t j = 0; j < NETS[i].capacity()-NETS[i-1].capacity(); j++)
		//	//	{

		//	//	}
		//	//}
		//	for (size_t j = 0; j < AET.size()-1; j++)
		//	{
		//		for (size_t k = 0; k < AET.size()-j-1; k++)
		//		{
		//			if (AET[k].edge.x > AET[k+1].edge.x)
		//			{
		//				std::swap(AET[k], AET[k + 1]);
		//			}
		//		}
		//	}
		//	i_2++;
		//	for (size_t j = 0; j < AET.size(); j++)
		//	{
		//		if (AET[j].edge.y_max==current_y)
		//		{
		//			AET.erase(AET.begin()+j);
		//		}
		//	}
		//}
		bool isVertex;
		for (size_t c = 0; c < ys.size(); c++)
		{
			if (current_y==ys[c])
			{
				isVertex = true;
				break;
			}
			else
			{
				isVertex = false;
			}
		}
		if (current_y==ys[i_2]&&i_2<NETS.size()&&isVertex)
		{
			for (size_t j = 0; j < NETS[i_2].size(); j++)
			{
				if (NETS[i_2][j].edge.y_min >= current_y)
				{
					AET.push_back(NETS[i_2][j]);
					if (j==NETS[i_2].size()-1)
					{
						i_2++;
						break;
					}
				}
			}
		}
		for (size_t j = 0; j < AET.size()-1; j++)
		{
			for (size_t k = 0; k < AET.size()-j-1; k++)
			{
			if (AET[k].edge.x >= AET[k+1].edge.x)
				{
					std::swap(AET[k], AET[k + 1]);
				}
			}
		}
		for (size_t index = 0; index < AET.size(); index++)
		{
			if (current_y >= AET[index].edge.y_max)
			{
				AET.erase(AET.begin()+index);
				index--;
			}
		}
		for (size_t index = 0; index < AET.size(); index+=2)
		{
			my_pDC->MoveTo(AET[index].edge.x, current_y);
			my_pDC->LineTo(AET[index+1].edge.x, current_y);
			my_pDC->SetPixel(AET[index].edge.x, current_y,RGB(0,255,0));
			my_pDC->SetPixel(AET[index+1].edge.x, current_y,RGB(0,255,0));
			AET[index].edge.x += AET[index].edge.step;
			AET[index+1].edge.x += AET[index+1].edge.step;
		}
	}
}

void CCGExperimentDlg::OnBnClickedSetcolor()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog m_colorDlg;
	m_colorDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	m_colorDlg.m_cc.rgbResult = curColor;
	if (IDOK == m_colorDlg.DoModal())
	{
		curColor = m_colorDlg.m_cc.rgbResult;
	}
}

void CCGExperimentDlg::DrawLineByCPen(CPoint point1, CPoint point2, int color, int lineWidth)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, lineWidth, color);
	CPen* oldpen = my_pDC->SelectObject(&pen);
	my_pDC->MoveTo(point1);
	my_pDC->LineTo(point2);
	my_pDC->SelectObject(oldpen);
}

void CCGExperimentDlg::OnBnClickedSetlinewidth()
{
	// TODO: 在此添加控件通知处理程序代码
	CSetLineWidthDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		curLineWidth = dlg.lineWidth;
	}
}

void CCGExperimentDlg::DrawCircleByCPen(CPoint point, int radius, int color, int lineWidth)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, lineWidth, color);
	CPen* oldpen = my_pDC->SelectObject(&pen);
	my_pDC->Ellipse(point.x - radius, point.y - radius, point.x + radius, point.y + radius);
	my_pDC->SelectObject(oldpen);
}

void CCGExperimentDlg::MySpecialName()
{
	//画刷与画笔
	//CBrush* brush1 = new CBrush(RGB(25, 202, 173));
	CBrush* brush2 = new CBrush(RGB(160, 238, 225));
	CBrush* brush3 = new CBrush(RGB(190, 231, 233));
	//CBrush* brush4 = new CBrush(RGB(190, 237, 199));
	CBrush* brush5 = new CBrush(RGB(214, 213, 183));
	CBrush* brush6 = new CBrush(RGB(230, 206, 172));
	CBrush* brush7 = new CBrush(RGB(236, 173, 158));
	CBrush* brush8 = new CBrush(RGB(238, 238, 238));
	CBrush* brush9 = new CBrush(RGB(244, 96, 108));
	CPen pen1(PS_SOLID, 15, RGB(25,202,173));
	CPen pen2(PS_SOLID, 15, RGB(236, 173, 158));
	CPen pen3(PS_SOLID, 15, RGB(214, 213, 183));
	CPen pen4(PS_SOLID, 15, RGB(190, 237, 199));
	CPen pen5(PS_SOLID, 15, RGB(244, 96, 108));
	//形状
	CRect* rect1 = new CRect(500,200,600,215);
	CRect* rect2 = new CRect(520,180,535,235);
	CRect* rect3 = new CRect(565,180,580,235);
	CRect* rect4 = new CRect(480,235,620,250);
	CRect* rect5 = new CRect(500,265,600,335);
	CRect* rect6 = new CRect(520,280,580,320);
	CRect* rect7 = new CRect(521,293,579,307);
	CRect* rect8 = new CRect(542,250,558,320);
	CPoint points1[4] = {
		{508,342},{522,350},{505,380},{490,370}
	};
	CPoint points2[4] = {
		{578,350},{592,342},{610,370},{596,380}
	};
	CRgn rgn1,rgn2;
	rgn1.CreatePolygonRgn(points1, 4, ALTERNATE);
	rgn2.CreatePolygonRgn(points2, 4, ALTERNATE);
	CRect* rect9 = new CRect(650, 280, 750, 300);
	CPoint points3[4] = {
		{815,220},{820,280},{810,350},{750,390}
	};
	CPoint points4[4] = {
		{900, 380},{920,395},{940,395},{960,380}
	};
	//填充
	my_pDC->FillRect(rect1, brush7);
	my_pDC->FillRect(rect2, brush5);
	my_pDC->FillRect(rect3, brush6);
	my_pDC->FillRect(rect4, brush3);
	my_pDC->SelectObject(brush2);
	my_pDC->RoundRect(rect5, CPoint(20, 20));
	my_pDC->SelectObject(brush8);
	my_pDC->RoundRect(rect6, CPoint(10, 10));
	my_pDC->FillRect(rect7, brush7);
	my_pDC->FillRect(rect8, brush9);
	my_pDC->FillRgn(&rgn1, brush6);
	my_pDC->FillRgn(&rgn2, brush5);
	my_pDC->FillRect(rect9, brush7);
	my_pDC->SelectObject(&pen1);
	my_pDC->PolyBezier(points3, 4);
	my_pDC->SelectObject(&pen2);
	my_pDC->MoveTo(815,220);
	my_pDC->LineTo(900,220);
	my_pDC->SelectObject(&pen3);
	my_pDC->LineTo(900, 380);
	my_pDC->SelectObject(&pen4);
	my_pDC->PolyBezier(points4, 4);
	my_pDC->MoveTo(960, 380);
	my_pDC->LineTo(960, 360);
	my_pDC->MoveTo(840, 270);
	my_pDC->LineTo(870, 300);
}

void CCGExperimentDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	curColor = RGB(0, 0, 0);
	curLineWidth = 1.0;
}

void CCGExperimentDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (cs_parent + cs == "二维几何变换平移变换" && flag == 1)
	{
		Tx = point.x - lastPoint.x;
		Ty = point.y - lastPoint.y;
		lastPoint = point;
		left += Tx;
		right += Tx;
		top += Ty;
		bottom += Ty;
		for (size_t i = 0; i < 6; i++)
		{
			myStartPoints[i].x += Tx;
			myStartPoints[i].y += Ty;
		}
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
		my_pDC->Polyline(myStartPoints, 6);
		
	}
	else if (cs_parent + cs == "多边形绘制五边形")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < 5; i++)
		{
			if (point.x > fivePoints[i].x - 5 && point.x < fivePoints[i].x + 5 && point.y > fivePoints[i].y - 5 && point.y < fivePoints[i].y + 5 && clickedFlag == 1)
			{
				pointMove = 1;
				seq = i;
			}
		}
		if (pointMove == 1)
		{
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			fivePoints[seq].x = point.x;
			fivePoints[seq].y = point.y;
			my_pDC->Polygon(fivePoints, 5);
		}
	}
	else if (cs_parent + cs == "多边形绘制六边形")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < 6; i++)
		{
			if (point.x > sixPoints[i].x - 5 && point.x < sixPoints[i].x + 5 && point.y > sixPoints[i].y - 5 && point.y < sixPoints[i].y + 5 && clickedFlag == 1)
			{
				pointMove = 1;
				seq = i;
			}
		}
		if (pointMove == 1)
		{
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			sixPoints[seq].x = point.x;
			sixPoints[seq].y = point.y;
			my_pDC->Polygon(sixPoints, 6);
		}
	}
	else if (cs_parent + cs == "三维图形平移" && flag3D == 1)
	{
		int disX, disY;
		disX = point.x - lastPoint3D.x;
		disY = point.y - lastPoint3D.y;
		lastPoint3D = point;
		left3D += disX;
		right3D += disX;
		top3D += disY;
		bottom3D += disY;
		for (size_t i = 0; i < 8; i++)
		{
			finalCubeCoord[i][0] += disX;
			finalCubeCoord[i][2] += disY;
		}
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
		DrawCube(finalCubeCoord);
	}
	else if (cs_parent + cs == "曲线Bezier曲线")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < n + 1; i++)
		{
			if (point.x > ctrlPoints[i].x - 5 && point.x < ctrlPoints[i].x + 5 && point.y > ctrlPoints[i].y - 5 && point.y < ctrlPoints[i].y + 5 && clickedFlag == 1)
			{
				pointMove = 1;
				seq = i;
			}
		}
		if (pointMove == 1)
		{
			CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
			CPen* pOldPen = my_pDC->SelectObject(&penRed);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			for (size_t j = 0; j < n + 1; j++)
			{
				if (j != seq)
				{
					my_pDC->MoveTo(ctrlPoints[j].x, ctrlPoints[j].y);
					my_pDC->LineTo(ctrlPoints[j].x, ctrlPoints[j].y);
				}
			}
			my_pDC->MoveTo(point.x, point.y);
			my_pDC->LineTo(point.x, point.y);
			ctrlPoints[seq].x = point.x;
			ctrlPoints[seq].y = point.y;
			CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
			my_pDC->SelectObject(&penBlue);
			for (size_t i = 0; i < n; i++)
			{
				my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
				my_pDC->LineTo(ctrlPoints[i + 1].x, ctrlPoints[i + 1].y);
			}
			DrawBezierCurve(n,ctrlPoints);
		}
	}
	else if (cs_parent + cs == "曲线B样条曲线")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < pointsNum; i++)
		{
			if (point.x > ctrlPoints[i].x - 5 && point.x < ctrlPoints[i].x + 5 && point.y > ctrlPoints[i].y - 5 && point.y < ctrlPoints[i].y + 5 && clickedFlag == 1)
			{
				pointMove = 1;
				seq = i;
			}
		}
		if (pointMove == 1)
		{
			CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
			CPen* pOldPen = my_pDC->SelectObject(&penRed);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			for (size_t j = 0; j < pointsNum; j++)
			{
				if (j != seq)
				{
					my_pDC->MoveTo(ctrlPoints[j].x, ctrlPoints[j].y);
					my_pDC->LineTo(ctrlPoints[j].x, ctrlPoints[j].y);
				}
			}
			my_pDC->MoveTo(point.x, point.y);
			my_pDC->LineTo(point.x, point.y);
			ctrlPoints[seq].x = point.x;
			ctrlPoints[seq].y = point.y;
			CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
			my_pDC->SelectObject(&penBlue);
			for (size_t i = 0; i < pointsNum - 1; i++)
			{
				my_pDC->MoveTo(ctrlPoints[i].x, ctrlPoints[i].y);
				my_pDC->LineTo(ctrlPoints[i + 1].x, ctrlPoints[i + 1].y);
			}
			DrawBSampleCurve(pointsNum, ctrlPoints);
		}
	}
	else if (cs_parent + cs == "曲面Bezier曲面")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (point.x > P[i][j].x - 5 && point.x < P[i][j].x + 5 && point.y > P[i][j].y - 5 && point.y < P[i][j].y + 5 && clickedFlag == 1)
				{
					pointMove = 1;
					seq_i = i;
					seq_j = j;
					break;
				}
			}
			if (pointMove == 1)
			{
				P[seq_i][seq_j].x = point.x;
				P[seq_i][seq_j].y = point.y;
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
				CPen* pOldPen = my_pDC->SelectObject(&penRed);
				for (size_t i = 0; i < 4; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (i != seq_i || j != seq_j)
						{
							my_pDC->MoveTo(P[i][j].x, P[i][j].y);
							my_pDC->LineTo(P[i][j].x, P[i][j].y);
						}
					}
				}
				my_pDC->MoveTo(point.x, point.y);
				my_pDC->LineTo(point.x, point.y);
				DrawBezierCurvedSurface(P);
			}
		}
	}
	else if (cs_parent + cs == "曲面B样条曲面")
	{
		point.y += 38;
		point.x += 8;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (point.x > P[i][j].x - 5 && point.x < P[i][j].x + 5 && point.y > P[i][j].y - 5 && point.y < P[i][j].y + 5 && clickedFlag == 1)
				{
					pointMove = 1;
					seq_i = i;
					seq_j = j;
					break;
				}
			}
			if (pointMove == 1)
			{
				P[seq_i][seq_j].x = point.x;
				P[seq_i][seq_j].y = point.y;
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
				GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
				CPen penRed(PS_SOLID, 10, RGB(255, 0, 0));
				CPen* pOldPen = my_pDC->SelectObject(&penRed);
				for (size_t i = 0; i < 4; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (i != seq_i || j != seq_j)
						{
							my_pDC->MoveTo(P[i][j].x, P[i][j].y);
							my_pDC->LineTo(P[i][j].x, P[i][j].y);
						}
					}
				}
				my_pDC->MoveTo(point.x, point.y);
				my_pDC->LineTo(point.x, point.y);
				DrawBspCurvedSurface(P);
			}
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

int* CCGExperimentDlg::MyMatrixMultiply(int a[3], float b[3][3])
{
	int result[3];
	result[0] = float(a[0]) * b[0][0] + float(a[1]) * b[1][0] + float(a[2]) * b[2][0];
	result[1] = float(a[0]) * b[0][1] + float(a[1]) * b[1][1] + float(a[2]) * b[2][1];
	result[2] = float(a[0]) * b[0][2] + float(a[1]) * b[1][2] + float(a[2]) * b[2][2];
	return result;
}

int(*CCGExperimentDlg::MyMatrixMultiply2(float a[][4], float b[][4]))[4]
{
	int(*result)[4] = new int[8][4];
	float temp;
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			temp = 0;
			for (size_t k = 0; k < 4; k++)
			{
				temp += a[i][k] * b[k][j];
			}
			result[i][j] = int(temp);
		}
	}
	return result;
}

int(*CCGExperimentDlg::MyMatrixMultiply3(float a[][4], float b[][4]))[4]
{
	int(*result)[4] = new int[12][4];
	float temp;
	for (size_t i = 0; i < 12; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			temp = 0;
			for (size_t k = 0; k < 4; k++)
			{
				temp += a[i][k] * b[k][j];
			}
			result[i][j] = int(temp);
		}
	}
	return result;
}

void CCGExperimentDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (cs == "缩放变换")
	{
		lastPoint = point;
		xyFlag = 2;
	}

	CDialogEx::OnRButtonDown(nFlags, point);
}

void CCGExperimentDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			T2d[i][j] = 0;
		}
	}
	if (point.y > lastPoint.y)
	{
		T2d[1][1] = float(point.y - lastPoint.y) / 600 + 1.0;
		T2d[0][0] = 1;
	}
	else
	{
		T2d[1][1] = 1 - float(lastPoint.y - point.y) / 600;
		T2d[0][0] = 1;
	}
	T2d[2][2] = 1;
	T2d[2][0] = (1 - T2d[0][0]) * 600;
	T2d[2][1] = (1 - T2d[1][1]) * 335;
	lastPoint = point;
	for (size_t i = 0; i < 6; i++)
	{
		pos[0] = myStartPoints[i].x;
		pos[1] = myStartPoints[i].y;
		pos[2] = 1;
		int* result = MyMatrixMultiply(pos, T2d);
		myStartPoints[i].x = result[0];
		myStartPoints[i].y = result[1];
	}
	GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
	my_pDC->Polyline(myStartPoints, 6);

	CDialogEx::OnRButtonUp(nFlags, point);
}

int CCGExperimentDlg::EnCode(int x, int y, int XL, int XR, int YB, int YT)
{
	int c = 0;
	if (x < XL) c = c | LEFT;
	else if (x > XR) c = c | RIGHT;
	if (y < YB) c = c | BOTTOM;
	else if (y > YT) c = c | TOP;
	return c;
}

void CCGExperimentDlg::LineClip(int x1, int y1, int x2, int y2,int code1, int code2,int XL,int XR,int YB,int YT)
{
	int code,x = 0 ,y = 0;
	while (code1 != 0 || code2 != 0)
	{
		if ((code1 & code2) != 0) return;
		code = code1;
		if (code1 == 0) code = code2;
		if ((LEFT & code) != 0)
		{
			x = XL;
			y = y1 + (y2 - y1) * float(XL - x1) / (x2 - x1);
		}
		else if ((RIGHT & code) != 0)
		{
			x = XR;
			y = y1 + (y2 - y1) * float(XR - x1) / (x2 - x1);
		}
		else if ((BOTTOM & code) != 0)
		{
			y = YB;
			x = x1 + (x2 - x1) * (YB - y1) / (y2 - y1);
		}
		else if ((TOP & code) != 0)
		{
			y = YT;
			x = x1 + (x2 - x1) * (YT - y1) / (y2 - y1);
		}
		if (code==code1)
		{
			x1 = x;
			y1 = y;
			code1 = EnCode(x,y,XL,XR,YB,YT);
		}
		else
		{
			x2 = x;
			y2 = y;
			code2 = EnCode(x, y, XL, XR, YB, YT);
		}
	}
	CPen penBlue(PS_SOLID, 3, RGB(0, 0, 255));
	CPen* pOldblue = my_pDC->SelectObject(&penBlue);
	my_pDC->MoveTo(x1,y1);
	my_pDC->LineTo(x2,y2);
	my_pDC->SelectObject(pOldblue);
	return;
}

void CCGExperimentDlg::Symmetrical(float T2d[][3], CPoint points[])
{
	int temp[3];
	temp[2] = 1;
	for (size_t i = 0; i < 6; i++)
	{
		temp[0] = points[i].x;
		temp[1] = points[i].y;
		points[i].x = MyMatrixMultiply(temp, T2d)[0];
		points[i].y = MyMatrixMultiply(temp, T2d)[1];
	}
	my_pDC->Polygon(points, 6);
}

void CCGExperimentDlg::DrawCube(int coord[8][4])
{
	CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
	CPen* pOldPen = my_pDC->SelectObject(&penRed);
	my_pDC->MoveTo(coord[0][0], coord[0][2]);
	my_pDC->LineTo(coord[1][0], coord[1][2]);
	my_pDC->LineTo(coord[2][0], coord[2][2]);
	my_pDC->LineTo(coord[3][0], coord[3][2]);
	my_pDC->LineTo(coord[0][0], coord[0][2]);
	my_pDC->MoveTo(coord[4][0], coord[4][2]);
	my_pDC->LineTo(coord[5][0], coord[5][2]);
	my_pDC->LineTo(coord[6][0], coord[6][2]);
	my_pDC->LineTo(coord[7][0], coord[7][2]);
	my_pDC->LineTo(coord[4][0], coord[4][2]);
	my_pDC->LineTo(coord[0][0], coord[0][2]);
	my_pDC->MoveTo(coord[1][0], coord[1][2]);
	my_pDC->LineTo(coord[5][0], coord[5][2]);
	my_pDC->MoveTo(coord[2][0], coord[2][2]);
	my_pDC->LineTo(coord[6][0], coord[6][2]);
	my_pDC->MoveTo(coord[3][0], coord[3][2]);
	my_pDC->LineTo(coord[7][0], coord[7][2]);
	my_pDC->SelectObject(pOldPen);
}

void CCGExperimentDlg::Draw3DDrawing(int coord[12][4])
{
	CPen penBlue(PS_SOLID, 2, RGB(0, 0, 255));
	CPen* pOldPen = my_pDC->SelectObject(&penBlue);
	my_pDC->MoveTo(coord[0][0], coord[0][1]);
	for (size_t i = 1; i < 4; i++)
		my_pDC->LineTo(coord[i][0], coord[i][1]);
	my_pDC->LineTo(coord[0][0], coord[0][1]);
	my_pDC->MoveTo(coord[4][0], coord[4][1]);
	for (size_t i = 5; i < 8; i++)
		my_pDC->LineTo(coord[i][0], coord[i][1]);
	my_pDC->LineTo(coord[4][0], coord[4][1]);
	my_pDC->MoveTo(coord[8][0], coord[8][1]);
	for (size_t i = 8; i < 12; i++)
		my_pDC->LineTo(coord[i][0], coord[i][1]);
	my_pDC->LineTo(coord[8][0], coord[8][1]);
	my_pDC->MoveTo(coord[1][0], coord[1][1]);
	my_pDC->LineTo(coord[9][0], coord[9][1]);
	my_pDC->MoveTo(coord[2][0], coord[2][1]);
	my_pDC->LineTo(coord[10][0], coord[10][1]);
	my_pDC->MoveTo(coord[5][0], coord[5][1]);
	my_pDC->LineTo(coord[8][0], coord[8][1]);
	my_pDC->MoveTo(coord[6][0], coord[6][1]);
	my_pDC->LineTo(coord[11][0], coord[11][1]);
	my_pDC->MoveTo(coord[0][0], coord[0][1]);
	my_pDC->LineTo(coord[4][0], coord[4][1]);
	my_pDC->MoveTo(coord[3][0], coord[3][1]);
	my_pDC->LineTo(coord[7][0], coord[7][1]);
}

void CCGExperimentDlg::DrawBezierCurve(int n, CPoint p[])
{
	CPen pen(PS_SOLID, 3, curColor);
	CPen* pOldPen = my_pDC->SelectObject(&pen);
	float t = 0;
	float* temp;
	temp = new float[n + 1];
	float* T;
	T = new float[n + 1];
	float x = 0;
	float y = 0;
	int F3[3][3] = {
			{1,-2,1},
			{-2,2,0},
			{1,0,0}
	};
	int F4[4][4] = {
		{-1,3,-3,1},
		{3,-6,3,0},
		{-3,3,0,0},
		{1,0,0,0}
	};
	int F5[5][5] = {
	{1,-4,6,-4,1},
	{-4,12,-12,4,0},
	{6,-12,6,0,0},
	{-4,4,0,0,0},
	{1,0,0,0,0}
	};
	switch (n)
	{
	case 2:
		for (t; t <= 1.0; t += 0.01)
		{
			for (size_t i = 0; i < n + 1; i++)
			{
				T[i] = pow(t, n - i);
			}
			temp[0] = T[0] * F3[0][0] + T[1] * F3[1][0] + T[2] * F3[2][0];
			temp[1] = T[0] * F3[0][1] + T[1] * F3[1][1] + T[2] * F3[2][1];
			temp[2] = T[0] * F3[0][2] + T[1] * F3[1][2] + T[2] * F3[2][2];
			x = temp[0] * p[0].x + temp[1] * p[1].x + temp[2] * p[2].x;
			y = temp[0] * p[0].y + temp[1] * p[1].y + temp[2] * p[2].y;
			my_pDC->MoveTo(x, y);
			my_pDC->LineTo(x, y);
		}
		break;
	case 3:
		for (t; t <= 1.0; t += 0.01)
		{
			for (size_t i = 0; i < n + 1; i++)
			{
				T[i] = pow(t, n - i);
			}
			temp[0] = T[0] * F4[0][0] + T[1] * F4[1][0] + T[2] * F4[2][0] + T[3] * F4[3][0];
			temp[1] = T[0] * F4[0][1] + T[1] * F4[1][1] + T[2] * F4[2][1] + T[3] * F4[3][1];
			temp[2] = T[0] * F4[0][2] + T[1] * F4[1][2] + T[2] * F4[2][2] + T[3] * F4[3][2];
			temp[3] = T[0] * F4[0][3] + T[1] * F4[1][3] + T[2] * F4[2][3] + T[3] * F4[3][3];
			x = temp[0] * p[0].x + temp[1] * p[1].x + temp[2] * p[2].x + temp[3] * p[3].x;
			y = temp[0] * p[0].y + temp[1] * p[1].y + temp[2] * p[2].y + temp[3] * p[3].y;
			my_pDC->MoveTo(x, y);
			my_pDC->LineTo(x, y);
		}
		break;
	case 4:
		for (t; t <= 1.0; t += 0.01)
		{
			for (size_t i = 0; i < n + 1; i++)
			{
				T[i] = pow(t, n - i);
			}
			temp[0] = T[0] * F5[0][0] + T[1] * F5[1][0] + T[2] * F5[2][0] + T[3] * F5[3][0] + T[4] * F5[4][0];
			temp[1] = T[0] * F5[0][1] + T[1] * F5[1][1] + T[2] * F5[2][1] + T[3] * F5[3][1] + T[4] * F5[4][1];
			temp[2] = T[0] * F5[0][2] + T[1] * F5[1][2] + T[2] * F5[2][2] + T[3] * F5[3][2] + T[4] * F5[4][2];
			temp[3] = T[0] * F5[0][3] + T[1] * F5[1][3] + T[2] * F5[2][3] + T[3] * F5[3][3] + T[4] * F5[4][3];
			temp[4] = T[0] * F5[0][4] + T[1] * F5[1][4] + T[2] * F5[2][4] + T[3] * F5[3][4] + T[4] * F5[4][4];
			x = temp[0] * p[0].x + temp[1] * p[1].x + temp[2] * p[2].x + temp[3] * p[3].x + temp[4] * p[4].x;
			y = temp[0] * p[0].y + temp[1] * p[1].y + temp[2] * p[2].y + temp[3] * p[3].y + temp[4] * p[4].y;
			my_pDC->MoveTo(x, y);
			my_pDC->LineTo(x, y);
		}
		break;
	default:
		break;
	}
	my_pDC->SelectObject(pOldPen);
}

void CCGExperimentDlg::DrawFourFixedPoints()
{
	CPen penBlackPoint(PS_SOLID, 8, RGB(0, 0, 0));
	CPen penBlackLine(PS_SOLID, 2, RGB(0, 0, 0));
	my_pDC->SelectObject(&penBlackPoint);
	CPoint prefabPoint[4] = {
		{400,400},{380,250},{550,250},{700,400}
	};
	for (size_t i = 0; i < 4; i++)
	{
		my_pDC->MoveTo(prefabPoint[i].x, prefabPoint[i].y);
		my_pDC->LineTo(prefabPoint[i].x, prefabPoint[i].y);
	}
	my_pDC->SelectObject(&penBlackLine);
	my_pDC->MoveTo(prefabPoint[0].x, prefabPoint[0].y);
	for (size_t i = 1; i < 4; i++)
		my_pDC->LineTo(prefabPoint[i].x, prefabPoint[i].y);
}

void CCGExperimentDlg::DrawThreeGreenPoints(CPoint greenPoints[3])
{
	CPen penGreenPoint(PS_SOLID, 8, RGB(0, 255, 0));
	CPen penGreenLine(PS_SOLID, 2, RGB(0, 255, 0));
	my_pDC->SelectObject(&penGreenPoint);
	for (size_t i = 0; i < 3; i++)
	{
		my_pDC->MoveTo(greenPoints[i].x, greenPoints[i].y);
		my_pDC->LineTo(greenPoints[i].x, greenPoints[i].y);
	}
	my_pDC->SelectObject(&penGreenLine);
	my_pDC->MoveTo(greenPoints[0].x, greenPoints[0].y);
	for (size_t i = 1; i < 3; i++)
		my_pDC->LineTo(greenPoints[i].x, greenPoints[i].y);
}

void CCGExperimentDlg::DrawTwoBluePoints(CPoint bluePoints[2])
{
	CPen penBluePoint(PS_SOLID, 8, RGB(0, 0, 255));
	CPen penBlueLine(PS_SOLID, 2, RGB(0, 0, 255));
	my_pDC->SelectObject(&penBluePoint);
	for (size_t i = 0; i < 2; i++)
	{
		my_pDC->MoveTo(bluePoints[i].x, bluePoints[i].y);
		my_pDC->LineTo(bluePoints[i].x, bluePoints[i].y);
	}
	my_pDC->SelectObject(&penBlueLine);
	my_pDC->MoveTo(bluePoints[0].x, bluePoints[0].y);
	my_pDC->LineTo(bluePoints[1].x, bluePoints[1].y);
}

void CCGExperimentDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		if (curT <= 1.0f)
		{
			curT += 0.005;
			CPoint greenPoints[3];
			greenPoints[0].x = 400 - 20 * curT;
			greenPoints[0].y = 400 - 150 * curT;
			greenPoints[1].x = 380 + 170 * curT;
			greenPoints[1].y = 250.0f;
			greenPoints[2].x = 550 + 150 * curT;
			greenPoints[2].y = 250 + 150 * curT;
			CPoint bluePoints[2];
			bluePoints[0].x = (1 - curT) * greenPoints[0].x + curT * greenPoints[1].x;
			bluePoints[0].y = (1 - curT) * greenPoints[0].y + curT * greenPoints[1].y;
			bluePoints[1].x = (1 - curT) * greenPoints[1].x + curT * greenPoints[2].x;
			bluePoints[1].y = (1 - curT) * greenPoints[1].y + curT * greenPoints[2].y;
			redPoint.x = (1 - curT) * bluePoints[0].x + curT * bluePoints[1].x;
			redPoint.y = (1 - curT) * bluePoints[0].y + curT * bluePoints[1].y;
			CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
			redPoints.push_back(redPoint);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_RECT)->ShowWindow(FALSE);
			DrawFourFixedPoints();
			DrawThreeGreenPoints(greenPoints);
			DrawTwoBluePoints(bluePoints);
			my_pDC->SelectObject(&penRed);
			for (size_t i = 0; i < redPoints.size(); i++)
			{
				my_pDC->MoveTo(redPoints[i].x, redPoints[i].y);
				my_pDC->LineTo(redPoints[i].x, redPoints[i].y);
			}
			if (curT >= 1.0f)
			{
				curT = 0.0f;
				redPoints.clear();
				KillTimer(1);
			}
		}
		break;
	case 2:
		if (_i <=  1000)
		{
			DrawEightFixedPoints();
			CPen penRed(PS_SOLID, 3, RGB(255, 0, 0));
			my_pDC->SelectObject(&penRed);
			for (size_t i = 0; i < _i+1; i++)
			{
				my_pDC->MoveTo(bsp[i].x, bsp[i].y);
				my_pDC->LineTo(bsp[i].x, bsp[i].y);
			}
		}
		_i += 1;
		if (_i > 1000)
		{
			_i = 0;
			KillTimer(2);
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CCGExperimentDlg::DrawBSampleCurve(int pointsNum, CPoint p[])
{
	CPen pen(PS_SOLID, 3, curColor);
	CPen* pOldPen = my_pDC->SelectObject(&pen);
	float t = 0;
	float _temp[4];
	float _T[4];
	float x = 0;
	float y = 0;
	int F[4][4] = {
		{-1,3,-3,1},
		{3,-6,3,0},
		{-3,0,3,0},
		{1,4,1,0}
	};
	
	for (t; t <= 1.0; t += 0.01)
	{
		for (size_t j = 0; j < pointsNum - 3; j++)
		{
			for (size_t i = j; i < j + 4; i++)
			{
				_T[i-j] = pow(t, 3 + j - i);
			}
			_temp[0] = (_T[0] * F[0][0] + _T[1] * F[1][0] + _T[2] * F[2][0] + _T[3] * F[3][0]) / 6.0;
			_temp[1] = (_T[0] * F[0][1] + _T[1] * F[1][1] + _T[2] * F[2][1] + _T[3] * F[3][1]) / 6.0;
			_temp[2] = (_T[0] * F[0][2] + _T[1] * F[1][2] + _T[2] * F[2][2] + _T[3] * F[3][2]) / 6.0;
			_temp[3] = (_T[0] * F[0][3] + _T[1] * F[1][3] + _T[2] * F[2][3] + _T[3] * F[3][3]) / 6.0;
			x = _temp[0] * p[0+j].x + _temp[1] * p[1+j].x + _temp[2] * p[2+j].x + _temp[3] * p[3+j].x;
			y = _temp[0] * p[0+j].y + _temp[1] * p[1+j].y + _temp[2] * p[2+j].y + _temp[3] * p[3+j].y;
			my_pDC->MoveTo(x, y);
			my_pDC->LineTo(x, y);
		}
	}
	my_pDC->SelectObject(pOldPen);
}

void CCGExperimentDlg::DrawEightFixedPoints()
{
	CPen penBlackPoint(PS_SOLID, 8, RGB(0, 0, 0));
	CPen penBlackLine(PS_SOLID, 2, RGB(0, 0, 0));
	my_pDC->SelectObject(&penBlackPoint);
	CPoint prefabPoints[8] = {
		{300,250},{450,80},{700,120},{850,280},{660,450},{420,370},{490,260},{630,230}
	};
	for (size_t i = 0; i < 8; i++)
	{
		my_pDC->MoveTo(prefabPoints[i].x, prefabPoints[i].y);
		my_pDC->LineTo(prefabPoints[i].x, prefabPoints[i].y);
	}
	my_pDC->SelectObject(&penBlackLine);
	my_pDC->MoveTo(prefabPoints[0].x, prefabPoints[0].y);
	for (size_t i = 1; i < 8; i++)
		my_pDC->LineTo(prefabPoints[i].x, prefabPoints[i].y);
}

void CCGExperimentDlg::DrawAuxiliaryLine(CPoint firstPoint, CPoint secondPoint, CPoint thirdPoint, CPoint importantPoint)
{
	CPen penBlue(PS_SOLID, 2, RGB(0, 0, 255));
	my_pDC->SelectObject(&penBlue);
	my_pDC->MoveTo(firstPoint.x, firstPoint.y);
	my_pDC->LineTo(thirdPoint.x, thirdPoint.y);
	my_pDC->MoveTo(secondPoint.x, secondPoint.y);
	my_pDC->LineTo(firstPoint.x + (thirdPoint.x - firstPoint.x) / 2, firstPoint.y + (thirdPoint.y - firstPoint.y) / 2);
	CPen penGreen(PS_SOLID, 5, RGB(0, 255, 0));
	my_pDC->SelectObject(&penGreen);
	//my_pDC->MoveTo(importantPoint.x, importantPoint.y);
	//my_pDC->LineTo(importantPoint.x, importantPoint.y);
}

float CCGExperimentDlg::bSampleBasis(int i, int k, float _curT)
{
	float alpha = 0, beta = 0, result = 0;
	if (k==0)
	{
		if ((nodeVector[i] <= _curT) & (_curT <= nodeVector[i + 1]))
			result = 1;
		else
			result = 0;
	}
	else
	{
		int length1 = nodeVector[i + k] - nodeVector[i];
		int length2 = nodeVector[i + k + 1] - nodeVector[i + 1];
		alpha = (length1 == 0 ? 0 : (_curT - nodeVector[i]) / length1);
		beta = (length2 == 0 ? 0 : (nodeVector[i + k + 1] - _curT) / length2);
		if (alpha==0 && beta!=0)
			result = beta * bSampleBasis(i + 1, k - 1, _curT);
		else  if (alpha != 0 && beta == 0)
			result = alpha * bSampleBasis(i, k - 1, _curT);
		else if (alpha != 0 && beta != 0)
		{
			result = alpha * bSampleBasis(i, k - 1, _curT) + beta * bSampleBasis(i + 1, k - 1, _curT);
		}
	}
	return result;
}

void CCGExperimentDlg::DrawBezierCurvedSurface(CPoint P[4][4])
{
	int M[4][4] =
	{
		{-1, 3, -3, 1},
		{3, -6, 3, 0},
		{-3, 3, 0, 0},
		{1, 0, 0, 0}
	};
	float U[4], V[4], u = 0.0f, v = 0.0f;
	float temp[4], tempX[4], tempY[4], _tempX[4], _tempY[4], posX = 0, posY = 0;
	CPen penBlue(PS_SOLID, 4, RGB(0, 0, 255));
	my_pDC->SelectObject(&penBlue);
	for (size_t m = 0; m <= 50; m++)
	{
		for (size_t i = 0; i < 4; i++)
			U[i] = pow(u, 3 - i);
		for (size_t n = 0; n <= 50; n++)
		{
			for (size_t i = 0; i < 4; i++)
				V[i] = pow(v, 3 - i);
			if (n == 0)
			{
				for (size_t i = 0; i < 4; i++)
					temp[i] = U[0] * M[0][i] + U[1] * M[1][i] + U[2] * M[2][i] + U[3] * M[3][i];
				for (size_t i = 0; i < 4; i++)
					tempX[i] = temp[0] * P[0][i].x + temp[1] * P[1][i].x + temp[2] * P[2][i].x + temp[3] * P[3][i].x;
				for (size_t i = 0; i < 4; i++)
					tempY[i] = temp[0] * P[0][i].y + temp[1] * P[1][i].y + temp[2] * P[2][i].y + temp[3] * P[3][i].y;
				for (size_t i = 0; i < 4; i++)
					_tempX[i] = tempX[0] * M[0][i] + tempX[1] * M[1][i] + tempX[2] * M[2][i] + tempX[3] * M[3][i];
				for (size_t i = 0; i < 4; i++)
					_tempY[i] = tempY[0] * M[0][i] + tempY[1] * M[1][i] + tempY[2] * M[2][i] + tempY[3] * M[3][i];
			}
			for (size_t i = 0; i < 4; i++)
			{
				posX += _tempX[i] * V[i];
				posY += _tempY[i] * V[i];
			}
			my_pDC->MoveTo(posX, posY);
			my_pDC->LineTo(posX, posY);
			posX = 0.0f;
			posY = 0.0f;
			v += 0.02f;
		}
		v = 0.0f;
		u += 0.02f;
	}
}

void CCGExperimentDlg::DrawBspCurvedSurface(CPoint P[4][4])
{
	float M[4][4] =
	{
		{-1/6.0f, 3/6.0f, -3/6.0f, 1/6.0f},
		{3/6.0f, -6/6.0f, 3/6.0f, 0/6.0f},
		{-3/6.0f, 0/6.0f, 3/6.0f, 0/6.0f},
		{1/6.0f, 4/6.0f, 1/6.0f, 0/6.0f}
	};
	float _M[4][4] =
	{
		{-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f},
		{3 / 6.0f, -6 / 6.0f, 0 / 6.0f, 4 / 6.0f},
		{-3 / 6.0f, 3 / 6.0f, 3 / 6.0f, 1 / 6.0f},
		{1 / 6.0f, 0 / 6.0f,  0 / 6.0f, 0 / 6.0f}
	};
	float U[4], V[4], u = 0.0f, v = 0.0f;
	float temp[4], tempX[4], tempY[4], _tempX[4], _tempY[4], posX = 0, posY = 0;
	CPen penBlue(PS_SOLID, 4, RGB(0, 0, 255));
	my_pDC->SelectObject(&penBlue);
	for (size_t m = 0; m <= 20; m++)
	{
		for (size_t i = 0; i < 4; i++)
			U[i] = pow(u, 3 - i);
		for (size_t n = 0; n <= 20; n++)
		{
			for (size_t i = 0; i < 4; i++)
				V[i] = pow(v, 3 - i);
			if (n == 0)
			{
				for (size_t i = 0; i < 4; i++)
					temp[i] = U[0] * M[0][i] + U[1] * M[1][i] + U[2] * M[2][i] + U[3] * M[3][i];
				for (size_t i = 0; i < 4; i++)
					tempX[i] = temp[0] * P[0][i].x + temp[1] * P[1][i].x + temp[2] * P[2][i].x + temp[3] * P[3][i].x;
				for (size_t i = 0; i < 4; i++)
					tempY[i] = temp[0] * P[0][i].y + temp[1] * P[1][i].y + temp[2] * P[2][i].y + temp[3] * P[3][i].y;
				for (size_t i = 0; i < 4; i++)
					_tempX[i] = tempX[0] * _M[0][i] + tempX[1] * _M[1][i] + tempX[2] * _M[2][i] + tempX[3] * _M[3][i];
				for (size_t i = 0; i < 4; i++)
					_tempY[i] = tempY[0] * _M[0][i] + tempY[1] * _M[1][i] + tempY[2] * _M[2][i] + tempY[3] * _M[3][i];
			}
			for (size_t i = 0; i < 4; i++)
			{
				posX += _tempX[i] * V[i];
				posY += _tempY[i] * V[i];
			}
			my_pDC->MoveTo(posX, posY);
			my_pDC->LineTo(posX, posY);
			posX = 0.0f;
			posY = 0.0f;
			v += 0.05f;
		}
		v = 0.0f;
		u += 0.05f;
	}
}




