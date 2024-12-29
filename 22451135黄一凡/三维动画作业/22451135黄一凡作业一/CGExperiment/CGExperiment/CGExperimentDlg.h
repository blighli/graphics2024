
// CGExperimentDlg.h: 头文件
//

#pragma once

#include "CMyPicCtrl.h"
#include<vector>
#include<algorithm>
#include<stack>
#include<cmath>
#include<stdio.h>
#include<stdlib.h>



// CCGExperimentDlg 对话框
class CCGExperimentDlg : public CDialogEx
{
// 构造
public:
	CCGExperimentDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CGEXPERIMENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
private:
	CTreeCtrl m_treeCtrl1;
	CMyPicCtrl m_picCtrl;
	CRect rect;
	CString cs;
	CString cs_parent;
	CString cs_grandparent;
	CPoint point_Down;
	CPoint point_Up;
	CDC * my_pDC;
	//csliderctrl m_sliderr;
	//csliderctrl m_sliderg;
	//csliderctrl m_sliderb;
	//int curRed;
	//int curGreen;
	//int curBlue;
	COLORREF curColor;
	int curLineWidth;
	std::vector<CPoint> points;
	CButton m_setColor;
	CPoint seedPoint;
	CPoint fivePoints[5] =
	{
		{300,300},
		{400,200},
		{500,250},
		{531,374},
		{574,237}
	};
	CPoint sixPoints[6] =
	{
		{300,300},
		{400,200},
		{500,250},
		{531,374},
		{574,237},
		{355,437}
	};
	int flag = 0;
	CPoint lastPoint;
	float T2d[3][3];
	int Tx, Ty;
	int left= 530, right= 670, top= 290, bottom= 390;
	CPoint myStartPoints[6];
	int pos[3];
	int xyFlag = 0;
	int flag3D = 0;
	CPoint lastPoint3D;
	int left3D = 400, right3D = 600, top3D = 400, bottom3D = 600;
	int(*finalCubeCoord)[4];
	float cubeCoord[8][4];
	float isometricProjection[4][4];
	bool xFlag = false;
	bool yFlag = false;
	bool zFlag = false;
	float T3d[4][4];
	float myCoord[12][4];
	int(*finalMyCoord)[4];
	CPoint* ctrlPoints;
	int n = 3;
	int clickedFlag = 0;
	int pointMove = 0;
	int seq, seq_i, seq_j;
	float curT = 0.0f;
	CPoint redPoint;
	std::vector<CPoint> redPoints;
	int pointsNum = 5;
	CPoint prefabPoints[8] = {
		{300,250},{450,80},{700,120},{850,280},{660,450},{420,370},{490,260},{630,230}
	};
	int _i = 0;
	CPoint bsp[1000];
	int nodeVector[12] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5 };
	CPoint P[4][4];

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	void DrawLine1(CPoint point1, CPoint point2, int color);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void DrawLine2(CPoint point1, CPoint point2, int color);
	void DrawLine3(CPoint point1, CPoint point2, int color);
	void DrawCircle1(CPoint point, int radius, int color);
	void DrawCircle2(CPoint point, int radius, int color);

//	afx_msg void OnNMCustomdrawSliderRed(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnTRBNThumbPosChangingSliderRed(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnNMReleasedcaptureSliderRed(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnNMReleasedcaptureSliderGreen(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnNMReleasedcaptureSliderBlue(NMHDR* pNMHDR, LRESULT* pResult);
	CButton m_clearBtn;
	afx_msg void OnBnClickedButton1();
	//afx_msg void sin_opengl();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnDestroy();
	void DrawGossip();
	void SmallYellowHuamn();
	void SmallCat();
	void SmallFox();
	void SeedFill_digui(CPoint point, int oldColor, int newColor);
	void SeedFill_stack(CPoint point, int oldColor, int newColor);
	void Polygonfill(std::vector<CPoint> points, int color);
	afx_msg void OnBnClickedSetcolor();
	void DrawLineByCPen(CPoint point1, CPoint point2, int color, int lineWidth);
	afx_msg void OnBnClickedSetlinewidth();
	void DrawCircleByCPen(CPoint point, int radius, int color, int lineWidth);
	void MySpecialName();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	int* MyMatrixMultiply(int a[3], float b[3][3]);
	int (*MyMatrixMultiply2(float a[][4], float b[][4]))[4];
	int (*MyMatrixMultiply3(float a[][4], float b[][4]))[4];
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	int EnCode(int x,int y,int XL,int XR,int YB,int YT);
	void LineClip(int x1,int y1,int x2,int y2,int XL,int XR,int YB,int YT,int code1,int code2);
	void Symmetrical(float T2d[][3],CPoint points[]);
	void DrawCube(int coord[8][4]);
	void Draw3DDrawing(int coord[12][4]);
	void DrawBezierCurve(int n, CPoint p[]);
	void DrawFourFixedPoints();
	void DrawThreeGreenPoints(CPoint greenPoints[3]);
	void DrawTwoBluePoints(CPoint bluePoints[2]);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void DrawBSampleCurve(int pointsNum, CPoint p[]);
	void DrawEightFixedPoints();
	void DrawAuxiliaryLine(CPoint firstPoint, CPoint secondPoint, CPoint thirdPoint, CPoint importantPoint);
	float bSampleBasis(int _i, int k, float _curT);
	void DrawBezierCurvedSurface(CPoint P[4][4]);
	void DrawBspCurvedSurface(CPoint P[4][4]);
};
