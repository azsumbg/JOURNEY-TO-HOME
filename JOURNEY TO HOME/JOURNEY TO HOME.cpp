#include "framework.h"
#include "JOURNEY TO HOME.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "D2BMPLOADER.h"
#include "gifresizer.h"
#include "FCheck.h"
#include "ErrH.h"
#include "space.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "space.lib")

constexpr wchar_t bWinClassName[]{ L"Meteors" };

constexpr char tmp_file[](".\\res\\data\\temp.dat");
constexpr wchar_t Ltmp_file[](L".\\res\\data\\temp.dat");
constexpr wchar_t record_file[](L".\\res\\data\\record.dat");
constexpr wchar_t save_file[](L".\\res\\data\\save.dat");
constexpr wchar_t help_file[](L".\\res\\data\\help.dat");
constexpr wchar_t sound_file[](L".\\res\\data\\main.wav");

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int no_record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int record{ 2003 };
WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HICON mainIcon{ nullptr };
HCURSOR mainCursor{ nullptr };
HCURSOR outCursor{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
PAINTSTRUCT bPaint{};
HDC PaintDC{ nullptr };
HWND bHwnd{ nullptr };
MSG bMsg{};
BOOL bRet{ 0 };
POINT cur_pos{};
UINT bTimer{ 0 };

D2D1_RECT_F b1Rect{ 0, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3, 0, scr_width - 50.0f, 50.0f };

D2D1_RECT_F b1TxtRect{ b1Rect.left + 40.0f, b1Rect.top + 10.0f, b1Rect.right, b1Rect.bottom };
D2D1_RECT_F b2TxtRect{ b2Rect.left + 35.0f, b2Rect.top + 10.0f, b2Rect.right, b2Rect.bottom };
D2D1_RECT_F b3TxtRect{ b3Rect.left + 30.0f, b3Rect.top + 10.0f, b3Rect.right, b3Rect.bottom };

bool pause = false;
bool sound = true;
bool show_help = false;
bool in_client = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool name_set = false;

wchar_t current_player[16]{ L"ONE CAPTAIN" };

int level = 1;
int score = 0;
int mins = 0;
int secs = 0;

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* StatusBckgBrush{ nullptr };
ID2D1SolidColorBrush* TxtBrush{ nullptr };
ID2D1SolidColorBrush* HgltBrush{ nullptr };
ID2D1SolidColorBrush* InactBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpBullet{ nullptr };

ID2D1Bitmap* bmpShipR[62]{ nullptr };
ID2D1Bitmap* bmpShipL[62]{ nullptr };

ID2D1Bitmap* bmpEarth[240]{ nullptr };
ID2D1Bitmap* bmpExplosion[24]{ nullptr };

ID2D1Bitmap* bmpField1[19]{ nullptr };
ID2D1Bitmap* bmpField2[90]{ nullptr };
ID2D1Bitmap* bmpField3[24]{ nullptr };

ID2D1Bitmap* bmpIntro[48]{ nullptr };

ID2D1Bitmap* bmpMeteor1[10]{ nullptr };
ID2D1Bitmap* bmpMeteor2[5]{ nullptr };
ID2D1Bitmap* bmpMeteor3[20]{ nullptr };
ID2D1Bitmap* bmpMeteor4[20]{ nullptr };
ID2D1Bitmap* bmpMeteor5[20]{ nullptr };

//////////////////////////////////////////////////

dll::RandIt RandEngine;
struct FIELDS
{
    dll::DLLObject Up{ nullptr };
    dll::DLLObject Left{ nullptr };
    dll::DLLObject Right{ nullptr };
    dll::DLLObject Down{ nullptr };
    dll::DLLObject Center{ nullptr };
} Field;
dirs field_dir = dirs::stop;


dll::DLLObject Ship{ nullptr };


/////////////////////////////////////////////////

template<typename T>concept HasRelease = requires (T check)
{
    check.Release();
};
template<HasRelease U> bool ClearHeap(U** var)
{
    if ((*var))
    {
        (*var)->Release();
        (*var) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
    err << what << L" Time of occurence: " << std::chrono::system_clock::now() << std::endl;
    err.close();
}
void ClearResources()
{
    if (!ClearHeap(&iFactory))LogError(L"Error releasing iFactory !");
    if (!ClearHeap(&Draw))LogError(L"Error releasing HwndRenderTarget !");
    if (!ClearHeap(&b1BckgBrush))LogError(L"Error releasing b1BckgBrush !");
    if (!ClearHeap(&b2BckgBrush))LogError(L"Error releasing b2BckgBrush !");
    if (!ClearHeap(&b3BckgBrush))LogError(L"Error releasing b3BckgBrush !");
    if (!ClearHeap(&StatusBckgBrush))LogError(L"Error releasing StatusBckgBrush !");
    if (!ClearHeap(&TxtBrush))LogError(L"Error releasing TxtBrush !");
    if (!ClearHeap(&HgltBrush))LogError(L"Error releasing HgltBrush !");
    if (!ClearHeap(&InactBrush))LogError(L"Error releasing InactBrush !");
    if (!ClearHeap(&iWriteFactory))LogError(L"Error releasing iWriteFactory !");
    if (!ClearHeap(&nrmFormat))LogError(L"Error releasing nrmFormat !");
    if (!ClearHeap(&midFormat))LogError(L"Error releasing midFormat !");
    if (!ClearHeap(&bigFormat))LogError(L"Error releasing bigFormat !");
    if (!ClearHeap(&bmpBullet))LogError(L"Error releasing bmpBullet !");

    for (int i = 0; i < 62; ++i)if (!ClearHeap(&bmpShipR[i]))LogError(L"Error releasing bmpShipR !");
    for (int i = 0; i < 62; ++i)if (!ClearHeap(&bmpShipL[i]))LogError(L"Error releasing bmpShipL !");

    for (int i = 0; i < 240; ++i)if (!ClearHeap(&bmpEarth[i]))LogError(L"Error releasing bmpEarth !");
    for (int i = 0; i < 24; ++i)if (!ClearHeap(&bmpExplosion[i]))LogError(L"Error releasing bmpExplosion !");

    for (int i = 0; i < 19; ++i)if (!ClearHeap(&bmpField1[i]))LogError(L"Error releasing bmpField1 !");
    for (int i = 0; i < 90; ++i)if (!ClearHeap(&bmpField2[i]))LogError(L"Error releasing bmpField2 !");
    for (int i = 0; i < 24; ++i)if (!ClearHeap(&bmpField3[i]))LogError(L"Error releasing bmpField3 !");

    for (int i = 0; i < 48; ++i)if (!ClearHeap(&bmpIntro[i]))LogError(L"Error releasing bmpIntro !");

    for (int i = 0; i < 10; ++i)if (!ClearHeap(&bmpMeteor1[i]))LogError(L"Error releasing bmpMeteor1 !");
    for (int i = 0; i < 5; ++i)if (!ClearHeap(&bmpMeteor2[i]))LogError(L"Error releasing bmpMeteor2 !");
    for (int i = 0; i < 20; ++i)if (!ClearHeap(&bmpMeteor3[i]))LogError(L"Error releasing bmpMeteor3 !");
    for (int i = 0; i < 20; ++i)if (!ClearHeap(&bmpMeteor4[i]))LogError(L"Error releasing bmpMeteor4 !");
    for (int i = 0; i < 20; ++i)if (!ClearHeap(&bmpMeteor5[i]))LogError(L"Error releasing bmpMeteor5 !");

}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBoxW(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    ClearResources();
    std::remove(tmp_file);
    exit(1);
}
void InitGame()
{
    level = 1;
    score = 0;
    mins = 0;
    secs = 180;

    wcscpy_s(current_player, L"ONE CAPTAIN");
    ////////////////////////////////////////////

    if (Field.Center)ClearHeap(&Field.Center);
    if (Field.Up)ClearHeap(&Field.Up);
    if (Field.Down)ClearHeap(&Field.Down);
    if (Field.Left)ClearHeap(&Field.Left);
    if (Field.Right)ClearHeap(&Field.Right);

    switch (RandEngine(0, 2))
    {
    case 0:
        Field.Center = dll::ObjectFactory(type_field1, 0, 50.0f, NULL, NULL);
        break;

    case 1:
        Field.Center = dll::ObjectFactory(type_field2, 0, 50.0f, NULL, NULL);
        break;

    case 2:
        Field.Center = dll::ObjectFactory(type_field3, 0, 50.0f, NULL, NULL);
        break;
    }
    switch (RandEngine(0, 2))
    {
    case 0:
        Field.Up = dll::ObjectFactory(type_field1, 0, -scr_height + 100.0f, NULL, NULL);
        break;

    case 1:
        Field.Up = dll::ObjectFactory(type_field2, 0, -scr_height + 100.0f, NULL, NULL);
        break;

    case 2:
        Field.Up = dll::ObjectFactory(type_field3, 0, -scr_height + 100.0f, NULL, NULL);
        break;
    }
    switch (RandEngine(0, 2))
    {
    case 0:
        Field.Down = dll::ObjectFactory(type_field1, 0, scr_height, NULL, NULL);
        break;

    case 1:
        Field.Down = dll::ObjectFactory(type_field2, 0, scr_height + 50.0f, NULL, NULL);
        break;

    case 2:
        Field.Down = dll::ObjectFactory(type_field3, 0, scr_height, NULL, NULL);
        break;
    }
    switch (RandEngine(0, 2))
    {
    case 0:
        Field.Left = dll::ObjectFactory(type_field1, -scr_width, 50.0f, NULL, NULL);
        break;

    case 1:
        Field.Left = dll::ObjectFactory(type_field2, -scr_width, 50.0f, NULL, NULL);
        break;

    case 2:
        Field.Left = dll::ObjectFactory(type_field3, -scr_width, 50.0f, NULL, NULL);
        break;
    }
    switch (RandEngine(0, 2))
    {
    case 0:
        Field.Right = dll::ObjectFactory(type_field1, scr_width, 50.0f, NULL, NULL);
        break;

    case 1:
        Field.Right = dll::ObjectFactory(type_field2, scr_width, 50.0f, NULL, NULL);
        break;

    case 2:
        Field.Right = dll::ObjectFactory(type_field3, scr_width, 50.0f, NULL, NULL);
        break;
    }

    field_dir = dirs::stop;
    
    ///////////////////////////////////////////

    ClearHeap(&Ship);
    Ship = dll::ObjectFactory(object_ship, 100.0f, (float)(RandEngine(50, 600)), NULL, NULL);

}

void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);




    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return true;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;
            
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16) < 1)
            {
                wcscpy_s(current_player, L"ONE CAPTAIN");
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името", L"Забраватор", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }
        break;
    }

    return(INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, bTimer, 1000, NULL);
        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();

        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");
        
        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mNew, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLvl, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mExit, L"Зала на славата");
        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако излезеш, губиш прогреса по тази игра !\n\nНаистина ли излизаш ?",
            L"Изход ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(20, 20, 20)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        secs--;
        mins = secs / 60;
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = true;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                        b3Hglt = true;
                    }
                }

                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                    b2Hglt = false;
                    b3Hglt = false;
                }
           
                SetCursor(mainCursor);
                return true;
            }
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако рестартираш, губиш прогреса по тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

            break;

        case mLvl:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако прескочиш нивото, губиш прогреса по това ниво !\n\nНаистина ли прескачаш нивото?",
                L"Следващо ниво ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            //LevelUp();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        }
        break;

    case WM_KEYDOWN:
        if (pause || !Ship)break;
        switch (wParam)
        {
        case VK_LEFT:
            Ship->dir = dirs::left;
            break;

        case VK_RIGHT:
            Ship->dir = dirs::right;
            break;

        case VK_UP:
            Ship->dir = dirs::up;
            break;

        case VK_DOWN:
            Ship->dir = dirs::down;
            break;
        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);

    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream start(Ltmp_file);
        start << L"Game started at: " << std::chrono::system_clock::now();
        start.close();
    }

    int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2));
    if (win_x + (int)(scr_width) > GetSystemMetrics(SM_CXSCREEN) || (int)(scr_height) + 20 > GetSystemMetrics(SM_CYSCREEN))
        ErrExit(eScreen);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFileW(L".\\res\\main.ani");
    outCursor = LoadCursorFromFileW(L".\\res\\out.ani");

    if (!mainCursor || !outCursor)ErrExit(eCursor);

    bWinClass.lpszClassName = bWinClassName;
    bWinClass.hInstance = bIns;
    bWinClass.lpfnWndProc = &WinProc;
    bWinClass.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));
    bWinClass.hCursor = mainCursor;
    bWinClass.hIcon = mainIcon;
    bWinClass.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWinClass))ErrExit(eClass);

    bHwnd = CreateWindow(bWinClassName, L"ДЪЛГИЯТ ПЪТ КЪМ ВКЪЩИ !", WS_CAPTION | WS_SYSMENU, win_x, 20,
        (int)(scr_width), (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1Factory !");
            ErrExit(eD2D);
        }

        if (iFactory)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
                D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1HwndRenderTarget !");
                ErrExit(eD2D);
            }

            if (Draw)
            {
                D2D1_GRADIENT_STOP gStops[2]{};
                ID2D1GradientStopCollection* gColl = nullptr;

                gStops[0].position = 0;
                gStops[0].color = D2D1::ColorF(D2D1::ColorF::PowderBlue);
                gStops[1].position = 1.0f;
                gStops[1].color = D2D1::ColorF(D2D1::ColorF::SandyBrown);

                hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
                if (hr != S_OK)
                {
                    LogError(L"Error ceating D2D1GradientStopCollection !");
                    ErrExit(eD2D);
                }

                if (gColl)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left +
                        (b1Rect.right - b1Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2, 25.0f),
                        gColl, &b1BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left +
                        (b2Rect.right - b2Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2, 25.0f),
                        gColl, &b2BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left +
                        (b3Rect.right - b3Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2, 25.0f),
                        gColl, &b3BckgBrush);
                
                    if (hr != S_OK)
                    {
                        LogError(L"Error creating D2D1RadialGradient Brushes !");
                        ErrExit(eD2D);
                    }

                    ClearHeap(&gColl);

                }

                Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Indigo), &TxtBrush);
                Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &HgltBrush);
                Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateGray), &InactBrush);
            
                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1SolidColor brushes !");
                    ErrExit(eD2D);
                }

                bmpBullet = Load(L".\\res\\img\\bullet.png", Draw);
                if (!bmpBullet)
                {
                    LogError(L"Error loading bmpBullet !");
                    ErrExit(eD2D);
                }

                
                for (int i = 0; i < 62; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\ship\\r\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpShipR[i] = Load(name, Draw);
                    if (!bmpShipR[i])
                    {
                        LogError(L"Error loading bmpShipR !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 62; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\ship\\l\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpShipL[i] = Load(name, Draw);

                    if (!bmpShipL[i])
                    {
                        LogError(L"Error loading bmpShipL !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 240; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\earth\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"000");
                    else if (i < 100)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpEarth[i] = Load(name, Draw);

                    if (!bmpEarth[i])
                    {
                        LogError(L"Error loading bmpEarth !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 24; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\explosion\\";
                    wchar_t add[10] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpExplosion[i] = Load(name, Draw);

                    if (!bmpExplosion[i])
                    {
                        LogError(L"Error loading bmpExplosion !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 48; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\intro\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpIntro[i] = Load(name, Draw);

                    if (!bmpIntro[i])
                    {
                        LogError(L"Error loading bmpIntro !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 19; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\field1\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpField1[i] = Load(name, Draw);

                    if (!bmpField1[i])
                    {
                        LogError(L"Error loading bmpField1 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 90; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\field2\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpField2[i] = Load(name, Draw);

                    if (!bmpField2[i])
                    {
                        LogError(L"Error loading bmpField2 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 24; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\field3\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpField3[i] = Load(name, Draw);

                    if (!bmpField3[i])
                    {
                        LogError(L"Error loading bmpField3 !");
                        ErrExit(eD2D);
                    }
                }

                for (int i = 0; i < 10; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\meteor1\\0";
                    wchar_t add[10] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpMeteor1[i] = Load(name, Draw);

                    if (!bmpMeteor1[i])
                    {
                        LogError(L"Error loading bmpMeteor1 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 5; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\meteor2\\";
                    wchar_t add[10] = L"\0";

                    wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpMeteor2[i] = Load(name, Draw);

                    if (!bmpMeteor2[i])
                    {
                        LogError(L"Error loading bmpMeteor2 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 20; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\meteor3\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpMeteor3[i] = Load(name, Draw);

                    if (!bmpMeteor3[i])
                    {
                        LogError(L"Error loading bmpMeteor3 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 20; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\meteor4\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpMeteor4[i] = Load(name, Draw);

                    if (!bmpMeteor4[i])
                    {
                        LogError(L"Error loading bmpMeteor4 !");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 20; ++i)
                {
                    wchar_t name[150] = L".\\res\\img\\meteor5\\";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpMeteor5[i] = Load(name, Draw);

                    if (!bmpMeteor5[i])
                    {
                        LogError(L"Error loading bmpMeteor5 !");
                        ErrExit(eD2D);
                    }
                }
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
            reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating D2D1WriteFactory !");
            ErrExit(eD2D);
        }

        if (iWriteFactory)
        {
            hr = iWriteFactory->CreateTextFormat(L"SEGOE PRINT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &nrmFormat);

            hr = iWriteFactory->CreateTextFormat(L"SEGOE PRINT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 48.0f, L"", &midFormat);

            hr = iWriteFactory->CreateTextFormat(L"SEGOE PRINT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigFormat);
            if (hr != S_OK)
            {
                LogError(L"Error creating D2D1WriteText Formats !");
                ErrExit(eD2D);
            }
        }

    }

    int intro_frame = 0;
    int intro_frame_delay = 2;

    if (Draw && bigFormat && HgltBrush)
    {
        mciSendString(L"play .\\res\\snd\\morse.wav", NULL, NULL, NULL);
        wchar_t start_txt[43] = L"ДЪЛГИЯТ ПЪТ КЪМ ВКЪЩИ !\n\n    dev. Daniel !";
        wchar_t show_txt[43] = L"\0";
        
        for (int i = 0; i < 43; ++i)
        {
            show_txt[i] = start_txt[i];

            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawTextW(show_txt, i, bigFormat, D2D1::RectF(50.0f, 100.0f, scr_width, scr_height), HgltBrush);
            intro_frame_delay--;
            if (intro_frame_delay <= 0)
            {
                intro_frame_delay = 2;
                ++intro_frame;
                if (intro_frame > 47)intro_frame = 0;
            }
            Draw->EndDraw();
            Sleep(100);
        }
        PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)
    {
        LogError(L"Error in Windows hInstance");
        ErrExit(eClass);
    }

    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessageW(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[0], D2D1::RectF(0, 0, scr_width, scr_height));
            Draw->DrawTextW(L"ПАУЗА", 6, bigFormat, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, scr_height),
                HgltBrush);
            Draw->EndDraw();
            continue;
        }

        //////////////////////////////////////////////////////////////////

        if (Field.Center && Field.Up && Field.Down && Field.Left && Field.Right && Ship)
        {
            switch (Ship->dir)
            {
            case dirs::stop:
                field_dir = dirs::stop;
                Field.Center->dir = dirs::stop;
                Field.Up->dir = dirs::stop;
                Field.Down->dir = dirs::stop;
                Field.Left->dir = dirs::stop;
                Field.Right->dir = dirs::stop;
                break;

            case dirs::left:
                field_dir = dirs::right;
                Field.Center->dir = dirs::right;
                Field.Up->dir = dirs::right;
                Field.Down->dir = dirs::right;
                Field.Left->dir = dirs::right;
                Field.Right->dir = dirs::right;
                break;

            case dirs::right:
                field_dir = dirs::left;
                Field.Center->dir = dirs::left;
                Field.Up->dir = dirs::left;
                Field.Down->dir = dirs::left;
                Field.Left->dir = dirs::left;
                Field.Right->dir = dirs::left;
                break;
   
            case dirs::up:
                field_dir = dirs::down;
                Field.Center->dir = dirs::down;
                Field.Up->dir = dirs::down;
                Field.Down->dir = dirs::down;
                Field.Left->dir = dirs::down;
                Field.Right->dir = dirs::down;
                break;
 
            case dirs::down:
                field_dir = dirs::up;
                Field.Center->dir = dirs::up;
                Field.Up->dir = dirs::up;
                Field.Down->dir = dirs::up;
                Field.Left->dir = dirs::up;
                Field.Right->dir = dirs::up;
                break;
            }

            if (field_dir == dirs::left)
            {
                Field.Left->Move(field_dir, (float)(level));
                Field.Right->Move(field_dir, (float)(level));
                Field.Center->Move(field_dir, (float)(level));
                
                if (Field.Center->end.x <= 0)
                {
                    ClearHeap(&Field.Left);
                    Field.Left = Field.Center;
                    Field.Center = Field.Right;
                    switch (RandEngine(0, 2))
                    {
                    case 0:
                        Field.Right = dll::ObjectFactory(type_field1, scr_width, 50.0f, NULL, NULL);
                        break;

                    case 1:
                        Field.Right = dll::ObjectFactory(type_field2, scr_width, 50.0f, NULL, NULL);
                        break;

                    case 2:
                        Field.Right = dll::ObjectFactory(type_field3, scr_width, 50.0f, NULL, NULL);
                        break;
                    }
                }
            }
            if (field_dir == dirs::right)
            {
                Field.Left->Move(field_dir, (float)(level));
                Field.Right->Move(field_dir, (float)(level));
                Field.Center->Move(field_dir, (float)(level));
                
                if (Field.Center->start.x >= scr_width)
                {
                    ClearHeap(&Field.Right);
                    Field.Right = Field.Center;
                    Field.Center = Field.Left;
                    switch (RandEngine(0, 2))
                    {
                    case 0:
                        Field.Left = dll::ObjectFactory(type_field1, -scr_width, 50.0f, NULL, NULL);
                        break;

                    case 1:
                        Field.Left = dll::ObjectFactory(type_field2, -scr_width, 50.0f, NULL, NULL);
                        break;

                    case 2:
                        Field.Left = dll::ObjectFactory(type_field3, -scr_width, 50.0f, NULL, NULL);
                        break;
                    }
                }
            }
            if (field_dir == dirs::up)
            {
                Field.Up->Move(field_dir, (float)(level));
                Field.Down->Move(field_dir, (float)(level));
                Field.Center->Move(field_dir, (float)(level));

                if (Field.Center->end.y <= 50.0f)
                {
                    ClearHeap(&Field.Up);
                    Field.Up = Field.Center;
                    Field.Center = Field.Down;
                    switch (RandEngine(0, 2))
                    {
                    case 0:
                        Field.Down = dll::ObjectFactory(type_field1, 0, scr_height, NULL, NULL);
                        break;

                    case 1:
                        Field.Down = dll::ObjectFactory(type_field2, 0, scr_height, NULL, NULL);
                        break;

                    case 2:
                        Field.Down = dll::ObjectFactory(type_field3, 0, scr_height, NULL, NULL);
                        break;
                    }
                }
            }
            if (field_dir == dirs::down)
            {
                Field.Up->Move(field_dir, (float)(level));
                Field.Down->Move(field_dir, (float)(level));
                Field.Center->Move(field_dir, (float)(level));

                if (Field.Center->start.y >= ground)
                {
                    ClearHeap(&Field.Down);
                    Field.Down = Field.Center;
                    Field.Center = Field.Up;
                    switch (RandEngine(0, 2))
                    {
                    case 0:
                        Field.Up = dll::ObjectFactory(type_field1, 0, -scr_height + 100.0f, NULL, NULL);
                        break;

                    case 1:
                        Field.Up = dll::ObjectFactory(type_field2, 0, -scr_height + 100.0f, NULL, NULL);
                        break;

                    case 2:
                        Field.Up = dll::ObjectFactory(type_field3, 0, -scr_height + 100.0f, NULL, NULL);
                        break;
                    }
                }
            }
        }

        if (Ship)Ship->Move(Ship->dir, (float)(level));

        if (Field.Center && Field.Up && Field.Down && Field.Left && Field.Right)
        {
            if (Field.Left->end.x < Field.Center->start.x)
            {
                Field.Left->start.x = Field.Center->start.x - scr_width;
                Field.Left->SetEdges();
            }
            if (Field.Right->start.x > Field.Center->end.x)
            {
                Field.Right->start.x = Field.Center->end.x;
                Field.Right->SetEdges();
            }
            if (Field.Up->end.y < Field.Center->start.y)
            {
                Field.Up->start.y = Field.Center->start.y - scr_height;
                Field.Up->SetEdges();
            }
            if (Field.Down->start.y > Field.Center->end.y)
            {
                Field.Down->start.y = Field.Center->end.y;
                Field.Down->SetEdges();
            }
        }







        // DRAW THINGS ***************************************************

        Draw->BeginDraw();

        if (Field.Center && Field.Up && Field.Down && Field.Left && Field.Right)
        {
            switch (Field.Center->GetType())
            {
            case type_field1:
                Draw->DrawBitmap(bmpField1[Field.Center->GetFrame()], D2D1::RectF(Field.Center->start.x,
                    Field.Center->start.y, Field.Center->end.x, Field.Center->end.y));
                break;

            case type_field2:
                Draw->DrawBitmap(bmpField2[Field.Center->GetFrame()], D2D1::RectF(Field.Center->start.x,
                    Field.Center->start.y, Field.Center->end.x, Field.Center->end.y));
                break;

            case type_field3:
                Draw->DrawBitmap(bmpField3[Field.Center->GetFrame()], D2D1::RectF(Field.Center->start.x,
                    Field.Center->start.y, Field.Center->end.x, Field.Center->end.y));
                break;
            }
            switch (Field.Up->GetType())
            {
            case type_field1:
                Draw->DrawBitmap(bmpField1[Field.Up->GetFrame()], D2D1::RectF(Field.Up->start.x,
                    Field.Up->start.y, Field.Up->end.x, Field.Up->end.y));
                break;

            case type_field2:
                Draw->DrawBitmap(bmpField2[Field.Up->GetFrame()], D2D1::RectF(Field.Up->start.x,
                    Field.Up->start.y, Field.Up->end.x, Field.Up->end.y));
                break;

            case type_field3:
                Draw->DrawBitmap(bmpField3[Field.Up->GetFrame()], D2D1::RectF(Field.Up->start.x,
                    Field.Up->start.y, Field.Up->end.x, Field.Up->end.y));
                break;
            }
            switch (Field.Down->GetType())
            {
            case type_field1:
                Draw->DrawBitmap(bmpField1[Field.Down->GetFrame()], D2D1::RectF(Field.Down->start.x,
                    Field.Down->start.y, Field.Down->end.x, Field.Down->end.y));
                break;

            case type_field2:
                Draw->DrawBitmap(bmpField2[Field.Down->GetFrame()], D2D1::RectF(Field.Down->start.x,
                    Field.Down->start.y, Field.Down->end.x, Field.Down->end.y));
                break;

            case type_field3:
                Draw->DrawBitmap(bmpField3[Field.Down->GetFrame()], D2D1::RectF(Field.Down->start.x,
                    Field.Down->start.y, Field.Down->end.x, Field.Down->end.y));
                break;
            }
            switch (Field.Left->GetType())
            {
            case type_field1:
                Draw->DrawBitmap(bmpField1[Field.Left->GetFrame()], D2D1::RectF(Field.Left->start.x,
                    Field.Left->start.y, Field.Left->end.x, Field.Left->end.y));
                break;

            case type_field2:
                Draw->DrawBitmap(bmpField2[Field.Left->GetFrame()], D2D1::RectF(Field.Left->start.x,
                    Field.Left->start.y, Field.Left->end.x, Field.Left->end.y));
                break;

            case type_field3:
                Draw->DrawBitmap(bmpField3[Field.Left->GetFrame()], D2D1::RectF(Field.Left->start.x,
                    Field.Left->start.y, Field.Left->end.x, Field.Left->end.y));
                break;
            }
            switch (Field.Right->GetType())
            {
            case type_field1:
                Draw->DrawBitmap(bmpField1[Field.Right->GetFrame()], D2D1::RectF(Field.Right->start.x,
                    Field.Right->start.y, Field.Right->end.x, Field.Right->end.y));
                break;

            case type_field2:
                Draw->DrawBitmap(bmpField2[Field.Right->GetFrame()], D2D1::RectF(Field.Right->start.x,
                    Field.Right->start.y, Field.Right->end.x, Field.Right->end.y));
                break;

            case type_field3:
                Draw->DrawBitmap(bmpField3[Field.Right->GetFrame()], D2D1::RectF(Field.Right->start.x,
                    Field.Right->start.y, Field.Right->end.x, Field.Right->end.y));
                break;
            }
        }
        
        if (TxtBrush && HgltBrush && InactBrush && nrmFormat && b1BckgBrush && b2BckgBrush && b3BckgBrush)
        {
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, 10.0f, 15.0f), b1BckgBrush);
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, 10.0f, 15.0f), b2BckgBrush);
            Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, 10.0f, 15.0f), b3BckgBrush);

            if (name_set) Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, InactBrush);
            else
            {
                if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, TxtBrush);
                else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, HgltBrush);
            }

            if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, TxtBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, HgltBrush);
        
            if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, TxtBrush);
            else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, HgltBrush);
        }

        

        if (Ship)
        {
            int curr_frame = Ship->GetFrame();
            
            switch (Ship->dir)
            {
            case dirs::right:
                Draw->DrawBitmap(bmpShipR[curr_frame], Resizer(bmpShipR[curr_frame], Ship->start.x, Ship->start.y));
                break;

            case dirs::left:
                Draw->DrawBitmap(bmpShipL[curr_frame], Resizer(bmpShipL[curr_frame], Ship->start.x, Ship->start.y));
                break;

            case dirs::up:
                if (Ship->end.x < scr_width / 2)
                    Draw->DrawBitmap(bmpShipR[curr_frame], Resizer(bmpShipR[curr_frame], Ship->start.x, Ship->start.y));
                else Draw->DrawBitmap(bmpShipL[curr_frame], Resizer(bmpShipL[curr_frame], Ship->start.x, Ship->start.y));
                break;

            case dirs::down:
                if (Ship->end.x < scr_width / 2)
                    Draw->DrawBitmap(bmpShipR[curr_frame], Resizer(bmpShipR[curr_frame], Ship->start.x, Ship->start.y));
                else Draw->DrawBitmap(bmpShipL[curr_frame], Resizer(bmpShipL[curr_frame], Ship->start.x, Ship->start.y));
                break;

            case dirs::stop:
                if (Ship->end.x < scr_width / 2)
                    Draw->DrawBitmap(bmpShipR[curr_frame], Resizer(bmpShipR[curr_frame], Ship->start.x, Ship->start.y));
                else Draw->DrawBitmap(bmpShipL[curr_frame], Resizer(bmpShipL[curr_frame], Ship->start.x, Ship->start.y));
                break;
            }
        }
        
        
        /////////////////////////////////////////////////////////////////

        Draw->EndDraw();
    }

    ClearResources();
    std::remove(tmp_file);

    return (int) bMsg.wParam;
}