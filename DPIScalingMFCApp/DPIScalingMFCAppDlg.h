
// DPIScalingMFCAppDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <map>;


// CDPIScalingMFCAppDlg dialog
class CDPIScalingMFCAppDlg : public CDialogEx
{
// Construction
public:
	CDPIScalingMFCAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DPISCALINGMFCAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

    //to store display info along with corresponding list item
    struct DisplayData {
        LUID m_adapterId;
        int m_targetID;
        int m_sourceID;
        //int currentDPI, minDPI, maxDPI, recommendedDPI;

        DisplayData()
        {
            m_adapterId = {};
            m_targetID = m_sourceID = -1;
            //currentDPI = minDPI = maxDPI = recommendedDPI = -1;
        }
    };
    std::map<int, DisplayData> m_displayDataCache;

	// Generated message map functions
	virtual BOOL OnInitDialog();
    bool FillDisplayInfo(LUID adapterID, int sourceID);
    bool Refresh();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnCbnSelchangeCombo1();
    // Drop down where we show displays
    CComboBox m_displayList;
    CEdit m_currentDPI;
    CEdit m_recommendedDPI;
    CListBox m_dpiList;
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton1();
};
