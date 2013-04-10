// ArpSpoofDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ArpSpoof.h"
#include "ArpSpoofDlg.h"
#include "ArpSpoofLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CArpSpoofDlg 对话框




CArpSpoofDlg::CArpSpoofDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArpSpoofDlg::IDD, pParent)
	, szLocalMac(_T(""))
	, szHostMac(_T(""))
	, szGatewayIp(_T("192.168.1.1"))
	, szHostIp(_T("192.168.1.253"))
	, szGatewayMac(_T("74ea3a28971c"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CArpSpoofDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOCAL_MAC, szLocalMac);
	DDX_Text(pDX, IDC_EDIT_HOST_MAC, szHostMac);
	DDX_Text(pDX, IDC_EDIT_GATEWAY_IP, szGatewayIp);
	DDX_Text(pDX, IDC_EDIT_HOST_IP, szHostIp);
	DDX_Text(pDX, IDC_EDIT_GATEWAY_MAC, szGatewayMac);
	DDX_Control(pDX, IDC_CMB_INTERFACE_LIST, m_cmbInterfaceList);
}

BEGIN_MESSAGE_MAP(CArpSpoofDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CArpSpoofDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CArpSpoofDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_HOST, &CArpSpoofDlg::OnBnClickedButtonHost)
END_MESSAGE_MAP()


// CArpSpoofDlg 消息处理程序

BOOL CArpSpoofDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//初始化接口列表
	InitInterfaceList();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CArpSpoofDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CArpSpoofDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CArpSpoofDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CArpSpoofDlg::OnBnClickedOk()
{
	KillTimer(0);
	SetTimer(0, 3000, NULL);

	//OnOK();
}

void CArpSpoofDlg::OnBnClickedCancel()
{
	KillTimer(0);
	OnCancel();
}

int CArpSpoofDlg::InitInterfaceList()
{
	int nRet = 0;
	nRet = ListInterfaceInfomation(ListInterface, this);
	return nRet;
}

int CArpSpoofDlg::ListInterface(pcap_if_t * d, void * pPara)
{
	USES_CONVERSION;
	CArpSpoofDlg * pThis = (CArpSpoofDlg * )pPara;

	COMBOBOXEXITEM   Item;
	Item.mask = CBEIF_TEXT;
	Item.pszText = d->name;
	Item.iItem = pThis->m_cmbInterfaceList.GetCount(); //index 
	pThis->m_cmbInterfaceList.InsertItem(&Item);
	pThis->m_cmbInterfaceList.SetCurSel(0);
	TRACE(_T("%s\n"), d->name);

	return 0;
}

void CArpSpoofDlg::OnTimer(UINT_PTR nIDEvent)
{
	USES_CONVERSION;
	int nIndex = 0;

	COMBOBOXEXITEM Item;
	UpdateData();
	TRACE(szGatewayMac);
	CString szInterfaceName;
	memset(&Item, 0, sizeof(COMBOBOXEXITEM));
	Item.mask = CBEIF_TEXT;
	nIndex = m_cmbInterfaceList.GetCurSel();

	m_cmbInterfaceList.GetLBText(nIndex, szInterfaceName);

	TRACE(_T("%s\n"), szInterfaceName);

	ArpSpoof(T2A((LPTSTR)(LPCTSTR)szInterfaceName), T2A((LPTSTR)(LPCTSTR)szGatewayIp),
		T2A((LPTSTR)(LPCTSTR)szGatewayMac), T2A((LPTSTR)(LPCTSTR)szHostIp),
		T2A((LPTSTR)(LPCTSTR)szHostMac), T2A((LPTSTR)(LPCTSTR)szLocalMac));

	CDialog::OnTimer(nIDEvent);
}

void CArpSpoofDlg::OnBnClickedButtonHost()
{
	UpdateData();
	GetMac(szHostIp)
}
