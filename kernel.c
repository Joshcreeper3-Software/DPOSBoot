// ===== COMPLETE STUBS for LLVM UEFI (no external libraries needed) =====
#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/WiFi.h>
#include <Protocol/WiFi2.h>

// Global tables
EFI_SYSTEM_TABLE *gST = NULL;
EFI_BOOT_SERVICES *gBS = NULL;
EFI_RUNTIME_SERVICES *gRT = NULL;

// Protocol GUIDs
EFI_GUID gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GUID gEfiSimplePointerProtocolGuid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
EFI_GUID gEfiAbsolutePointerProtocolGuid = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiWiFiProtocolGuid = EFI_WIRELESS_MAC_CONNECTION_PROTOCOL_GUID;
EFI_GUID gEfiWiFi2ProtocolGuid = EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL_GUID;

// Print implementation
UINTN Print(CONST CHAR16 *Format, ...) {
    if (gST && gST->ConOut) {
        return gST->ConOut->OutputString(gST->ConOut, (CHAR16*)Format);
    }
    return 0;
}

// SetMem implementation
VOID *SetMem(VOID *Buffer, UINTN Size, UINT8 Value) {
    UINT8 *Ptr = (UINT8*)Buffer;
    for (UINTN i = 0; i < Size; i++) {
        Ptr[i] = Value;
    }
    return Buffer;
}

// Add this function (around line where you have SetMem)
static VOID
ZeroMem(IN VOID *Buffer, IN UINTN Size) {
    UINT8 *Ptr = (UINT8*)Buffer;
    for (UINTN i = 0; i < Size; i++) {
        Ptr[i] = 0;
    }
}

// UnicodeSPrint implementation (simple version)
UINTN UnicodeSPrint(CHAR16 *Dest, UINTN DestSize, CONST CHAR16 *Format, ...) {
    if (!Dest || DestSize == 0) return 0;
    
    UINTN i = 0;
    while (Format[i] && i < DestSize - 1) {
        Dest[i] = Format[i];
        i++;
    }
    Dest[i] = L'\0';
    return i;
}

// InitializeLib implementation
EFI_STATUS InitializeLib(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    // Just set the global tables - they're already set
    return EFI_SUCCESS;
}

// Forward declaration of your main function
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);

// Entry point
EFI_STATUS EFIAPI efi_main(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gRT = SystemTable->RuntimeServices;
    
    // Call your main function
    return UefiMain(ImageHandle, SystemTable);
}
// ===== END OF STUBS =====

#define APP_COUNT 13
#define ICON_W 84
#define ICON_H 84
#define TITLE_H 28
#define BORDER_W 2
#define TASKBAR_H 46
#define KB_CURSOR_STEP 8
#define DOUBLE_CLICK_TICKS 24

#define NOTES_CAP 4096

#define PAINT_GRID_W 64
#define PAINT_GRID_H 40
#define PAINT_EMPTY 0xFF
#define PAINT_COLOR_COUNT 6

#define GLYPH_W 5
#define GLYPH_H 7

#define CALC_EXPR_CAP 96
#define CALC_RESULT_CAP 64

#define TERM_LINE_CAP 96
#define TERM_LINES 32

#define PHOTO_MAX 8
#define SNAKE_MAX 128
#define SNAKE_GRID_W 24
#define SNAKE_GRID_H 16
#define WIFI_SSID_CAP 32
#define WIFI_PASS_CAP 32
#define MAIL_MAX 10
#define USER_MAX 4
#define USER_NAME_CAP 16
#define PASS_CAP 20
#define SALT_LEN 8
#define STATE_MAGIC 0x504F5344u
#define STATE_VERSION 1u
#define STATE_FLAG_SETUP_DONE 0x1u

enum {
  APP_NOTES = 0,
  APP_PAINT = 1,
  APP_FILES = 2,
  APP_CALC = 3,
  APP_CLOCK = 4,
  APP_TERMINAL = 5,
  APP_SETTINGS = 6,
  APP_TASKS = 7,
  APP_PHOTO = 8,
  APP_SNAKE = 9,
  APP_MONITOR = 10,
  APP_BROWSER = 11,
  APP_MAIL = 12
};

typedef enum {
  UI_SETUP = 0,
  UI_LOGIN = 1,
  UI_DESKTOP = 2
} UI_MODE;

typedef struct {
  CHAR16 Name[USER_NAME_CAP];
  UINT8 Salt[SALT_LEN];
  UINT32 PassHash;
  UINT8 IsAdmin;
  UINT8 _Pad[3];
} USER_RECORD;

typedef struct {
  UINT32 Magic;
  UINT32 Version;
  UINT32 Flags;
  UINT32 UserCount;
  UINT32 ActiveUser;
  USER_RECORD Users[USER_MAX];
} OS_STATE;

typedef struct {
  UINTN X;
  UINTN Y;
  UINTN W;
  UINTN H;
} RECT;

typedef struct {
  CHAR16 *Name;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Accent;
  UINTN IconX;
  UINTN IconY;
  UINTN WinX;
  UINTN WinY;
  UINTN WinW;
  UINTN WinH;
  BOOLEAN Open;
} APP_WINDOW;

typedef struct {
  CHAR16 Ch;
  UINT8 Rows[GLYPH_H];
} GLYPH_DEF;

static EFI_GRAPHICS_OUTPUT_PROTOCOL *mGop;
static EFI_SIMPLE_POINTER_PROTOCOL *mSimplePointer;
static EFI_ABSOLUTE_POINTER_PROTOCOL *mAbsolutePointer;
static EFI_WIRELESS_MAC_CONNECTION_PROTOCOL *mWiFi;
static EFI_WIRELESS_MAC_CONNECTION_II_PROTOCOL *mWiFi2;
static UINTN mScreenW;
static UINTN mScreenH;
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL *mBackBuffer = NULL;
static BOOLEAN mUseBackBuffer = FALSE;

static UINTN mCursorX = 80;
static UINTN mCursorY = 80;
static UINTN mCursorSize = 12;
static BOOLEAN mPointerDown = FALSE;
static BOOLEAN mPrevPointerDown = FALSE;
static UINTN mTick = 0;
static UINTN mFrameDelayUs = 16666;
static UINTN mKeyboardCursorTicks = 0;

static INTN mFocusedApp = -1;
static INTN mSelectedIcon = -1;
static INTN mLastClickIcon = -1;
static UINTN mLastClickTick = 0;

static INTN mDraggingApp = -1;
static INTN mDragOffsetX = 0;
static INTN mDragOffsetY = 0;

static INTN mResizingApp = -1;
static UINTN mResizeStartCursorX = 0;
static UINTN mResizeStartCursorY = 0;
static UINTN mResizeStartW = 0;
static UINTN mResizeStartH = 0;

static BOOLEAN mPainting = FALSE;
static UINTN mPaintColorIndex = 0;
static UINT8 mPaintGrid[PAINT_GRID_H][PAINT_GRID_W];
static UINTN mPaintCellSize = 1;

static UINTN mZOrder[APP_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static APP_WINDOW mApps[APP_COUNT] = {
    {L"NOTEPAD", {0x55, 0x9E, 0xF3, 0}, 42, 58, 200, 112, 500, 360, FALSE},
    {L"PAINT", {0x72, 0xC4, 0x72, 0}, 42, 176, 240, 140, 560, 380, FALSE},
    {L"FILES", {0xEC, 0xA6, 0x55, 0}, 42, 294, 220, 170, 520, 340, FALSE},
    {L"CALCULATOR", {0xA8, 0x7F, 0xE8, 0}, 154, 58, 300, 160, 360, 250, FALSE},
    {L"CLOCK", {0x62, 0xD6, 0xD8, 0}, 154, 176, 360, 180, 340, 220, FALSE},
    {L"TERMINAL", {0x4F, 0xD4, 0x8A, 0}, 154, 294, 180, 120, 640, 360, FALSE},
    {L"SETTINGS", {0x84, 0x84, 0xE8, 0}, 266, 58, 420, 130, 540, 420, FALSE},
    {L"TASKS", {0xE8, 0x84, 0x84, 0}, 266, 176, 260, 160, 460, 320, FALSE},
    {L"PHOTO", {0x96, 0xD8, 0x7A, 0}, 266, 294, 280, 120, 560, 360, FALSE},
    {L"SNAKE", {0x48, 0xC8, 0x48, 0}, 378, 58, 300, 140, 500, 320, FALSE},
    {L"MONITOR", {0x90, 0x90, 0x90, 0}, 378, 176, 340, 160, 460, 280, FALSE},
    {L"BROWSER", {0xD8, 0xC0, 0x60, 0}, 378, 294, 260, 100, 620, 400, FALSE},
    {L"MAIL", {0xE0, 0x90, 0x90, 0}, 490, 58, 280, 130, 600, 360, FALSE},
};

static CHAR16 mNotesText[NOTES_CAP];
static UINTN mNotesLen = 0;
static INTN mFilesSelected = 0;
static CHAR16 mCalcExpr[CALC_EXPR_CAP];
static UINTN mCalcExprLen = 0;
static CHAR16 mCalcResult[CALC_RESULT_CAP] = L"READY";
static CHAR16 mTermLines[TERM_LINES][TERM_LINE_CAP];
static UINTN mTermLineCount = 0;
static CHAR16 mTermInput[TERM_LINE_CAP];
static UINTN mTermInputLen = 0;
static INTN mTaskSelected = APP_NOTES;
static UINT8 mPhotos[PHOTO_MAX][PAINT_GRID_H][PAINT_GRID_W];
static UINTN mPhotoCount = 0;
static UINTN mPhotoSelected = 0;
static RECT mPhotoPrevButton = {0, 0, 0, 0};
static RECT mPhotoNextButton = {0, 0, 0, 0};
static RECT mPhotoSnapButton = {0, 0, 0, 0};
static UINTN mSnakeLen = 0;
static INTN mSnakeDir = 1;
static INTN mSnakeNextDir = 1;
static INTN mSnakeX[SNAKE_MAX];
static INTN mSnakeY[SNAKE_MAX];
static INTN mSnakeFoodX = 6;
static INTN mSnakeFoodY = 6;
static BOOLEAN mSnakeAlive = FALSE;
static UINTN mSnakeScore = 0;
static UINTN mSnakeLastStepTick = 0;
static UINTN mSnakeStepTicks = 7;
static CHAR16 mWifiSsid[WIFI_SSID_CAP];
static CHAR16 mWifiPass[WIFI_PASS_CAP];
static BOOLEAN mWifiConnected = FALSE;
static CHAR16 mWifiStatus[64] = L"DISCONNECTED";
static INTN mSettingsEditField = 0;
static RECT mSettingsSsidRect = {0, 0, 0, 0};
static RECT mSettingsPassRect = {0, 0, 0, 0};
static RECT mSettingsConnectBtn = {0, 0, 0, 0};
static RECT mSettingsDisconnectBtn = {0, 0, 0, 0};
static INTN mBrowserPage = 0;
static RECT mBrowserPrevBtn = {0, 0, 0, 0};
static RECT mBrowserNextBtn = {0, 0, 0, 0};
static CHAR16 mMailItems[MAIL_MAX][64];
static UINTN mMailCount = 0;
static INTN mMailSelected = 0;
static UINTN mMailSyncCount = 0;
static RECT mMailSyncBtn = {0, 0, 0, 0};
static RECT mMailClearBtn = {0, 0, 0, 0};
static RECT mMailRows[MAIL_MAX];

static RECT mPaintPaletteRects[PAINT_COLOR_COUNT];
static RECT mPaintCanvasRect = {0, 0, 0, 0};
static RECT mFilesRows[3];
static RECT mFilesOpenButton = {0, 0, 0, 0};
static RECT mFilesClearButton = {0, 0, 0, 0};
static RECT mSettingsCursorMinus = {0, 0, 0, 0};
static RECT mSettingsCursorPlus = {0, 0, 0, 0};
static RECT mSettingsFpsMinus = {0, 0, 0, 0};
static RECT mSettingsFpsPlus = {0, 0, 0, 0};
static RECT mTaskRows[APP_COUNT];
static RECT mTasksOpenButton = {0, 0, 0, 0};
static RECT mTasksCloseButton = {0, 0, 0, 0};
static RECT mSettingsResetBtn = {0, 0, 0, 0};
static RECT mShutdownBtn = {0, 0, 0, 0};
static RECT mRebootBtn = {0, 0, 0, 0};
static RECT mSetupNextBtn = {0, 0, 0, 0};
static RECT mSetupBackBtn = {0, 0, 0, 0};
static RECT mSetupFieldRect = {0, 0, 0, 0};
static RECT mLoginOkBtn = {0, 0, 0, 0};
static RECT mLoginPassRect = {0, 0, 0, 0};
static RECT mLoginUserRects[USER_MAX];

static UI_MODE mUiMode = UI_DESKTOP;
static UINTN mSetupStep = 0;
static CHAR16 mSetupName[USER_NAME_CAP];
static UINTN mSetupNameLen = 0;
static CHAR16 mSetupPass[PASS_CAP];
static UINTN mSetupPassLen = 0;
static CHAR16 mSetupPassConfirm[PASS_CAP];
static UINTN mSetupPassConfirmLen = 0;
static CHAR16 mSetupStatus[64];

static INTN mLoginUserIndex = 0;
static CHAR16 mLoginPass[PASS_CAP];
static UINTN mLoginPassLen = 0;
static CHAR16 mLoginStatus[64];

static OS_STATE mState;

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL mPaintColors[PAINT_COLOR_COUNT] = {
    {0x10, 0x10, 0x10, 0}, {0x30, 0x30, 0xD8, 0}, {0x30, 0xB8, 0x30, 0},
    {0xD8, 0x30, 0x30, 0}, {0x40, 0xD8, 0xD8, 0}, {0xD8, 0x40, 0xD8, 0},
};

static CONST GLYPH_DEF mGlyphs[] = {
    {L'A', {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {L'B', {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}},
    {L'C', {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
    {L'D', {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}},
    {L'E', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}},
    {L'F', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}},
    {L'G', {0x0E, 0x11, 0x10, 0x13, 0x11, 0x11, 0x0E}},
    {L'H', {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {L'I', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}},
    {L'J', {0x1F, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E}},
    {L'K', {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}},
    {L'L', {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}},
    {L'M', {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}},
    {L'N', {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11}},
    {L'O', {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {L'P', {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}},
    {L'Q', {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}},
    {L'R', {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}},
    {L'S', {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}},
    {L'T', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {L'U', {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {L'V', {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}},
    {L'W', {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}},
    {L'X', {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}},
    {L'Y', {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}},
    {L'Z', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}},
    {L'0', {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}},
    {L'1', {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}},
    {L'2', {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}},
    {L'3', {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E}},
    {L'4', {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}},
    {L'5', {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E}},
    {L'6', {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E}},
    {L'7', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
    {L'8', {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}},
    {L'9', {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E}},
    {L' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {L'.', {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C}},
    {L',', {0x00, 0x00, 0x00, 0x00, 0x0C, 0x08, 0x10}},
    {L':', {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00}},
    {L';', {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x08, 0x10}},
    {L'!', {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}},
    {L'?', {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}},
    {L'-', {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}},
    {L'_', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F}},
    {L'/', {0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00}},
    {L'(', {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02}},
    {L')', {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08}},
};

static CONST UINT8 mFallbackGlyph[GLYPH_H] = {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04};

static UINTN
TaskbarHeight(void) {
  return (mScreenH > TASKBAR_H) ? TASKBAR_H : mScreenH;
}

static UINTN
ClampU(IN INTN Value, IN UINTN MinValue, IN UINTN MaxValue) {
  if (Value < (INTN)MinValue) {
    return MinValue;
  }
  if (Value > (INTN)MaxValue) {
    return MaxValue;
  }
  return (UINTN)Value;
}

static VOID
SetRect(IN OUT RECT *R, IN UINTN X, IN UINTN Y, IN UINTN W, IN UINTN H) {
  R->X = X;
  R->Y = Y;
  R->W = W;
  R->H = H;
}

static BOOLEAN
RectContains(IN CONST RECT *R, IN UINTN X, IN UINTN Y) {
  return (X >= R->X && X < R->X + R->W && Y >= R->Y && Y < R->Y + R->H);
}

static EFI_STATUS
InitBackBuffer(void) {
  EFI_STATUS status;
  UINTN pixels;

  pixels = mScreenW * mScreenH;
  status = gBS->AllocatePool(EfiBootServicesData,
                             pixels * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL),
                             (VOID **)&mBackBuffer);
  if (EFI_ERROR(status) || mBackBuffer == NULL) {
    mUseBackBuffer = FALSE;
    mBackBuffer = NULL;
    return EFI_OUT_OF_RESOURCES;
  }

  mUseBackBuffer = TRUE;
  return EFI_SUCCESS;
}

static VOID
FreeBackBuffer(void) {
  if (mBackBuffer != NULL) {
    gBS->FreePool(mBackBuffer);
    mBackBuffer = NULL;
  }
  mUseBackBuffer = FALSE;
}

static VOID
PresentBackBuffer(void) {
  if (!mUseBackBuffer || mBackBuffer == NULL) {
    return;
  }

  mGop->Blt(mGop, mBackBuffer, EfiBltBufferToVideo, 0, 0, 0, 0, mScreenW, mScreenH, 0);
}

static VOID
FillRect(IN UINTN X,
         IN UINTN Y,
         IN UINTN W,
         IN UINTN H,
         IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color) {
  UINTN yy;
  UINTN xx;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *row;

  if (W == 0 || H == 0 || X >= mScreenW || Y >= mScreenH) {
    return;
  }
  if (X + W > mScreenW) {
    W = mScreenW - X;
  }
  if (Y + H > mScreenH) {
    H = mScreenH - Y;
  }

  if (!mUseBackBuffer || mBackBuffer == NULL) {
    mGop->Blt(mGop, &Color, EfiBltVideoFill, 0, 0, X, Y, W, H, 0);
    return;
  }

  for (yy = 0; yy < H; ++yy) {
    row = &mBackBuffer[(Y + yy) * mScreenW + X];
    for (xx = 0; xx < W; ++xx) {
      row[xx] = Color;
    }
  }
}

static VOID
DrawBorder(IN UINTN X,
           IN UINTN Y,
           IN UINTN W,
           IN UINTN H,
           IN UINTN T,
           IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color) {
  if (W < T * 2 || H < T * 2) {
    return;
  }
  FillRect(X, Y, W, T, Color);
  FillRect(X, Y + H - T, W, T, Color);
  FillRect(X, Y, T, H, Color);
  FillRect(X + W - T, Y, T, H, Color);
}

static CONST UINT8 *
LookupGlyph(IN CHAR16 InCh) {
  UINTN i;
  CHAR16 ch;

  ch = InCh;
  if (ch >= L'a' && ch <= L'z') {
    ch = (CHAR16)(ch - (L'a' - L'A'));
  }

  for (i = 0; i < sizeof(mGlyphs) / sizeof(mGlyphs[0]); ++i) {
    if (mGlyphs[i].Ch == ch) {
      return mGlyphs[i].Rows;
    }
  }
  return mFallbackGlyph;
}

static VOID
DrawChar(IN UINTN X,
         IN UINTN Y,
         IN CHAR16 Ch,
         IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color,
         IN UINTN Scale) {
  CONST UINT8 *rows;
  UINTN r;
  UINTN c;
  UINT8 bit;

  rows = LookupGlyph(Ch);
  for (r = 0; r < GLYPH_H; ++r) {
    for (c = 0; c < GLYPH_W; ++c) {
      bit = (UINT8)(1U << (GLYPH_W - 1 - c));
      if ((rows[r] & bit) != 0) {
        FillRect(X + c * Scale, Y + r * Scale, Scale, Scale, Color);
      }
    }
  }
}

static UINTN
StrLen16(IN CONST CHAR16 *S) {
  UINTN n;
  n = 0;
  while (S[n] != 0) {
    n++;
  }
  return n;
}

static VOID
StrCopy16(IN OUT CHAR16 *Dest, IN UINTN DestCap, IN CONST CHAR16 *Src) {
  UINTN i;
  if (DestCap == 0) {
    return;
  }
  for (i = 0; i + 1 < DestCap && Src[i] != 0; ++i) {
    Dest[i] = Src[i];
  }
  Dest[i] = 0;
}

static VOID
ZeroState(void) {
  SetMem(&mState, sizeof(mState), 0);
  mState.Magic = STATE_MAGIC;
  mState.Version = STATE_VERSION;
}

static VOID
ClearLoginInput(void) {
  mLoginPassLen = 0;
  mLoginPass[0] = 0;
  mLoginStatus[0] = 0;
}

static VOID
ClearSetupInput(void) {
  mSetupStep = 0;
  mSetupNameLen = 0;
  mSetupPassLen = 0;
  mSetupPassConfirmLen = 0;
  mSetupName[0] = 0;
  mSetupPass[0] = 0;
  mSetupPassConfirm[0] = 0;
  mSetupStatus[0] = 0;
}

static UINT32
HashPassword(IN CONST CHAR16 *Pass, IN UINTN PassLen, IN CONST UINT8 *Salt) {
  UINT32 hash = 2166136261u;
  UINTN i;
  for (i = 0; i < SALT_LEN; ++i) {
    hash ^= Salt[i];
    hash *= 16777619u;
  }
  for (i = 0; i < PassLen; ++i) {
    UINT8 lo = (UINT8)(Pass[i] & 0xFF);
    UINT8 hi = (UINT8)((Pass[i] >> 8) & 0xFF);
    hash ^= lo;
    hash *= 16777619u;
    hash ^= hi;
    hash *= 16777619u;
  }
  return hash;
}

static VOID
FillSalt(OUT UINT8 *Salt) {
  EFI_TIME time;
  UINT32 seed = 0xA5C3u;
  UINTN i;
  if (!EFI_ERROR(gRT->GetTime(&time, NULL))) {
    seed ^= ((UINT32)time.Year << 16) ^ (time.Month << 8) ^ time.Day;
    seed ^= ((UINT32)time.Hour << 24) ^ (time.Minute << 16) ^ (time.Second << 8) ^ time.Nanosecond;
  }
  seed ^= (UINT32)mTick;
  for (i = 0; i < SALT_LEN; ++i) {
    seed = seed * 1664525u + 1013904223u;
    Salt[i] = (UINT8)(seed & 0xFF);
  }
}

static EFI_STATUS
OpenStateFile(IN BOOLEAN Create, OUT EFI_FILE_PROTOCOL **OutFile) {
  EFI_STATUS status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
  EFI_FILE_PROTOCOL *root;
  UINT64 mode;

  *OutFile = NULL;
  status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL, (VOID **)&fs);
  if (EFI_ERROR(status) || fs == NULL) {
    return EFI_NOT_FOUND;
  }
  status = fs->OpenVolume(fs, &root);
  if (EFI_ERROR(status) || root == NULL) {
    return EFI_NOT_FOUND;
  }
  mode = EFI_FILE_MODE_READ;
  if (Create) {
    mode |= EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE;
  }
  status = root->Open(root, OutFile, L"dpos\\state.bin", mode, 0);
  root->Close(root);
  return status;
}

static EFI_STATUS
LoadState(void) {
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file;
  UINTN size;

  ZeroState();
  status = OpenStateFile(FALSE, &file);
  if (EFI_ERROR(status) || file == NULL) {
    return status;
  }
  size = sizeof(mState);
  status = file->Read(file, &size, &mState);
  file->Close(file);
  if (EFI_ERROR(status) || size != sizeof(mState)) {
    ZeroState();
    return EFI_LOAD_ERROR;
  }
  if (mState.Magic != STATE_MAGIC || mState.Version != STATE_VERSION) {
    ZeroState();
    return EFI_COMPROMISED_DATA;
  }
  if (mState.UserCount > USER_MAX) {
    mState.UserCount = 0;
    mState.Flags = 0;
  }
  return EFI_SUCCESS;
}

static EFI_STATUS
SaveState(void) {
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file;
  UINTN size;

  status = OpenStateFile(TRUE, &file);
  if (EFI_ERROR(status) || file == NULL) {
    return status;
  }
  size = sizeof(mState);
  status = file->Write(file, &size, &mState);
  file->Flush(file);
  file->Close(file);
  return status;
}

static VOID
FactoryReset(void) {
  ZeroState();
  SaveState();
  ClearSetupInput();
  ClearLoginInput();
  mUiMode = UI_SETUP;
}

static VOID
ShutdownSystem(void) {
  if (gRT != NULL) {
    gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
  }
}

static VOID
RebootSystem(void) {
  if (gRT != NULL) {
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
}

static VOID
AppendChar16(IN OUT CHAR16 *Dest, IN UINTN DestCap, IN CHAR16 Ch) {
  UINTN len;
  if (DestCap < 2) {
    return;
  }
  len = StrLen16(Dest);
  if (len + 1 >= DestCap) {
    return;
  }
  Dest[len] = Ch;
  Dest[len + 1] = 0;
}

static VOID
AppendString16(IN OUT CHAR16 *Dest, IN UINTN DestCap, IN CONST CHAR16 *Src) {
  UINTN len;
  UINTN i;
  if (DestCap < 2) {
    return;
  }
  len = StrLen16(Dest);
  for (i = 0; len + 1 < DestCap && Src[i] != 0; ++i) {
    Dest[len++] = Src[i];
  }
  Dest[len] = 0;
}

static CHAR16
ToUpper16(IN CHAR16 Ch) {
  if (Ch >= L'a' && Ch <= L'z') {
    return (CHAR16)(Ch - (L'a' - L'A'));
  }
  return Ch;
}

static BOOLEAN
StrEqNoCase16(IN CONST CHAR16 *A, IN CONST CHAR16 *B) {
  UINTN i;
  i = 0;
  while (A[i] != 0 && B[i] != 0) {
    if (ToUpper16(A[i]) != ToUpper16(B[i])) {
      return FALSE;
    }
    i++;
  }
  return (A[i] == 0 && B[i] == 0);
}

static BOOLEAN
StrEq16(IN CONST CHAR16 *A, IN CONST CHAR16 *B) {
  UINTN i = 0;
  while (A[i] != 0 && B[i] != 0) {
    if (A[i] != B[i]) {
      return FALSE;
    }
    i++;
  }
  return (A[i] == 0 && B[i] == 0);
}

static BOOLEAN
StartsWithNoCase16(IN CONST CHAR16 *S, IN CONST CHAR16 *Prefix) {
  UINTN i;
  i = 0;
  while (Prefix[i] != 0) {
    if (S[i] == 0 || ToUpper16(S[i]) != ToUpper16(Prefix[i])) {
      return FALSE;
    }
    i++;
  }
  return TRUE;
}

static VOID
IntToStr16(IN INT64 Value, OUT CHAR16 *Out, IN UINTN OutCap) {
  CHAR16 tmp[32];
  UINTN n;
  UINTN i;
  UINT64 absValue;
  BOOLEAN neg;

  if (OutCap == 0) {
    return;
  }
  if (Value == 0) {
    if (OutCap >= 2) {
      Out[0] = L'0';
      Out[1] = 0;
    } else {
      Out[0] = 0;
    }
    return;
  }

  neg = (Value < 0);
  absValue = (UINT64)(neg ? -Value : Value);
  n = 0;
  while (absValue > 0 && n + 1 < sizeof(tmp) / sizeof(tmp[0])) {
    tmp[n++] = (CHAR16)(L'0' + (absValue % 10));
    absValue /= 10;
  }

  i = 0;
  if (neg && i + 1 < OutCap) {
    Out[i++] = L'-';
  }
  while (n > 0 && i + 1 < OutCap) {
    Out[i++] = tmp[--n];
  }
  Out[i] = 0;
}

static VOID
FormatTwoDigits(IN UINTN Value, OUT CHAR16 *Out) {
  Out[0] = (CHAR16)(L'0' + (Value / 10) % 10);
  Out[1] = (CHAR16)(L'0' + (Value % 10));
  Out[2] = 0;
}

static VOID
FormatUptime(IN UINTN Ticks, OUT CHAR16 *Out, IN UINTN OutCap) {
  UINTN totalSeconds;
  UINTN hours;
  UINTN minutes;
  UINTN seconds;
  CHAR16 num[32];
  CHAR16 two[3];

  if (OutCap == 0) {
    return;
  }
  Out[0] = 0;
  totalSeconds = Ticks / 60;
  hours = totalSeconds / 3600;
  minutes = (totalSeconds / 60) % 60;
  seconds = totalSeconds % 60;

  IntToStr16((INT64)hours, num, sizeof(num) / sizeof(num[0]));
  AppendString16(Out, OutCap, num);
  AppendChar16(Out, OutCap, L':');
  FormatTwoDigits(minutes, two);
  AppendString16(Out, OutCap, two);
  AppendChar16(Out, OutCap, L':');
  FormatTwoDigits(seconds, two);
  AppendString16(Out, OutCap, two);
}

static BOOLEAN
ParseCalcNumber(IN CONST CHAR16 *S, IN OUT UINTN *Pos, OUT INT64 *Out) {
  INT64 value;
  INT64 sign;
  UINTN i;
  BOOLEAN any;

  i = *Pos;
  while (S[i] == L' ') {
    i++;
  }

  sign = 1;
  if (S[i] == L'+') {
    i++;
  } else if (S[i] == L'-') {
    sign = -1;
    i++;
  }

  value = 0;
  any = FALSE;
  while (S[i] >= L'0' && S[i] <= L'9') {
    value = value * 10 + (S[i] - L'0');
    any = TRUE;
    i++;
  }
  if (!any) {
    return FALSE;
  }

  *Out = value * sign;
  *Pos = i;
  return TRUE;
}

static BOOLEAN
EvalCalcExpression(IN CONST CHAR16 *Expr, OUT INT64 *Result) {
  UINTN pos;
  INT64 total;
  INT64 term;
  INT64 value;
  CHAR16 addOp;
  CHAR16 op;

  pos = 0;
  if (!ParseCalcNumber(Expr, &pos, &term)) {
    return FALSE;
  }

  total = 0;
  addOp = L'+';
  while (TRUE) {
    while (Expr[pos] == L' ') {
      pos++;
    }
    if (Expr[pos] == 0) {
      break;
    }

    op = Expr[pos++];
    if (op != L'+' && op != L'-' && op != L'*' && op != L'/') {
      return FALSE;
    }
    if (!ParseCalcNumber(Expr, &pos, &value)) {
      return FALSE;
    }

    if (op == L'*') {
      term *= value;
    } else if (op == L'/') {
      if (value == 0) {
        return FALSE;
      }
      term /= value;
    } else {
      if (addOp == L'+') {
        total += term;
      } else {
        total -= term;
      }
      addOp = op;
      term = value;
    }
  }

  if (addOp == L'+') {
    total += term;
  } else {
    total -= term;
  }
  *Result = total;
  return TRUE;
}

static VOID
DrawTextLine(IN UINTN X,
             IN UINTN Y,
             IN CONST CHAR16 *S,
             IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color,
             IN UINTN Scale) {
  UINTN i;
  UINTN step;
  step = (GLYPH_W + 1) * Scale;
  for (i = 0; S[i] != 0; ++i) {
    DrawChar(X + i * step, Y, S[i], Color, Scale);
  }
}

static VOID
DrawCenteredLabel(IN UINTN CenterX,
                  IN UINTN Y,
                  IN CONST CHAR16 *S,
                  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color,
                  IN UINTN Scale) {
  UINTN len;
  UINTN w;
  UINTN x;

  len = StrLen16(S);
  w = len * (GLYPH_W + 1) * Scale;
  x = (CenterX > (w / 2)) ? (CenterX - (w / 2)) : 0;
  DrawTextLine(x, Y, S, Color, Scale);
}

static VOID
BringToFront(IN UINTN AppIndex) {
  UINTN i;
  UINTN pos;

  pos = 0;
  for (i = 0; i < APP_COUNT; ++i) {
    if (mZOrder[i] == AppIndex) {
      pos = i;
      break;
    }
  }
  for (i = pos; i + 1 < APP_COUNT; ++i) {
    mZOrder[i] = mZOrder[i + 1];
  }
  mZOrder[APP_COUNT - 1] = AppIndex;
}

static VOID
OpenApp(IN UINTN AppIndex) {
  mApps[AppIndex].Open = TRUE;
  mFocusedApp = (INTN)AppIndex;
  BringToFront(AppIndex);
}

static VOID
GetCloseRect(IN CONST APP_WINDOW *App, OUT RECT *R) {
  SetRect(R, App->WinX + App->WinW - 22, App->WinY + 6, 14, 14);
}

static VOID
GetResizeRect(IN CONST APP_WINDOW *App, OUT RECT *R) {
  SetRect(R, App->WinX + App->WinW - 16, App->WinY + App->WinH - 16, 12, 12);
}

static VOID
GetClientRect(IN CONST APP_WINDOW *App, OUT RECT *R) {
  SetRect(R, App->WinX + BORDER_W + 8, App->WinY + TITLE_H + 8,
          App->WinW - (BORDER_W * 2) - 16, App->WinH - TITLE_H - BORDER_W - 16);
}

static VOID
ResetPaint(void) {
  SetMem(mPaintGrid, sizeof(mPaintGrid), PAINT_EMPTY);
}

static VOID
ClearNotes(void) {
  mNotesLen = 0;
  mNotesText[0] = 0;
}

static VOID
NotesInsert(IN CHAR16 Ch) {
  if (mNotesLen + 1 >= NOTES_CAP) {
    return;
  }
  mNotesText[mNotesLen] = Ch;
  mNotesLen++;
  mNotesText[mNotesLen] = 0;
}

static VOID
HandleNotesKey(IN EFI_INPUT_KEY Key) {
  if (Key.UnicodeChar == 0) {
    return;
  }
  if (Key.UnicodeChar == 0x08) {
    if (mNotesLen > 0) {
      mNotesLen--;
      mNotesText[mNotesLen] = 0;
    }
    return;
  }
  if (Key.UnicodeChar == L'\r') {
    NotesInsert(L'\n');
    return;
  }
  if (Key.UnicodeChar == L'\t') {
    NotesInsert(L' ');
    NotesInsert(L' ');
    return;
  }
  if (Key.UnicodeChar >= 32 && Key.UnicodeChar <= 126) {
    NotesInsert(Key.UnicodeChar);
  }
}

static VOID
CalcSetResultText(IN CONST CHAR16 *Text) {
  StrCopy16(mCalcResult, CALC_RESULT_CAP, Text);
}

static VOID
CalcEvaluate(void) {
  INT64 result;
  CHAR16 num[32];

  if (mCalcExprLen == 0) {
    CalcSetResultText(L"READY");
    return;
  }
  if (!EvalCalcExpression(mCalcExpr, &result)) {
    CalcSetResultText(L"ERROR");
    return;
  }
  IntToStr16(result, num, sizeof(num) / sizeof(num[0]));
  CalcSetResultText(num);
}

static VOID
HandleCalcKey(IN EFI_INPUT_KEY Key) {
  CHAR16 ch;

  if (Key.UnicodeChar == 0) {
    return;
  }
  if (Key.UnicodeChar == 0x08) {
    if (mCalcExprLen > 0) {
      mCalcExprLen--;
      mCalcExpr[mCalcExprLen] = 0;
    }
    return;
  }
  if (Key.UnicodeChar == L'\r') {
    CalcEvaluate();
    return;
  }
  if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
    mCalcExprLen = 0;
    mCalcExpr[0] = 0;
    CalcSetResultText(L"READY");
    return;
  }

  ch = Key.UnicodeChar;
  if ((ch >= L'0' && ch <= L'9') || ch == L'+' || ch == L'-' || ch == L'*' || ch == L'/' ||
      ch == L' ') {
    if (mCalcExprLen + 1 < CALC_EXPR_CAP) {
      mCalcExpr[mCalcExprLen++] = ch;
      mCalcExpr[mCalcExprLen] = 0;
    }
  }
}

static VOID
TerminalClear(void) {
  UINTN i;
  for (i = 0; i < TERM_LINES; ++i) {
    mTermLines[i][0] = 0;
  }
  mTermLineCount = 0;
}

static VOID
TerminalPushLine(IN CONST CHAR16 *Line) {
  UINTN i;

  if (mTermLineCount < TERM_LINES) {
    StrCopy16(mTermLines[mTermLineCount], TERM_LINE_CAP, Line);
    mTermLineCount++;
    return;
  }

  for (i = 1; i < TERM_LINES; ++i) {
    StrCopy16(mTermLines[i - 1], TERM_LINE_CAP, mTermLines[i]);
  }
  StrCopy16(mTermLines[TERM_LINES - 1], TERM_LINE_CAP, Line);
}

static VOID
TerminalInit(void) {
  mTermInputLen = 0;
  mTermInput[0] = 0;
  TerminalClear();
  TerminalPushLine(L"TERM READY. TYPE HELP");
}

static VOID
TerminalRunCommand(void) {
  CHAR16 line[TERM_LINE_CAP];
  CHAR16 uptime[32];

  if (mTermInputLen == 0) {
    return;
  }

  line[0] = L'>';
  line[1] = L' ';
  line[2] = 0;
  AppendString16(line, TERM_LINE_CAP, mTermInput);
  TerminalPushLine(line);

  if (StrEqNoCase16(mTermInput, L"HELP")) {
    TerminalPushLine(L"HELP CLEAR TIME APPS ECHO <TEXT>");
  } else if (StrEqNoCase16(mTermInput, L"CLEAR")) {
    TerminalClear();
  } else if (StrEqNoCase16(mTermInput, L"TIME")) {
    line[0] = 0;
    AppendString16(line, TERM_LINE_CAP, L"UPTIME ");
    FormatUptime(mTick, uptime, sizeof(uptime) / sizeof(uptime[0]));
    AppendString16(line, TERM_LINE_CAP, uptime);
    TerminalPushLine(line);
  } else if (StrEqNoCase16(mTermInput, L"APPS")) {
    TerminalPushLine(L"NOTES PAINT FILES CALC CLOCK TERM");
  } else if (StartsWithNoCase16(mTermInput, L"ECHO ")) {
    TerminalPushLine(&mTermInput[5]);
  } else {
    TerminalPushLine(L"UNKNOWN COMMAND");
  }

  mTermInputLen = 0;
  mTermInput[0] = 0;
}

static VOID
HandleTerminalKey(IN EFI_INPUT_KEY Key) {
  CHAR16 ch;

  if (Key.UnicodeChar == 0) {
    return;
  }
  if (Key.UnicodeChar == 0x08) {
    if (mTermInputLen > 0) {
      mTermInputLen--;
      mTermInput[mTermInputLen] = 0;
    }
    return;
  }
  if (Key.UnicodeChar == L'\r') {
    TerminalRunCommand();
    return;
  }

  ch = Key.UnicodeChar;
  if (ch >= 32 && ch <= 126) {
    if (mTermInputLen + 1 < TERM_LINE_CAP) {
      mTermInput[mTermInputLen++] = ch;
      mTermInput[mTermInputLen] = 0;
    }
  }
}

static VOID
ReadPointerState(void) {
  EFI_STATUS status;
  EFI_SIMPLE_POINTER_STATE simpleState;
  EFI_ABSOLUTE_POINTER_STATE absState;
  BOOLEAN gotAny;
  BOOLEAN down;
  BOOLEAN absMoved;

  if (mKeyboardCursorTicks > 0) {
    mPointerDown = FALSE;
    return;
  }

  gotAny = FALSE;
  down = mPointerDown;
  absMoved = FALSE;

  if (mAbsolutePointer != NULL) {
    while (TRUE) {
      status = mAbsolutePointer->GetState(mAbsolutePointer, &absState);
      if (EFI_ERROR(status)) {
        break;
      }
      gotAny = TRUE;
      if (mAbsolutePointer->Mode->AbsoluteMaxX >
          mAbsolutePointer->Mode->AbsoluteMinX) {
        mCursorX = (UINTN)((absState.CurrentX - mAbsolutePointer->Mode->AbsoluteMinX) *
                           (mScreenW - 1) /
                           (mAbsolutePointer->Mode->AbsoluteMaxX -
                            mAbsolutePointer->Mode->AbsoluteMinX));
      }
      if (mAbsolutePointer->Mode->AbsoluteMaxY >
          mAbsolutePointer->Mode->AbsoluteMinY) {
        mCursorY = (UINTN)((absState.CurrentY - mAbsolutePointer->Mode->AbsoluteMinY) *
                           (mScreenH - 1) /
                           (mAbsolutePointer->Mode->AbsoluteMaxY -
                            mAbsolutePointer->Mode->AbsoluteMinY));
      }
      absMoved = TRUE;
      down = (absState.ActiveButtons != 0);
    }
  }

  if (mSimplePointer != NULL) {
    while (TRUE) {
      status = mSimplePointer->GetState(mSimplePointer, &simpleState);
      if (EFI_ERROR(status)) {
        break;
      }
      gotAny = TRUE;
      if (!absMoved) {
        mCursorX = ClampU((INTN)mCursorX + ((INTN)simpleState.RelativeMovementX / 8),
                          0, mScreenW - 1);
        mCursorY = ClampU((INTN)mCursorY + ((INTN)simpleState.RelativeMovementY / 8),
                          0, mScreenH - 1);
      }
      down = simpleState.LeftButton;
    }
  }

  if (gotAny) {
    mPointerDown = down;
  }
}

static VOID
PaintAtCursor(void) {
  INTN gx;
  INTN gy;

  if (mPaintCellSize == 0) {
    return;
  }
  if (!RectContains(&mPaintCanvasRect, mCursorX, mCursorY)) {
    return;
  }

  gx = (INTN)((mCursorX - mPaintCanvasRect.X) / mPaintCellSize);
  gy = (INTN)((mCursorY - mPaintCanvasRect.Y) / mPaintCellSize);
  if (gx < 0 || gx >= PAINT_GRID_W || gy < 0 || gy >= PAINT_GRID_H) {
    return;
  }
  mPaintGrid[gy][gx] = (UINT8)mPaintColorIndex;
}

static UINTN
CountPaintPixels(void) {
  UINTN x;
  UINTN y;
  UINTN count;

  count = 0;
  for (y = 0; y < PAINT_GRID_H; ++y) {
    for (x = 0; x < PAINT_GRID_W; ++x) {
      if (mPaintGrid[y][x] != PAINT_EMPTY) {
        count++;
      }
    }
  }
  return count;
}

static VOID
FilesOpenSelected(void) {
  if (mFilesSelected == 0) {
    OpenApp(APP_NOTES);
  } else if (mFilesSelected == 1) {
    OpenApp(APP_PAINT);
  } else {
    OpenApp(APP_NOTES);
    NotesInsert(L'[');
    NotesInsert(L'R');
    NotesInsert(L'E');
    NotesInsert(L'A');
    NotesInsert(L'D');
    NotesInsert(L'M');
    NotesInsert(L'E');
    NotesInsert(L']');
    NotesInsert(L' ');
    NotesInsert(L'U');
    NotesInsert(L'S');
    NotesInsert(L'E');
    NotesInsert(L' ');
    NotesInsert(L'D');
    NotesInsert(L'O');
    NotesInsert(L'U');
    NotesInsert(L'B');
    NotesInsert(L'L');
    NotesInsert(L'E');
    NotesInsert(L'-');
    NotesInsert(L'C');
    NotesInsert(L'L');
    NotesInsert(L'I');
    NotesInsert(L'C');
    NotesInsert(L'K');
    NotesInsert(L' ');
    NotesInsert(L'O');
    NotesInsert(L'N');
    NotesInsert(L' ');
    NotesInsert(L'I');
    NotesInsert(L'C');
    NotesInsert(L'O');
    NotesInsert(L'N');
    NotesInsert(L'S');
    NotesInsert(L'.');
    NotesInsert(L'\n');
  }
}

static VOID
FilesClearSelected(void) {
  if (mFilesSelected == 0) {
    ClearNotes();
  } else if (mFilesSelected == 1) {
    ResetPaint();
  }
}

static VOID
AdjustCursorSize(IN INTN Delta) {
  INTN value;
  value = (INTN)mCursorSize + Delta;
  if (value < 8) {
    value = 8;
  }
  if (value > 24) {
    value = 24;
  }
  mCursorSize = (UINTN)value;
}

static VOID
AdjustFrameDelay(IN INTN Delta) {
  INTN value;
  value = (INTN)mFrameDelayUs + Delta;
  if (value < 8000) {
    value = 8000;
  }
  if (value > 50000) {
    value = 50000;
  }
  mFrameDelayUs = (UINTN)value;
}

static VOID
TasksOpenSelected(void) {
  if (mTaskSelected >= 0 && mTaskSelected < (INTN)APP_COUNT) {
    OpenApp((UINTN)mTaskSelected);
  }
}

static VOID
TasksCloseSelected(void) {
  if (mTaskSelected >= 0 && mTaskSelected < (INTN)APP_COUNT) {
    mApps[mTaskSelected].Open = FALSE;
    if (mFocusedApp == mTaskSelected) {
      mFocusedApp = -1;
    }
  }
}

static VOID
PhotoCaptureFromPaint(void) {
  UINTN slot;
  UINTN x;
  UINTN y;

  if (mPhotoCount < PHOTO_MAX) {
    slot = mPhotoCount++;
  } else {
    for (slot = 1; slot < PHOTO_MAX; ++slot) {
      for (y = 0; y < PAINT_GRID_H; ++y) {
        for (x = 0; x < PAINT_GRID_W; ++x) {
          mPhotos[slot - 1][y][x] = mPhotos[slot][y][x];
        }
      }
    }
    slot = PHOTO_MAX - 1;
  }

  for (y = 0; y < PAINT_GRID_H; ++y) {
    for (x = 0; x < PAINT_GRID_W; ++x) {
      mPhotos[slot][y][x] = mPaintGrid[y][x];
    }
  }
  mPhotoSelected = slot;
}

static VOID
SnakeSpawnFood(void) {
  UINTN tries;
  UINTN i;
  INTN fx;
  INTN fy;
  BOOLEAN used;

  for (tries = 0; tries < 200; ++tries) {
    fx = (INTN)((mTick + tries * 7 + mSnakeScore * 3) % SNAKE_GRID_W);
    fy = (INTN)((mTick * 3 + tries * 11 + mSnakeScore * 5) % SNAKE_GRID_H);
    used = FALSE;
    for (i = 0; i < mSnakeLen; ++i) {
      if (mSnakeX[i] == fx && mSnakeY[i] == fy) {
        used = TRUE;
        break;
      }
    }
    if (!used) {
      mSnakeFoodX = fx;
      mSnakeFoodY = fy;
      return;
    }
  }
  mSnakeFoodX = 0;
  mSnakeFoodY = 0;
}

static VOID
SnakeReset(void) {
  mSnakeLen = 4;
  mSnakeX[0] = 8;
  mSnakeY[0] = 8;
  mSnakeX[1] = 7;
  mSnakeY[1] = 8;
  mSnakeX[2] = 6;
  mSnakeY[2] = 8;
  mSnakeX[3] = 5;
  mSnakeY[3] = 8;
  mSnakeDir = 1;
  mSnakeNextDir = 1;
  mSnakeAlive = TRUE;
  mSnakeScore = 0;
  mSnakeLastStepTick = mTick;
  SnakeSpawnFood();
}

static VOID
SnakeStep(void) {
  INTN nx;
  INTN ny;
  UINTN i;
  UINTN limit;

  if (!mSnakeAlive || mSnakeLen == 0) {
    return;
  }
  if (mTick - mSnakeLastStepTick < mSnakeStepTicks) {
    return;
  }
  mSnakeLastStepTick = mTick;
  mSnakeDir = mSnakeNextDir;

  nx = mSnakeX[0];
  ny = mSnakeY[0];
  if (mSnakeDir == 0) {
    ny--;
  } else if (mSnakeDir == 1) {
    nx++;
  } else if (mSnakeDir == 2) {
    ny++;
  } else {
    nx--;
  }

  if (nx < 0 || ny < 0 || nx >= SNAKE_GRID_W || ny >= SNAKE_GRID_H) {
    mSnakeAlive = FALSE;
    return;
  }

  limit = mSnakeLen;
  for (i = 0; i < limit; ++i) {
    if (mSnakeX[i] == nx && mSnakeY[i] == ny) {
      mSnakeAlive = FALSE;
      return;
    }
  }

  if (mSnakeLen + 1 < SNAKE_MAX && nx == mSnakeFoodX && ny == mSnakeFoodY) {
    for (i = mSnakeLen; i > 0; --i) {
      mSnakeX[i] = mSnakeX[i - 1];
      mSnakeY[i] = mSnakeY[i - 1];
    }
    mSnakeX[0] = nx;
    mSnakeY[0] = ny;
    mSnakeLen++;
    mSnakeScore++;
    SnakeSpawnFood();
  } else {
    for (i = mSnakeLen - 1; i > 0; --i) {
      mSnakeX[i] = mSnakeX[i - 1];
      mSnakeY[i] = mSnakeY[i - 1];
    }
    mSnakeX[0] = nx;
    mSnakeY[0] = ny;
  }
}

static VOID
WifiSetStatus(IN CONST CHAR16 *Status) {
  StrCopy16(mWifiStatus, sizeof(mWifiStatus) / sizeof(mWifiStatus[0]), Status);
}

static VOID
WifiInitProtocols(void) {
  EFI_STATUS status;

  status = gBS->LocateProtocol(&gEfiWiFi2ProtocolGuid, NULL, (VOID **)&mWiFi2);
  if (EFI_ERROR(status)) {
    mWiFi2 = NULL;
  }

  status = gBS->LocateProtocol(&gEfiWiFiProtocolGuid, NULL, (VOID **)&mWiFi);
  if (EFI_ERROR(status)) {
    mWiFi = NULL;
  }
}

static VOID
WifiConnect(void) {
    EFI_STATUS Status;
    UINTN i;
    
    if (StrLen16(mWifiSsid) == 0) {
        mWifiConnected = FALSE;
        StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), L"MISSING SSID");
        return;
    }

    // Try WiFi2 protocol first (newer)
    if (mWiFi2 != NULL) {
        EFI_80211_NETWORK Network;
        EFI_80211_CONNECT_NETWORK_DATA ConnectData;
        EFI_80211_CONNECT_NETWORK_TOKEN ConnectToken;
        
        ZeroMem(&Network, sizeof(Network));
        ZeroMem(&ConnectData, sizeof(ConnectData));
        ZeroMem(&ConnectToken, sizeof(ConnectToken));
        
        // Set up network info
        Network.BSSType = IeeeInfrastructureBSS;  // Fixed: IeeeInfrastructureBSS (capital SS)
        Network.SSId.SSIdLen = (UINT8)StrLen16(mWifiSsid);
        if (Network.SSId.SSIdLen > 32) Network.SSId.SSIdLen = 32;
        
        for (i = 0; i < Network.SSId.SSIdLen; i++) {
            Network.SSId.SSId[i] = (UINT8)mWifiSsid[i];
        }
        
        // Set security if password provided - FIXED: Password field might not exist
        // Some implementations use different structures - we'll skip for now
        // If your WiFi2.h doesn't have Password field, comment these lines:
        /*
        if (StrLen16(mWifiPass) > 0) {
            ConnectData.Password.Length = (UINT16)StrLen16(mWifiPass);
            ConnectData.Password.Data = (UINT8*)mWifiPass;
        }
        */
        
        ConnectData.Network = &Network;
        ConnectData.FailureTimeout = 15;  // seconds
        ConnectToken.Status = EFI_NOT_READY;
        ConnectToken.Data = &ConnectData;
        
        Status = mWiFi2->ConnectNetwork(mWiFi2, &ConnectToken);
        if (EFI_ERROR(Status)) {
            StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), 
                     L"CONNECT FAILED");
            return;
        }
        
        // Wait for connection (with timeout)
        for (i = 0; i < 300; i++) {  // 30 seconds max
            if (ConnectToken.Status != EFI_NOT_READY) {
                break;
            }
            gBS->Stall(100000);  // 100ms
        }
        
        if (!EFI_ERROR(ConnectToken.Status) && 
            ConnectToken.ResultCode == ConnectSuccess) {
            mWifiConnected = TRUE;
            StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), 
                     L"CONNECTED"); 
        } else {
            mWifiConnected = FALSE;
            StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), 
                     L"CONNECTION FAILED");
        }
        return;
    }
    
    // Try legacy WiFi protocol
    if (mWiFi != NULL) {
        // Legacy WiFi protocol implementation varies by platform
        StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), 
                 L"LEGACY WIFI - LIMITED");
        mWifiConnected = FALSE;  // Legacy may need different approach
        return;
    }
    
    StrCopy16(mWifiStatus, sizeof(mWifiStatus)/sizeof(CHAR16), 
             L"NO WIFI HARDWARE");
    mWifiConnected = FALSE;
}

static VOID
WifiDisconnect(void) {
  UINTN i;

  if (mWiFi2 != NULL) {
    EFI_80211_DISCONNECT_NETWORK_TOKEN token;
    EFI_STATUS status;

    SetMem(&token, sizeof(token), 0);
    token.Status = EFI_NOT_READY;
    status = mWiFi2->DisconnectNetwork(mWiFi2, &token);
    if (EFI_ERROR(status)) {
      mWifiConnected = FALSE;
      WifiSetStatus(L"DISCONNECT CALL FAILED");
      return;
    }
    for (i = 0; i < 60; ++i) {
      if (token.Status != EFI_NOT_READY) {
        break;
      }
      gBS->Stall(100000);
    }
  }
  
  mWifiConnected = FALSE;
  WifiSetStatus(L"DISCONNECTED");
}

static VOID SetupNextStep(void);
static VOID SetupPrevStep(void);
static VOID LoginAttempt(void);

static VOID
MailClear(void) {
  UINTN i;
  for (i = 0; i < MAIL_MAX; ++i) {
    mMailItems[i][0] = 0;
  }
  mMailCount = 0;
  mMailSelected = 0;
}

static VOID
MailPush(IN CONST CHAR16 *Msg) {
  UINTN i;
  if (mMailCount < MAIL_MAX) {
    StrCopy16(mMailItems[mMailCount], 64, Msg);
    mMailCount++;
    return;
  }
  for (i = 1; i < MAIL_MAX; ++i) {
    StrCopy16(mMailItems[i - 1], 64, mMailItems[i]);
  }
  StrCopy16(mMailItems[MAIL_MAX - 1], 64, Msg);
}

static VOID
MailSyncStub(void) {
  CHAR16 msg[64];
  CHAR16 num[16];

  if (!mWifiConnected) {
    MailPush(L"SYNC FAILED: OFFLINE");
    return;
  }
  mMailSyncCount++;
  msg[0] = 0;
  AppendString16(msg, 64, L"MAIL #");
  IntToStr16((INT64)mMailSyncCount, num, sizeof(num) / sizeof(num[0]));
  AppendString16(msg, 64, num);
  AppendString16(msg, 64, L" FROM NET");
  MailPush(msg);
}

static VOID
HandlePointerDownAction(void) {
  INTN z;
  UINTN idx;
  RECT closeRect;
  RECT resizeRect;
  RECT titleRect;
  RECT fullRect;
  RECT iconRect;
  UINTN p;
  UINTN r;

  if (mUiMode == UI_SETUP) {
    if (RectContains(&mSetupNextBtn, mCursorX, mCursorY)) {
      SetupNextStep();
      return;
    }
    if (RectContains(&mSetupBackBtn, mCursorX, mCursorY)) {
      SetupPrevStep();
      return;
    }
    return;
  }

  if (mUiMode == UI_LOGIN) {
    for (r = 0; r < mState.UserCount && r < USER_MAX; ++r) {
      if (RectContains(&mLoginUserRects[r], mCursorX, mCursorY)) {
        mLoginUserIndex = (INTN)r;
        mLoginStatus[0] = 0;
        return;
      }
    }
    if (RectContains(&mLoginOkBtn, mCursorX, mCursorY)) {
      LoginAttempt();
      return;
    }
    return;
  }

  for (z = APP_COUNT - 1; z >= 0; --z) {
    idx = mZOrder[z];
    if (!mApps[idx].Open) {
      continue;
    }

    GetCloseRect(&mApps[idx], &closeRect);
    if (RectContains(&closeRect, mCursorX, mCursorY)) {
      mApps[idx].Open = FALSE;
      if (mFocusedApp == (INTN)idx) {
        mFocusedApp = -1;
      }
      mDraggingApp = -1;
      mResizingApp = -1;
      return;
    }

    GetResizeRect(&mApps[idx], &resizeRect);
    if (RectContains(&resizeRect, mCursorX, mCursorY)) {
      BringToFront(idx);
      mFocusedApp = (INTN)idx;
      mResizingApp = (INTN)idx;
      mResizeStartCursorX = mCursorX;
      mResizeStartCursorY = mCursorY;
      mResizeStartW = mApps[idx].WinW;
      mResizeStartH = mApps[idx].WinH;
      mDraggingApp = -1;
      return;
    }

    SetRect(&titleRect, mApps[idx].WinX, mApps[idx].WinY, mApps[idx].WinW, TITLE_H);
    if (RectContains(&titleRect, mCursorX, mCursorY)) {
      BringToFront(idx);
      mFocusedApp = (INTN)idx;
      mDraggingApp = (INTN)idx;
      mDragOffsetX = (INTN)mCursorX - (INTN)mApps[idx].WinX;
      mDragOffsetY = (INTN)mCursorY - (INTN)mApps[idx].WinY;
      mResizingApp = -1;
      return;
    }

    SetRect(&fullRect, mApps[idx].WinX, mApps[idx].WinY, mApps[idx].WinW, mApps[idx].WinH);
    if (RectContains(&fullRect, mCursorX, mCursorY)) {
      BringToFront(idx);
      mFocusedApp = (INTN)idx;
      mDraggingApp = -1;
      mResizingApp = -1;

      if (idx == APP_PAINT) {
        for (p = 0; p < PAINT_COLOR_COUNT; ++p) {
          if (RectContains(&mPaintPaletteRects[p], mCursorX, mCursorY)) {
            mPaintColorIndex = p;
            return;
          }
        }
        if (RectContains(&mPaintCanvasRect, mCursorX, mCursorY)) {
          mPainting = TRUE;
          PaintAtCursor();
          return;
        }
      } else if (idx == APP_FILES) {
        for (r = 0; r < 3; ++r) {
          if (RectContains(&mFilesRows[r], mCursorX, mCursorY)) {
            mFilesSelected = (INTN)r;
            return;
          }
        }
        if (RectContains(&mFilesOpenButton, mCursorX, mCursorY)) {
          FilesOpenSelected();
          return;
        }
        if (RectContains(&mFilesClearButton, mCursorX, mCursorY)) {
          FilesClearSelected();
          return;
        }
      } else if (idx == APP_SETTINGS) {
        if (RectContains(&mSettingsCursorMinus, mCursorX, mCursorY)) {
          AdjustCursorSize(-2);
          return;
        }
        if (RectContains(&mSettingsCursorPlus, mCursorX, mCursorY)) {
          AdjustCursorSize(2);
          return;
        }
        if (RectContains(&mSettingsFpsMinus, mCursorX, mCursorY)) {
          AdjustFrameDelay(2000);
          return;
        }
        if (RectContains(&mSettingsFpsPlus, mCursorX, mCursorY)) {
          AdjustFrameDelay(-2000);
          return;
        }
        if (RectContains(&mSettingsSsidRect, mCursorX, mCursorY)) {
          mSettingsEditField = 1;
          return;
        }
        if (RectContains(&mSettingsPassRect, mCursorX, mCursorY)) {
          mSettingsEditField = 2;
          return;
        }
        if (RectContains(&mSettingsConnectBtn, mCursorX, mCursorY)) {
          WifiConnect();
          return;
        }
        if (RectContains(&mSettingsDisconnectBtn, mCursorX, mCursorY)) {
          WifiDisconnect();
          return;
        }
        if (RectContains(&mSettingsResetBtn, mCursorX, mCursorY)) {
          FactoryReset();
          return;
        }
      } else if (idx == APP_TASKS) {
        for (r = 0; r < APP_COUNT; ++r) {
          if (RectContains(&mTaskRows[r], mCursorX, mCursorY)) {
            mTaskSelected = (INTN)r;
            return;
          }
        }
        if (RectContains(&mTasksOpenButton, mCursorX, mCursorY)) {
          TasksOpenSelected();
          return;
        }
        if (RectContains(&mTasksCloseButton, mCursorX, mCursorY)) {
          TasksCloseSelected();
          return;
        }
      } else if (idx == APP_PHOTO) {
        if (RectContains(&mPhotoPrevButton, mCursorX, mCursorY)) {
          if (mPhotoCount > 0) {
            mPhotoSelected = (mPhotoSelected == 0) ? (mPhotoCount - 1) : (mPhotoSelected - 1);
          }
          return;
        }
        if (RectContains(&mPhotoNextButton, mCursorX, mCursorY)) {
          if (mPhotoCount > 0) {
            mPhotoSelected = (mPhotoSelected + 1) % mPhotoCount;
          }
          return;
        }
        if (RectContains(&mPhotoSnapButton, mCursorX, mCursorY)) {
          PhotoCaptureFromPaint();
          return;
        }
      } else if (idx == APP_SNAKE) {
        if (!mSnakeAlive) {
          SnakeReset();
        }
        return;
      } else if (idx == APP_BROWSER) {
        if (RectContains(&mBrowserPrevBtn, mCursorX, mCursorY)) {
          mBrowserPage = (mBrowserPage + 2) % 3;
          return;
        }
        if (RectContains(&mBrowserNextBtn, mCursorX, mCursorY)) {
          mBrowserPage = (mBrowserPage + 1) % 3;
          return;
        }
      } else if (idx == APP_MAIL) {
        for (r = 0; r < MAIL_MAX; ++r) {
          if (RectContains(&mMailRows[r], mCursorX, mCursorY)) {
            mMailSelected = (INTN)r;
            return;
          }
        }
        if (RectContains(&mMailSyncBtn, mCursorX, mCursorY)) {
          MailSyncStub();
          return;
        }
        if (RectContains(&mMailClearBtn, mCursorX, mCursorY)) {
          MailClear();
          return;
        }
      }
      return;
    }
  }

  if (RectContains(&mShutdownBtn, mCursorX, mCursorY)) {
    ShutdownSystem();
    return;
  }
  if (RectContains(&mRebootBtn, mCursorX, mCursorY)) {
    RebootSystem();
    return;
  }

  for (idx = 0; idx < APP_COUNT; ++idx) {
    SetRect(&iconRect, mApps[idx].IconX, mApps[idx].IconY, ICON_W, ICON_H + 20);
    if (RectContains(&iconRect, mCursorX, mCursorY)) {
      if (mSelectedIcon == (INTN)idx && mLastClickIcon == (INTN)idx &&
          (mTick - mLastClickTick) <= DOUBLE_CLICK_TICKS) {
        OpenApp(idx);
      } else {
        mSelectedIcon = (INTN)idx;
        mFocusedApp = -1;
      }
      mLastClickIcon = (INTN)idx;
      mLastClickTick = mTick;
      return;
    }
  }

  mSelectedIcon = -1;
  mFocusedApp = -1;
}

static VOID
HandlePointerHold(void) {
  APP_WINDOW *App;
  INTN newX;
  INTN newY;
  INTN newW;
  INTN newH;
  INTN deltaX;
  INTN deltaY;
  UINTN maxH;
  UINTN taskH;

  taskH = TaskbarHeight();

  if (mDraggingApp >= 0 && mDraggingApp < (INTN)APP_COUNT) {
    App = &mApps[mDraggingApp];
    newX = (INTN)mCursorX - mDragOffsetX;
    newY = (INTN)mCursorY - mDragOffsetY;
    if (newX < 0) {
      newX = 0;
    }
    if (newY < 0) {
      newY = 0;
    }
    if ((UINTN)newX + App->WinW > mScreenW) {
      newX = (INTN)(mScreenW - App->WinW);
    }
    if ((UINTN)newY + App->WinH > mScreenH - taskH) {
      newY = (INTN)(mScreenH - taskH - App->WinH);
    }
    App->WinX = (UINTN)newX;
    App->WinY = (UINTN)newY;
    return;
  }

  if (mResizingApp >= 0 && mResizingApp < (INTN)APP_COUNT) {
    App = &mApps[mResizingApp];
    deltaX = (INTN)mCursorX - (INTN)mResizeStartCursorX;
    deltaY = (INTN)mCursorY - (INTN)mResizeStartCursorY;
    newW = (INTN)mResizeStartW + deltaX;
    newH = (INTN)mResizeStartH + deltaY;
    maxH = mScreenH - taskH - App->WinY;

    if (newW < 260) {
      newW = 260;
    }
    if (newH < 180) {
      newH = 180;
    }
    if ((UINTN)newW + App->WinX > mScreenW) {
      newW = (INTN)(mScreenW - App->WinX);
    }
    if ((UINTN)newH > maxH) {
      newH = (INTN)maxH;
    }
    App->WinW = (UINTN)newW;
    App->WinH = (UINTN)newH;
    return;
  }

  if (mPainting && mFocusedApp == APP_PAINT && mApps[APP_PAINT].Open) {
    PaintAtCursor();
  }
}

static VOID
DrawDesktop(void) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL bandA;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL bandB;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL taskbar;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL taskbarTop;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL white;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btnBorder;
  UINTN y;
  UINTN taskH;
  UINTN btnW;
  UINTN btnH;
  UINTN btnY;

  bandA = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x34, 0x2A, 0x1E, 0};
  bandB = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x3D, 0x33, 0x23, 0};
  taskbar = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x1F, 0x1F, 0x1F, 0};
  taskbarTop = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x60, 0x60, 0x60, 0};
  white = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xE8, 0xE8, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x3A, 0x3A, 0x3A, 0};
  btnBorder = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x80, 0x80, 0x80, 0};

  taskH = TaskbarHeight();
  for (y = 0; y < mScreenH; y += 8) {
    FillRect(0, y, mScreenW, 8, ((y / 8) % 2 == 0) ? bandA : bandB);
  }
  FillRect(0, mScreenH - taskH, mScreenW, taskH, taskbar);
  FillRect(0, mScreenH - taskH, mScreenW, 2, taskbarTop);
  DrawTextLine(12, mScreenH - taskH + 14, L"DPOSBOOT V1.1.0", white, 1);

  btnW = 92;
  btnH = 24;
  btnY = mScreenH - taskH + 11;
  SetRect(&mRebootBtn, mScreenW - (btnW * 2) - 20, btnY, btnW, btnH);
  SetRect(&mShutdownBtn, mScreenW - btnW - 10, btnY, btnW, btnH);
  FillRect(mRebootBtn.X, mRebootBtn.Y, mRebootBtn.W, mRebootBtn.H, btn);
  FillRect(mShutdownBtn.X, mShutdownBtn.Y, mShutdownBtn.W, mShutdownBtn.H, btn);
  DrawBorder(mRebootBtn.X, mRebootBtn.Y, mRebootBtn.W, mRebootBtn.H, 1, btnBorder);
  DrawBorder(mShutdownBtn.X, mShutdownBtn.Y, mShutdownBtn.W, mShutdownBtn.H, 1, btnBorder);
  DrawCenteredLabel(mRebootBtn.X + (btnW / 2), mRebootBtn.Y + 7, L"REBOOT", white, 1);
  DrawCenteredLabel(mShutdownBtn.X + (btnW / 2), mShutdownBtn.Y + 7, L"SHUTDOWN", white, 1);
}

static VOID
DrawSetupScreen(void) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL bg;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  CHAR16 line[96];
  UINTN midX;
  UINTN boxW;
  UINTN boxX;
  UINTN y;
  UINTN i;

  bg = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x22, 0x22, 0x28, 0};
  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xE8, 0xEE, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x66, 0x66, 0x88, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x1C, 0x1C, 0x24, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xC8, 0xC8, 0xE8, 0};

  FillRect(0, 0, mScreenW, mScreenH, bg);
  boxW = (mScreenW > 520) ? 520 : (mScreenW - 40);
  boxX = (mScreenW - boxW) / 2;
  y = (mScreenH > 320) ? 60 : 20;
  FillRect(boxX, y, boxW, 220, panel);
  DrawBorder(boxX, y, boxW, 220, 2, border);
  midX = boxX + boxW / 2;

  DrawCenteredLabel(midX, y + 16, L"DPOSBOOT SETUP", text, 2);

  if (mSetupStep == 0) {
    DrawCenteredLabel(midX, y + 64, L"WELCOME", text, 1);
    DrawCenteredLabel(midX, y + 86, L"THIS WILL CREATE YOUR FIRST USER, TYPE YOUR USER NAME AND HIT ENTER!", text, 1);
  } else if (mSetupStep == 1) {
    DrawTextLine(boxX + 24, y + 64, L"USER NAME", text, 1);
    SetRect(&mSetupFieldRect, boxX + 24, y + 84, boxW - 48, 26);
    FillRect(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H,
             (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF8, 0xF8, 0xFF, 0});
    DrawBorder(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H, 1, border);
    DrawTextLine(mSetupFieldRect.X + 6, mSetupFieldRect.Y + 8, mSetupName, text, 1);
  } else if (mSetupStep == 2) {
    DrawTextLine(boxX + 24, y + 64, L"PASSWORD", text, 1);
    SetRect(&mSetupFieldRect, boxX + 24, y + 84, boxW - 48, 26);
    FillRect(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H,
             (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF8, 0xF8, 0xFF, 0});
    DrawBorder(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H, 1, border);
    for (i = 0; i < mSetupPassLen && i + 1 < PASS_CAP; ++i) {
      line[i] = L'*';
    }
    line[i] = 0;
    DrawTextLine(mSetupFieldRect.X + 6, mSetupFieldRect.Y + 8, line, text, 1);
  } else if (mSetupStep == 3) {
    DrawTextLine(boxX + 24, y + 64, L"CONFIRM PASSWORD", text, 1);
    SetRect(&mSetupFieldRect, boxX + 24, y + 84, boxW - 48, 26);
    FillRect(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H,
             (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF8, 0xF8, 0xFF, 0});
    DrawBorder(mSetupFieldRect.X, mSetupFieldRect.Y, mSetupFieldRect.W, mSetupFieldRect.H, 1, border);
    for (i = 0; i < mSetupPassConfirmLen && i + 1 < PASS_CAP; ++i) {
      line[i] = L'*';
    }
    line[i] = 0;
    DrawTextLine(mSetupFieldRect.X + 6, mSetupFieldRect.Y + 8, line, text, 1);
  } else {
    DrawCenteredLabel(midX, y + 64, L"SETUP COMPLETE", text, 1);
  }

  if (mSetupStatus[0] != 0) {
    DrawCenteredLabel(midX, y + 122, mSetupStatus, text, 1);
  }

  SetRect(&mSetupBackBtn, boxX + 24, y + 160, 88, 26);
  SetRect(&mSetupNextBtn, boxX + boxW - 112, y + 160, 88, 26);
  FillRect(mSetupBackBtn.X, mSetupBackBtn.Y, mSetupBackBtn.W, mSetupBackBtn.H, btn);
  FillRect(mSetupNextBtn.X, mSetupNextBtn.Y, mSetupNextBtn.W, mSetupNextBtn.H, btn);
  DrawBorder(mSetupBackBtn.X, mSetupBackBtn.Y, mSetupBackBtn.W, mSetupBackBtn.H, 1, border);
  DrawBorder(mSetupNextBtn.X, mSetupNextBtn.Y, mSetupNextBtn.W, mSetupNextBtn.H, 1, border);
  DrawCenteredLabel(mSetupBackBtn.X + 44, mSetupBackBtn.Y + 6, L"BACK", text, 1);
  DrawCenteredLabel(mSetupNextBtn.X + 44, mSetupNextBtn.Y + 6, L"NEXT", text, 1);
}

static VOID
DrawLoginScreen(void) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL bg;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selected;
  UINTN boxW;
  UINTN boxX;
  UINTN y;
  UINTN i;
  CHAR16 line[64];
  UINTN midX;

  bg = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x1C, 0x1C, 0x22, 0};
  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF0, 0xF0, 0xF4, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x66, 0x66, 0x88, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x1C, 0x1C, 0x24, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xC8, 0xC8, 0xE8, 0};
  selected = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xA8, 0xD8, 0xF4, 0};

  FillRect(0, 0, mScreenW, mScreenH, bg);
  boxW = (mScreenW > 520) ? 520 : (mScreenW - 40);
  boxX = (mScreenW - boxW) / 2;
  y = (mScreenH > 320) ? 60 : 20;
  FillRect(boxX, y, boxW, 240, panel);
  DrawBorder(boxX, y, boxW, 240, 2, border);
  midX = boxX + boxW / 2;

  DrawCenteredLabel(midX, y + 16, L"LOGIN", text, 2);

  for (i = 0; i < mState.UserCount && i < USER_MAX; ++i) {
    SetRect(&mLoginUserRects[i], boxX + 24, y + 52 + i * 26, boxW - 48, 22);
    FillRect(mLoginUserRects[i].X, mLoginUserRects[i].Y, mLoginUserRects[i].W,
             mLoginUserRects[i].H, (mLoginUserIndex == (INTN)i) ? selected : btn);
    DrawBorder(mLoginUserRects[i].X, mLoginUserRects[i].Y, mLoginUserRects[i].W,
               mLoginUserRects[i].H, 1, border);
    DrawTextLine(mLoginUserRects[i].X + 6, mLoginUserRects[i].Y + 6, mState.Users[i].Name, text, 1);
  }

  DrawTextLine(boxX + 24, y + 140, L"PASSWORD", text, 1);
  SetRect(&mLoginPassRect, boxX + 24, y + 160, boxW - 48, 26);
  FillRect(mLoginPassRect.X, mLoginPassRect.Y, mLoginPassRect.W, mLoginPassRect.H,
           (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF8, 0xF8, 0xFF, 0});
  DrawBorder(mLoginPassRect.X, mLoginPassRect.Y, mLoginPassRect.W, mLoginPassRect.H, 1, border);
  for (i = 0; i < mLoginPassLen && i + 1 < PASS_CAP; ++i) {
    line[i] = L'*';
  }
  line[i] = 0;
  DrawTextLine(mLoginPassRect.X + 6, mLoginPassRect.Y + 8, line, text, 1);

  SetRect(&mLoginOkBtn, boxX + boxW - 112, y + 196, 88, 26);
  FillRect(mLoginOkBtn.X, mLoginOkBtn.Y, mLoginOkBtn.W, mLoginOkBtn.H, btn);
  DrawBorder(mLoginOkBtn.X, mLoginOkBtn.Y, mLoginOkBtn.W, mLoginOkBtn.H, 1, border);
  DrawCenteredLabel(mLoginOkBtn.X + 44, mLoginOkBtn.Y + 6, L"LOGIN", text, 1);

  if (mLoginStatus[0] != 0) {
    DrawCenteredLabel(midX, y + 198, mLoginStatus, text, 1);
  }
}

static VOID
DrawIcon(IN UINTN AppIndex) {
  APP_WINDOW *App;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL iconBg;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selected;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL labelBg;

  App = &mApps[AppIndex];
  iconBg = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x26, 0x26, 0x26, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xC0, 0xC0, 0xC0, 0};
  selected = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x38, 0xD0, 0xD8, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF0, 0xF0, 0xF0, 0};
  labelBg = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x18, 0x18, 0x18, 0};

  FillRect(App->IconX, App->IconY, ICON_W, ICON_H, iconBg);
  DrawBorder(App->IconX, App->IconY, ICON_W, ICON_H, 2,
             (mSelectedIcon == (INTN)AppIndex) ? selected : border);
  FillRect(App->IconX + 10, App->IconY + 10, ICON_W - 20, ICON_H - 20, App->Accent);
  FillRect(App->IconX - 2, App->IconY + ICON_H + 2, ICON_W + 4, 14, labelBg);
  DrawCenteredLabel(App->IconX + (ICON_W / 2), App->IconY + ICON_H + 5, App->Name, text, 1);
}

static VOID
DrawNotesContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL cursor;
  UINTN x;
  UINTN y;
  UINTN i;
  UINTN stepX;
  UINTN stepY;
  UINTN maxX;
  UINTN maxY;
  BOOLEAN showCursor;
  RECT editRect;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF2, 0xF2, 0xF2, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x80, 0x80, 0x80, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x20, 0x20, 0};
  cursor = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x20, 0x20, 0};

  SetRect(&editRect, Client->X + 6, Client->Y + 6, Client->W - 12, Client->H - 12);
  FillRect(editRect.X, editRect.Y, editRect.W, editRect.H, panel);
  DrawBorder(editRect.X, editRect.Y, editRect.W, editRect.H, 1, border);

  stepX = GLYPH_W + 1;
  stepY = GLYPH_H + 2;
  x = editRect.X + 5;
  y = editRect.Y + 5;
  maxX = editRect.X + editRect.W - 6;
  maxY = editRect.Y + editRect.H - 8;
  showCursor = (((mTick / 20) % 2) == 0);

  for (i = 0; i < mNotesLen; ++i) {
    if (mNotesText[i] == L'\r') {
      continue;
    }
    if (mNotesText[i] == L'\n') {
      x = editRect.X + 5;
      y += stepY;
      if (y + GLYPH_H > maxY) {
        break;
      }
      continue;
    }
    if (x + GLYPH_W > maxX) {
      x = editRect.X + 5;
      y += stepY;
      if (y + GLYPH_H > maxY) {
        break;
      }
    }
    DrawChar(x, y, mNotesText[i], text, 1);
    x += stepX;
  }

  if (showCursor && mFocusedApp == APP_NOTES) {
    FillRect(x, y, 2, GLYPH_H, cursor);
  }
}

static VOID
DrawPaintContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL frame;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL white;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selectedBorder;
  RECT area;
  UINTN p;
  UINTN x;
  UINTN y;
  UINTN drawW;
  UINTN drawH;
  UINTN offX;
  UINTN offY;
  UINTN cellX;
  UINTN cellY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL borderColor;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xEA, 0xEA, 0xEA, 0};
  frame = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x77, 0x77, 0x77, 0};
  white = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xFF, 0xFF, 0xFF, 0};
  selectedBorder = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x10, 0x10, 0x10, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);

  for (p = 0; p < PAINT_COLOR_COUNT; ++p) {
    SetRect(&mPaintPaletteRects[p], Client->X + 8 + p * 24, Client->Y + 8, 18, 18);
    FillRect(mPaintPaletteRects[p].X, mPaintPaletteRects[p].Y, mPaintPaletteRects[p].W,
             mPaintPaletteRects[p].H, mPaintColors[p]);
    borderColor = (p == mPaintColorIndex) ? selectedBorder : frame;
    DrawBorder(mPaintPaletteRects[p].X, mPaintPaletteRects[p].Y, mPaintPaletteRects[p].W,
               mPaintPaletteRects[p].H, 1, borderColor);
  }

  SetRect(&area, Client->X + 8, Client->Y + 34, Client->W - 16, Client->H - 42);
  mPaintCellSize = area.W / PAINT_GRID_W;
  if (area.H / PAINT_GRID_H < mPaintCellSize) {
    mPaintCellSize = area.H / PAINT_GRID_H;
  }
  if (mPaintCellSize == 0) {
    mPaintCellSize = 1;
  }
  drawW = PAINT_GRID_W * mPaintCellSize;
  drawH = PAINT_GRID_H * mPaintCellSize;
  offX = area.X + ((area.W > drawW) ? ((area.W - drawW) / 2) : 0);
  offY = area.Y + ((area.H > drawH) ? ((area.H - drawH) / 2) : 0);
  SetRect(&mPaintCanvasRect, offX, offY, drawW, drawH);

  FillRect(mPaintCanvasRect.X, mPaintCanvasRect.Y, mPaintCanvasRect.W, mPaintCanvasRect.H,
           white);
  DrawBorder(mPaintCanvasRect.X, mPaintCanvasRect.Y, mPaintCanvasRect.W, mPaintCanvasRect.H, 1,
             frame);

  for (y = 0; y < PAINT_GRID_H; ++y) {
    for (x = 0; x < PAINT_GRID_W; ++x) {
      if (mPaintGrid[y][x] == PAINT_EMPTY) {
        continue;
      }
      cellX = mPaintCanvasRect.X + x * mPaintCellSize;
      cellY = mPaintCanvasRect.Y + y * mPaintCellSize;
      FillRect(cellX, cellY, mPaintCellSize, mPaintCellSize,
               mPaintColors[mPaintGrid[y][x] % PAINT_COLOR_COUNT]);
    }
  }
}

static VOID
DrawFilesContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL row;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selectedRow;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  CHAR16 line[64];
  CHAR16 num[24];
  UINTN r;
  UINTN painted;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE6, 0xE6, 0xE6, 0};
  row = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xD0, 0xD0, 0xD0, 0};
  selectedRow = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x9A, 0xC8, 0xF4, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x75, 0x75, 0x75, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x22, 0x22, 0x22, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xB0, 0xB0, 0xB0, 0};

  painted = CountPaintPixels();
  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);

  for (r = 0; r < 3; ++r) {
    SetRect(&mFilesRows[r], Client->X + 8, Client->Y + 10 + r * 30, Client->W - 16, 24);
    FillRect(mFilesRows[r].X, mFilesRows[r].Y, mFilesRows[r].W, mFilesRows[r].H,
             (mFilesSelected == (INTN)r) ? selectedRow : row);
    DrawBorder(mFilesRows[r].X, mFilesRows[r].Y, mFilesRows[r].W, mFilesRows[r].H, 1, border);
  }

  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"NOTES.TXT  ");
  IntToStr16((INT64)mNotesLen, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  AppendString16(line, sizeof(line) / sizeof(line[0]), L" B");
  DrawTextLine(mFilesRows[0].X + 8, mFilesRows[0].Y + 8, line, text, 1);
  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"PAINT.RAW  ");
  IntToStr16((INT64)painted, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  AppendString16(line, sizeof(line) / sizeof(line[0]), L" PX");
  DrawTextLine(mFilesRows[1].X + 8, mFilesRows[1].Y + 8, line, text, 1);
  DrawTextLine(mFilesRows[2].X + 8, mFilesRows[2].Y + 8, L"README.TXT  120 B", text, 1);

  SetRect(&mFilesOpenButton, Client->X + 12, Client->Y + Client->H - 34, 80, 22);
  SetRect(&mFilesClearButton, Client->X + 100, Client->Y + Client->H - 34, 80, 22);
  FillRect(mFilesOpenButton.X, mFilesOpenButton.Y, mFilesOpenButton.W, mFilesOpenButton.H, btn);
  FillRect(mFilesClearButton.X, mFilesClearButton.Y, mFilesClearButton.W, mFilesClearButton.H,
           btn);
  DrawBorder(mFilesOpenButton.X, mFilesOpenButton.Y, mFilesOpenButton.W, mFilesOpenButton.H, 1,
             border);
  DrawBorder(mFilesClearButton.X, mFilesClearButton.Y, mFilesClearButton.W, mFilesClearButton.H,
             1, border);
  DrawTextLine(mFilesOpenButton.X + 20, mFilesOpenButton.Y + 7, L"OPEN", text, 1);
  DrawTextLine(mFilesClearButton.X + 16, mFilesClearButton.Y + 7, L"CLEAR", text, 1);
}

static VOID
DrawCalcContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL box;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL cursor;
  UINTN cursorX;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xE8, 0xEF, 0};
  box = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF8, 0xF8, 0xFA, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x72, 0x72, 0x90, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x22, 0x22, 0x2A, 0};
  cursor = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x22, 0x22, 0x2A, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  DrawTextLine(Client->X + 8, Client->Y + 8, L"EXPR:", text, 1);
  FillRect(Client->X + 8, Client->Y + 20, Client->W - 16, 18, box);
  DrawBorder(Client->X + 8, Client->Y + 20, Client->W - 16, 18, 1, border);
  DrawTextLine(Client->X + 12, Client->Y + 25, mCalcExpr, text, 1);

  if (((mTick / 20) % 2) == 0 && mFocusedApp == APP_CALC) {
    cursorX = Client->X + 12 + mCalcExprLen * (GLYPH_W + 1);
    FillRect(cursorX, Client->Y + 25, 2, GLYPH_H, cursor);
  }

  DrawTextLine(Client->X + 8, Client->Y + 50, L"RESULT:", text, 1);
  FillRect(Client->X + 8, Client->Y + 62, Client->W - 16, 20, box);
  DrawBorder(Client->X + 8, Client->Y + 62, Client->W - 16, 20, 1, border);
  DrawTextLine(Client->X + 12, Client->Y + 68, mCalcResult, text, 1);
  DrawTextLine(Client->X + 8, Client->Y + Client->H - 16, L"ENTER=EVAL  C=CLEAR", text, 1);
}

static VOID
DrawClockContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ring;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  CHAR16 uptime[32];
  CHAR16 frameText[32];
  CHAR16 num[24];

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xF0, 0xF0, 0};
  ring = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x74, 0x86, 0x86, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x1E, 0x2A, 0x2A, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  DrawBorder(Client->X + 10, Client->Y + 10, Client->W - 20, Client->H - 20, 2, ring);
  DrawTextLine(Client->X + 18, Client->Y + 20, L"UPTIME", text, 1);

  FormatUptime(mTick, uptime, sizeof(uptime) / sizeof(uptime[0]));
  DrawTextLine(Client->X + 18, Client->Y + 42, uptime, text, 2);

  frameText[0] = 0;
  AppendString16(frameText, sizeof(frameText) / sizeof(frameText[0]), L"FRAMES ");
  IntToStr16((INT64)mTick, num, sizeof(num) / sizeof(num[0]));
  AppendString16(frameText, sizeof(frameText) / sizeof(frameText[0]), num);
  DrawTextLine(Client->X + 18, Client->Y + 82, frameText, text, 1);
}

static VOID
DrawTerminalContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL prompt;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  UINTN i;
  UINTN stepY;
  UINTN visibleRows;
  UINTN start;
  UINTN y;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x12, 0x18, 0x12, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x9A, 0xE8, 0x9A, 0};
  prompt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xC6, 0xF0, 0xC6, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x46, 0x6A, 0x46, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  DrawBorder(Client->X, Client->Y, Client->W, Client->H, 1, border);

  stepY = GLYPH_H + 2;
  if (Client->H < 24) {
    return;
  }
  visibleRows = (Client->H - 24) / stepY;
  if (visibleRows > TERM_LINES) {
    visibleRows = TERM_LINES;
  }
  start = (mTermLineCount > visibleRows) ? (mTermLineCount - visibleRows) : 0;

  y = Client->Y + 6;
  for (i = start; i < mTermLineCount; ++i) {
    DrawTextLine(Client->X + 6, y, mTermLines[i], text, 1);
    y += stepY;
  }

  DrawTextLine(Client->X + 6, Client->Y + Client->H - 15, L">", prompt, 1);
  DrawTextLine(Client->X + 14, Client->Y + Client->H - 15, mTermInput, prompt, 1);
  if (((mTick / 20) % 2) == 0 && mFocusedApp == APP_TERMINAL) {
    FillRect(Client->X + 14 + mTermInputLen * (GLYPH_W + 1), Client->Y + Client->H - 15, 2,
             GLYPH_H, prompt);
  }
}

static VOID
DrawSettingsContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  CHAR16 line[64];
  CHAR16 num[24];
  UINTN fps;
  UINTN yPos;
  UINTN rowHeight = 28;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE6, 0xE6, 0xF2, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x70, 0x70, 0xA0, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xB8, 0xB8, 0xE8, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x20, 0x2A, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  yPos = Client->Y + 10;

  // Cursor Size Section
  DrawTextLine(Client->X + 10, yPos, L"CURSOR SIZE", text, 1);
  yPos += 18;
  
  SetRect(&mSettingsCursorMinus, Client->X + 10, yPos, 24, 22);
  SetRect(&mSettingsCursorPlus, Client->X + 40, yPos, 24, 22);
  FillRect(mSettingsCursorMinus.X, mSettingsCursorMinus.Y, mSettingsCursorMinus.W,
           mSettingsCursorMinus.H, btn);
  FillRect(mSettingsCursorPlus.X, mSettingsCursorPlus.Y, mSettingsCursorPlus.W,
           mSettingsCursorPlus.H, btn);
  DrawBorder(mSettingsCursorMinus.X, mSettingsCursorMinus.Y, mSettingsCursorMinus.W,
             mSettingsCursorMinus.H, 1, border);
  DrawBorder(mSettingsCursorPlus.X, mSettingsCursorPlus.Y, mSettingsCursorPlus.W,
             mSettingsCursorPlus.H, 1, border);
  DrawTextLine(mSettingsCursorMinus.X + 8, mSettingsCursorMinus.Y + 6, L"-", text, 2);
  DrawTextLine(mSettingsCursorPlus.X + 8, mSettingsCursorPlus.Y + 6, L"+", text, 2);
  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"SIZE: ");
  IntToStr16((INT64)mCursorSize, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  DrawTextLine(Client->X + 80, yPos + 6, line, text, 1);
  yPos += rowHeight;

  // Frame Rate Section
  DrawTextLine(Client->X + 10, yPos, L"FRAME RATE", text, 1);
  yPos += 18;
  
  SetRect(&mSettingsFpsMinus, Client->X + 10, yPos, 24, 22);
  SetRect(&mSettingsFpsPlus, Client->X + 40, yPos, 24, 22);
  FillRect(mSettingsFpsMinus.X, mSettingsFpsMinus.Y, mSettingsFpsMinus.W, mSettingsFpsMinus.H,
           btn);
  FillRect(mSettingsFpsPlus.X, mSettingsFpsPlus.Y, mSettingsFpsPlus.W, mSettingsFpsPlus.H, btn);
  DrawBorder(mSettingsFpsMinus.X, mSettingsFpsMinus.Y, mSettingsFpsMinus.W, mSettingsFpsMinus.H,
             1, border);
  DrawBorder(mSettingsFpsPlus.X, mSettingsFpsPlus.Y, mSettingsFpsPlus.W, mSettingsFpsPlus.H, 1,
             border);
  DrawTextLine(mSettingsFpsMinus.X + 8, mSettingsFpsMinus.Y + 6, L"-", text, 2);
  DrawTextLine(mSettingsFpsPlus.X + 8, mSettingsFpsPlus.Y + 6, L"+", text, 2);
  fps = (mFrameDelayUs > 0) ? (1000000U / mFrameDelayUs) : 0;
  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"FPS: ");
  IntToStr16((INT64)fps, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  DrawTextLine(Client->X + 80, yPos + 6, line, text, 1);
  yPos += rowHeight + 8;

  // WiFi SSID Section
  DrawTextLine(Client->X + 10, yPos, L"WIFI SSID", text, 1);
  yPos += 18;
  
  SetRect(&mSettingsSsidRect, Client->X + 10, yPos, Client->W - 20, 22);
  FillRect(mSettingsSsidRect.X, mSettingsSsidRect.Y, mSettingsSsidRect.W, mSettingsSsidRect.H,
           (mSettingsEditField == 1) ? (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xD8, 0xE8, 0xFF, 0}
                                     : (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF6, 0xF6, 0xFF, 0});
  DrawBorder(mSettingsSsidRect.X, mSettingsSsidRect.Y, mSettingsSsidRect.W, mSettingsSsidRect.H, 1,
             border);
  DrawTextLine(mSettingsSsidRect.X + 6, mSettingsSsidRect.Y + 6, mWifiSsid, text, 1);
  yPos += rowHeight;

  // WiFi Password Section
  DrawTextLine(Client->X + 10, yPos, L"PASSWORD", text, 1);
  yPos += 18;
  
  SetRect(&mSettingsPassRect, Client->X + 10, yPos, Client->W - 20, 22);
  FillRect(mSettingsPassRect.X, mSettingsPassRect.Y, mSettingsPassRect.W, mSettingsPassRect.H,
           (mSettingsEditField == 2) ? (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xD8, 0xE8, 0xFF, 0}
                                     : (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF6, 0xF6, 0xFF, 0});
  DrawBorder(mSettingsPassRect.X, mSettingsPassRect.Y, mSettingsPassRect.W, mSettingsPassRect.H, 1,
             border);
  // Mask password
  CHAR16 maskedPass[WIFI_PASS_CAP];
  UINTN passLen = StrLen16(mWifiPass);
  for (UINTN i = 0; i < passLen && i < WIFI_PASS_CAP - 1; i++) {
    maskedPass[i] = L'*';
  }
  maskedPass[passLen] = 0;
  DrawTextLine(mSettingsPassRect.X + 6, mSettingsPassRect.Y + 6, maskedPass, text, 1);
  yPos += rowHeight + 8;

  // Connect/Disconnect Buttons
  UINTN btnWidth = (Client->W - 30) / 2;
  SetRect(&mSettingsConnectBtn, Client->X + 10, yPos, btnWidth, 28);
  SetRect(&mSettingsDisconnectBtn, Client->X + 20 + btnWidth, yPos, btnWidth, 28);
  
  FillRect(mSettingsConnectBtn.X, mSettingsConnectBtn.Y, mSettingsConnectBtn.W,
           mSettingsConnectBtn.H, btn);
  FillRect(mSettingsDisconnectBtn.X, mSettingsDisconnectBtn.Y, mSettingsDisconnectBtn.W,
           mSettingsDisconnectBtn.H, btn);
  DrawBorder(mSettingsConnectBtn.X, mSettingsConnectBtn.Y, mSettingsConnectBtn.W,
             mSettingsConnectBtn.H, 1, border);
  DrawBorder(mSettingsDisconnectBtn.X, mSettingsDisconnectBtn.Y, mSettingsDisconnectBtn.W,
             mSettingsDisconnectBtn.H, 1, border);
  
  // Center button text
  DrawCenteredLabel(mSettingsConnectBtn.X + (btnWidth / 2), mSettingsConnectBtn.Y + 8, 
                    L"CONNECT", text, 1);
  DrawCenteredLabel(mSettingsDisconnectBtn.X + (btnWidth / 2), mSettingsDisconnectBtn.Y + 8, 
                    L"DISCONNECT", text, 1);
  yPos += 38;

  // WiFi Status
  CHAR16 WifiDetail[96];
  WifiDetail[0] = 0;
  AppendString16(WifiDetail, 96, L"Status: ");
  AppendString16(WifiDetail, 96, mWifiStatus);
  
  if (mWifiConnected && StrLen16(mWifiSsid) > 0) {
      AppendString16(WifiDetail, 96, L" (");
      AppendString16(WifiDetail, 96, mWifiSsid);
      AppendString16(WifiDetail, 96, L")");
  }
  
  DrawTextLine(Client->X + 10, yPos, WifiDetail, text, 1);
  yPos += 20;

  SetRect(&mSettingsResetBtn, Client->X + 10, yPos, Client->W - 20, 26);
  FillRect(mSettingsResetBtn.X, mSettingsResetBtn.Y, mSettingsResetBtn.W, mSettingsResetBtn.H, btn);
  DrawBorder(mSettingsResetBtn.X, mSettingsResetBtn.Y, mSettingsResetBtn.W, mSettingsResetBtn.H, 1, border);
  DrawCenteredLabel(mSettingsResetBtn.X + (mSettingsResetBtn.W / 2), mSettingsResetBtn.Y + 6,
                    L"FACTORY RESET", text, 1);
  yPos += 34;

  // Help text at bottom
  DrawTextLine(Client->X + 10, Client->Y + Client->H - 18, 
               L"TAB: Switch fields  ENTER: Connect  [ ]: Cursor  - =: FPS", text, 1);
}

static VOID
DrawTasksContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL row;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selectedRow;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  UINTN r;
  CHAR16 line[64];

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF2, 0xE9, 0xE9, 0};
  row = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xDB, 0xC8, 0xC8, 0};
  selectedRow = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF4, 0xB8, 0x9A, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x88, 0x66, 0x66, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x2A, 0x20, 0x20, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xD8, 0xB8, 0xB8, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  for (r = 0; r < APP_COUNT; ++r) {
    SetRect(&mTaskRows[r], Client->X + 8, Client->Y + 8 + r * 22, Client->W - 16, 18);
    FillRect(mTaskRows[r].X, mTaskRows[r].Y, mTaskRows[r].W, mTaskRows[r].H,
             (mTaskSelected == (INTN)r) ? selectedRow : row);
    DrawBorder(mTaskRows[r].X, mTaskRows[r].Y, mTaskRows[r].W, mTaskRows[r].H, 1, border);

    line[0] = 0;
    AppendString16(line, sizeof(line) / sizeof(line[0]), mApps[r].Name);
    AppendString16(line, sizeof(line) / sizeof(line[0]), mApps[r].Open ? L"  OPEN" : L"  CLOSED");
    DrawTextLine(mTaskRows[r].X + 6, mTaskRows[r].Y + 5, line, text, 1);
  }

  SetRect(&mTasksOpenButton, Client->X + 10, Client->Y + Client->H - 28, 84, 18);
  SetRect(&mTasksCloseButton, Client->X + 100, Client->Y + Client->H - 28, 84, 18);
  FillRect(mTasksOpenButton.X, mTasksOpenButton.Y, mTasksOpenButton.W, mTasksOpenButton.H, btn);
  FillRect(mTasksCloseButton.X, mTasksCloseButton.Y, mTasksCloseButton.W, mTasksCloseButton.H,
           btn);
  DrawBorder(mTasksOpenButton.X, mTasksOpenButton.Y, mTasksOpenButton.W, mTasksOpenButton.H, 1,
             border);
  DrawBorder(mTasksCloseButton.X, mTasksCloseButton.Y, mTasksCloseButton.W, mTasksCloseButton.H,
             1, border);
  DrawTextLine(mTasksOpenButton.X + 12, mTasksOpenButton.Y + 5, L"OPEN", text, 1);
  DrawTextLine(mTasksCloseButton.X + 10, mTasksCloseButton.Y + 5, L"CLOSE", text, 1);
}

static VOID
DrawPhotoContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  RECT canvas;
  UINTN cell;
  UINTN drawW;
  UINTN drawH;
  UINTN x;
  UINTN y;
  UINTN ox;
  UINTN oy;
  CHAR16 line[64];
  CHAR16 num[16];

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE9, 0xF1, 0xE9, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x6A, 0x86, 0x6A, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xB8, 0xD8, 0xB8, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x2A, 0x20, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  SetRect(&mPhotoPrevButton, Client->X + 8, Client->Y + 8, 40, 18);
  SetRect(&mPhotoNextButton, Client->X + 54, Client->Y + 8, 40, 18);
  SetRect(&mPhotoSnapButton, Client->X + 100, Client->Y + 8, 64, 18);
  FillRect(mPhotoPrevButton.X, mPhotoPrevButton.Y, mPhotoPrevButton.W, mPhotoPrevButton.H, btn);
  FillRect(mPhotoNextButton.X, mPhotoNextButton.Y, mPhotoNextButton.W, mPhotoNextButton.H, btn);
  FillRect(mPhotoSnapButton.X, mPhotoSnapButton.Y, mPhotoSnapButton.W, mPhotoSnapButton.H, btn);
  DrawBorder(mPhotoPrevButton.X, mPhotoPrevButton.Y, mPhotoPrevButton.W, mPhotoPrevButton.H, 1,
             border);
  DrawBorder(mPhotoNextButton.X, mPhotoNextButton.Y, mPhotoNextButton.W, mPhotoNextButton.H, 1,
             border);
  DrawBorder(mPhotoSnapButton.X, mPhotoSnapButton.Y, mPhotoSnapButton.W, mPhotoSnapButton.H, 1,
             border);
  DrawTextLine(mPhotoPrevButton.X + 10, mPhotoPrevButton.Y + 6, L"PREV", text, 1);
  DrawTextLine(mPhotoNextButton.X + 10, mPhotoNextButton.Y + 6, L"NEXT", text, 1);
  DrawTextLine(mPhotoSnapButton.X + 10, mPhotoSnapButton.Y + 6, L"SNAP", text, 1);

  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"COUNT ");
  IntToStr16((INT64)mPhotoCount, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  DrawTextLine(Client->X + 172, Client->Y + 12, line, text, 1);

  SetRect(&canvas, Client->X + 8, Client->Y + 34, Client->W - 16, Client->H - 42);
  cell = canvas.W / PAINT_GRID_W;
  if (canvas.H / PAINT_GRID_H < cell) {
    cell = canvas.H / PAINT_GRID_H;
  }
  if (cell == 0) {
    cell = 1;
  }
  drawW = PAINT_GRID_W * cell;
  drawH = PAINT_GRID_H * cell;
  ox = canvas.X + ((canvas.W > drawW) ? ((canvas.W - drawW) / 2) : 0);
  oy = canvas.Y + ((canvas.H > drawH) ? ((canvas.H - drawH) / 2) : 0);
  FillRect(ox, oy, drawW, drawH, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xFF, 0xFF, 0xFF, 0});
  DrawBorder(ox, oy, drawW, drawH, 1, border);

  if (mPhotoCount == 0) {
    DrawTextLine(ox + 8, oy + 8, L"NO SNAPSHOTS YET", text, 1);
    return;
  }

  for (y = 0; y < PAINT_GRID_H; ++y) {
    for (x = 0; x < PAINT_GRID_W; ++x) {
      if (mPhotos[mPhotoSelected][y][x] == PAINT_EMPTY) {
        continue;
      }
      FillRect(ox + x * cell, oy + y * cell, cell, cell,
               mPaintColors[mPhotos[mPhotoSelected][y][x] % PAINT_COLOR_COUNT]);
    }
  }
}

static VOID
DrawSnakeContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL snake;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL food;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  RECT board;
  UINTN cell;
  UINTN i;
  CHAR16 line[48];
  CHAR16 num[16];

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xF4, 0xE8, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x5A, 0x8A, 0x5A, 0};
  snake = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0xA0, 0x20, 0};
  food = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x30, 0x30, 0xD0, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x30, 0x20, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  SetRect(&board, Client->X + 8, Client->Y + 8, Client->W - 16, Client->H - 28);
  cell = board.W / SNAKE_GRID_W;
  if (board.H / SNAKE_GRID_H < cell) {
    cell = board.H / SNAKE_GRID_H;
  }
  if (cell == 0) {
    cell = 1;
  }
  board.W = SNAKE_GRID_W * cell;
  board.H = SNAKE_GRID_H * cell;
  FillRect(board.X, board.Y, board.W, board.H, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xFC, 0xFC, 0xFC, 0});
  DrawBorder(board.X, board.Y, board.W, board.H, 1, border);

  if (mSnakeAlive) {
    FillRect(board.X + mSnakeFoodX * cell, board.Y + mSnakeFoodY * cell, cell, cell, food);
    for (i = 0; i < mSnakeLen; ++i) {
      FillRect(board.X + mSnakeX[i] * cell, board.Y + mSnakeY[i] * cell, cell, cell, snake);
    }
  } else {
    DrawTextLine(board.X + 8, board.Y + 8, L"CLICK OR R TO START", text, 1);
  }

  line[0] = 0;
  AppendString16(line, sizeof(line) / sizeof(line[0]), L"SCORE ");
  IntToStr16((INT64)mSnakeScore, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, sizeof(line) / sizeof(line[0]), num);
  DrawTextLine(Client->X + 8, Client->Y + Client->H - 14, line, text, 1);
}

static VOID
DrawMonitorContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  CHAR16 line[64];
  CHAR16 num[24];
  UINTN fps;
  UINTN openCount;
  UINTN i;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE4, 0xE4, 0xE4, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x20, 0x20, 0x20, 0};
  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);

  fps = (mFrameDelayUs > 0) ? (1000000U / mFrameDelayUs) : 0;
  openCount = 0;
  for (i = 0; i < APP_COUNT; ++i) {
    if (mApps[i].Open) {
      openCount++;
    }
  }

  line[0] = 0;
  AppendString16(line, 64, L"FPS ");
  IntToStr16((INT64)fps, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, 64, num);
  DrawTextLine(Client->X + 8, Client->Y + 8, line, text, 1);
  line[0] = 0;
  AppendString16(line, 64, L"CURSOR ");
  IntToStr16((INT64)mCursorX, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, 64, num);
  AppendString16(line, 64, L",");
  IntToStr16((INT64)mCursorY, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, 64, num);
  DrawTextLine(Client->X + 8, Client->Y + 22, line, text, 1);
  line[0] = 0;
  AppendString16(line, 64, L"OPEN APPS ");
  IntToStr16((INT64)openCount, num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, 64, num);
  DrawTextLine(Client->X + 8, Client->Y + 36, line, text, 1);
  line[0] = 0;
  AppendString16(line, 64, L"PAINT PIX ");
  IntToStr16((INT64)CountPaintPixels(), num, sizeof(num) / sizeof(num[0]));
  AppendString16(line, 64, num);
  DrawTextLine(Client->X + 8, Client->Y + 50, line, text, 1);
  DrawTextLine(Client->X + 8, Client->Y + 68, mWifiConnected ? L"WIFI CONNECTED" : L"WIFI OFFLINE",
               text, 1);
}

static VOID
DrawBrowserContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  CHAR16 line[96];

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF2, 0xEF, 0xD9, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xA0, 0x88, 0x44, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE0, 0xD0, 0x90, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x2A, 0x24, 0x10, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  SetRect(&mBrowserPrevBtn, Client->X + 8, Client->Y + 8, 40, 18);
  SetRect(&mBrowserNextBtn, Client->X + 54, Client->Y + 8, 40, 18);
  FillRect(mBrowserPrevBtn.X, mBrowserPrevBtn.Y, mBrowserPrevBtn.W, mBrowserPrevBtn.H, btn);
  FillRect(mBrowserNextBtn.X, mBrowserNextBtn.Y, mBrowserNextBtn.W, mBrowserNextBtn.H, btn);
  DrawBorder(mBrowserPrevBtn.X, mBrowserPrevBtn.Y, mBrowserPrevBtn.W, mBrowserPrevBtn.H, 1, border);
  DrawBorder(mBrowserNextBtn.X, mBrowserNextBtn.Y, mBrowserNextBtn.W, mBrowserNextBtn.H, 1, border);
  DrawTextLine(mBrowserPrevBtn.X + 8, mBrowserPrevBtn.Y + 6, L"PREV", text, 1);
  DrawTextLine(mBrowserNextBtn.X + 8, mBrowserNextBtn.Y + 6, L"NEXT", text, 1);

  if (mBrowserPage == 0) {
    DrawTextLine(Client->X + 104, Client->Y + 12, L"HOME", text, 1);
    DrawTextLine(Client->X + 8, Client->Y + 36, L"DPOS OFFLINE HOME PAGE", text, 1);
  } else if (mBrowserPage == 1) {
    DrawTextLine(Client->X + 104, Client->Y + 12, L"STATUS", text, 1);
    if (mWifiConnected) {
      line[0] = 0;
      AppendString16(line, 96, L"CONNECTED TO ");
      AppendString16(line, 96, mWifiSsid);
      DrawTextLine(Client->X + 8, Client->Y + 36, line, text, 1);
    } else {
      DrawTextLine(Client->X + 8, Client->Y + 36, L"OFFLINE. CONFIGURE WIFI IN SETTINGS.", text, 1);
    }
  } else {
    DrawTextLine(Client->X + 104, Client->Y + 12, L"DOCS", text, 1);
    DrawTextLine(Client->X + 8, Client->Y + 36, L"MAIL USES SAME WIFI STATE AS BROWSER.", text, 1);
    DrawTextLine(Client->X + 8, Client->Y + 50, L"KEYS: N/P TO CHANGE PAGE.", text, 1);
  }
}

static VOID
DrawMailContent(IN CONST RECT *Client) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL panel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL row;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL selectedRow;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL border;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL text;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL btn;
  UINTN r;

  panel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF3, 0xE8, 0xE8, 0};
  row = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE0, 0xD0, 0xD0, 0};
  selectedRow = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF0, 0xB8, 0xB8, 0};
  border = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x96, 0x66, 0x66, 0};
  text = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x2A, 0x20, 0x20, 0};
  btn = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE8, 0xC8, 0xC8, 0};

  FillRect(Client->X, Client->Y, Client->W, Client->H, panel);
  SetRect(&mMailSyncBtn, Client->X + 8, Client->Y + 8, 56, 18);
  SetRect(&mMailClearBtn, Client->X + 70, Client->Y + 8, 56, 18);
  FillRect(mMailSyncBtn.X, mMailSyncBtn.Y, mMailSyncBtn.W, mMailSyncBtn.H, btn);
  FillRect(mMailClearBtn.X, mMailClearBtn.Y, mMailClearBtn.W, mMailClearBtn.H, btn);
  DrawBorder(mMailSyncBtn.X, mMailSyncBtn.Y, mMailSyncBtn.W, mMailSyncBtn.H, 1, border);
  DrawBorder(mMailClearBtn.X, mMailClearBtn.Y, mMailClearBtn.W, mMailClearBtn.H, 1, border);
  DrawTextLine(mMailSyncBtn.X + 10, mMailSyncBtn.Y + 6, L"SYNC", text, 1);
  DrawTextLine(mMailClearBtn.X + 8, mMailClearBtn.Y + 6, L"CLEAR", text, 1);

  DrawTextLine(Client->X + 132, Client->Y + 12, mWifiConnected ? L"ONLINE" : L"OFFLINE", text, 1);

  for (r = 0; r < MAIL_MAX; ++r) {
    SetRect(&mMailRows[r], Client->X + 8, Client->Y + 32 + r * 20, Client->W - 16, 18);
    FillRect(mMailRows[r].X, mMailRows[r].Y, mMailRows[r].W, mMailRows[r].H,
             (mMailSelected == (INTN)r) ? selectedRow : row);
    DrawBorder(mMailRows[r].X, mMailRows[r].Y, mMailRows[r].W, mMailRows[r].H, 1, border);
    if (r < mMailCount) {
      DrawTextLine(mMailRows[r].X + 6, mMailRows[r].Y + 5, mMailItems[r], text, 1);
    }
  }
}

static VOID
DrawWindow(IN UINTN AppIndex) {
  APP_WINDOW *App;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL body;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL frame;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL activeFrame;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL closeColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL titleText;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL handle;
  RECT closeRect;
  RECT resizeRect;
  RECT clientRect;
  UINTN i;

  App = &mApps[AppIndex];
  body = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x22, 0x22, 0x22, 0};
  frame = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xA9, 0xA9, 0xA9, 0};
  activeFrame = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xE0, 0xE0, 0x80, 0};
  closeColor = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x3D, 0x49, 0xE8, 0};
  titleText = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x15, 0x15, 0x15, 0};
  handle = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xD0, 0xD0, 0xD0, 0};

  FillRect(App->WinX, App->WinY, App->WinW, App->WinH, body);
  DrawBorder(App->WinX, App->WinY, App->WinW, App->WinH, BORDER_W,
             (mFocusedApp == (INTN)AppIndex) ? activeFrame : frame);
  FillRect(App->WinX + BORDER_W, App->WinY + BORDER_W, App->WinW - (BORDER_W * 2),
           TITLE_H - BORDER_W, App->Accent);
  DrawTextLine(App->WinX + 10, App->WinY + 9, App->Name, titleText, 1);

  GetCloseRect(App, &closeRect);
  FillRect(closeRect.X, closeRect.Y, closeRect.W, closeRect.H, closeColor);
  DrawTextLine(closeRect.X + 4, closeRect.Y + 4, L"X", frame, 1);

  GetResizeRect(App, &resizeRect);
  FillRect(resizeRect.X, resizeRect.Y, resizeRect.W, resizeRect.H, handle);
  for (i = 0; i < 3; ++i) {
    FillRect(resizeRect.X + 2 + i * 3, resizeRect.Y + resizeRect.H - 3 - i * 3, 2, 2, frame);
  }

  GetClientRect(App, &clientRect);
  if (AppIndex == APP_NOTES) {
    DrawNotesContent(&clientRect);
  } else if (AppIndex == APP_PAINT) {
    DrawPaintContent(&clientRect);
  } else if (AppIndex == APP_FILES) {
    DrawFilesContent(&clientRect);
  } else if (AppIndex == APP_CALC) {
    DrawCalcContent(&clientRect);
  } else if (AppIndex == APP_CLOCK) {
    DrawClockContent(&clientRect);
  } else if (AppIndex == APP_TERMINAL) {
    DrawTerminalContent(&clientRect);
  } else if (AppIndex == APP_SETTINGS) {
    DrawSettingsContent(&clientRect);
  } else if (AppIndex == APP_TASKS) {
    DrawTasksContent(&clientRect);
  } else if (AppIndex == APP_PHOTO) {
    DrawPhotoContent(&clientRect);
  } else if (AppIndex == APP_SNAKE) {
    DrawSnakeContent(&clientRect);
  } else if (AppIndex == APP_MONITOR) {
    DrawMonitorContent(&clientRect);
  } else if (AppIndex == APP_BROWSER) {
    DrawBrowserContent(&clientRect);
  } else if (AppIndex == APP_MAIL) {
    DrawMailContent(&clientRect);
  }
}

static VOID
DrawCursor(void) {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL white;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL black;
  UINTN cap;
  UINTN inner;

  white = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0xF2, 0xF2, 0xF2, 0};
  black = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL){0x00, 0x00, 0x00, 0};
  cap = mCursorSize;
  if (cap < 8) {
    cap = 8;
  }
  inner = (cap > 4) ? (cap - 4) : 1;
  FillRect(mCursorX, mCursorY, cap, 2, white);
  FillRect(mCursorX, mCursorY, 2, cap, white);
  FillRect(mCursorX + 2, mCursorY + 2, inner, 1, black);
  FillRect(mCursorX + 2, mCursorY + 2, 1, inner, black);
}

static VOID
RenderFrame(void) {
  UINTN i;
  UINTN idx;
  if (mUiMode == UI_SETUP) {
    DrawSetupScreen();
  } else if (mUiMode == UI_LOGIN) {
    DrawLoginScreen();
  } else {
    DrawDesktop();
    for (i = 0; i < APP_COUNT; ++i) {
      DrawIcon(i);
    }
    for (i = 0; i < APP_COUNT; ++i) {
      idx = mZOrder[i];
      if (mApps[idx].Open) {
        DrawWindow(idx);
      }
    }
  }
  DrawCursor();
  PresentBackBuffer();
}

static VOID
SetupNextStep(void) {
  mSetupStatus[0] = 0;
  if (mSetupStep == 0) {
    mSetupStep = 1;
    return;
  }
  if (mSetupStep == 1) {
    if (mSetupNameLen == 0) {
      StrCopy16(mSetupStatus, sizeof(mSetupStatus) / sizeof(mSetupStatus[0]), L"ENTER A USER NAME");
      return;
    }
    mSetupStep = 2;
    return;
  }
  if (mSetupStep == 2) {
    if (mSetupPassLen == 0) {
      StrCopy16(mSetupStatus, sizeof(mSetupStatus) / sizeof(mSetupStatus[0]), L"ENTER A PASSWORD");
      return;
    }
    mSetupStep = 3;
    return;
  }
  if (mSetupStep == 3) {
    if (mSetupPassLen != mSetupPassConfirmLen || !StrEq16(mSetupPass, mSetupPassConfirm)) {
      StrCopy16(mSetupStatus, sizeof(mSetupStatus) / sizeof(mSetupStatus[0]), L"PASSWORDS DO NOT MATCH");
      return;
    }
    ZeroState();
    mState.UserCount = 1;
    mState.ActiveUser = 0;
    mState.Flags |= STATE_FLAG_SETUP_DONE;
    StrCopy16(mState.Users[0].Name, USER_NAME_CAP, mSetupName);
    FillSalt(mState.Users[0].Salt);
    mState.Users[0].PassHash = HashPassword(mSetupPass, mSetupPassLen, mState.Users[0].Salt);
    mState.Users[0].IsAdmin = 1;
    SaveState();
    ClearSetupInput();
    ClearLoginInput();
    mUiMode = UI_DESKTOP;
    return;
  }
}

static VOID
SetupPrevStep(void) {
  if (mSetupStep > 0) {
    mSetupStep--;
  }
  mSetupStatus[0] = 0;
}

static VOID
LoginAttempt(void) {
  UINT32 hash;
  if (mState.UserCount == 0) {
    mUiMode = UI_SETUP;
    return;
  }
  if (mLoginUserIndex < 0 || mLoginUserIndex >= (INTN)mState.UserCount) {
    mLoginUserIndex = 0;
  }
  hash = HashPassword(mLoginPass, mLoginPassLen, mState.Users[mLoginUserIndex].Salt);
  if (hash == mState.Users[mLoginUserIndex].PassHash) {
    mState.ActiveUser = (UINT32)mLoginUserIndex;
    SaveState();
    ClearLoginInput();
    mUiMode = UI_DESKTOP;
  } else {
    StrCopy16(mLoginStatus, sizeof(mLoginStatus) / sizeof(mLoginStatus[0]), L"INVALID PASSWORD");
    mLoginPassLen = 0;
    mLoginPass[0] = 0;
  }
}

static VOID
HandleKeyInput(IN EFI_INPUT_KEY Key, OUT BOOLEAN *Running, OUT BOOLEAN *KeyboardClick) {
  if (mUiMode == UI_SETUP) {
    if (Key.UnicodeChar == L'\r') {
      SetupNextStep();
      return;
    }
    if (Key.ScanCode == SCAN_ESC) {
      return;
    }
    if (Key.UnicodeChar == 0x08) {
      if (mSetupStep == 1 && mSetupNameLen > 0) {
        mSetupName[--mSetupNameLen] = 0;
      } else if (mSetupStep == 2 && mSetupPassLen > 0) {
        mSetupPass[--mSetupPassLen] = 0;
      } else if (mSetupStep == 3 && mSetupPassConfirmLen > 0) {
        mSetupPassConfirm[--mSetupPassConfirmLen] = 0;
      }
      return;
    }
    if (Key.UnicodeChar >= 32 && Key.UnicodeChar <= 126) {
      if (mSetupStep == 1 && mSetupNameLen + 1 < USER_NAME_CAP) {
        mSetupName[mSetupNameLen++] = Key.UnicodeChar;
        mSetupName[mSetupNameLen] = 0;
      } else if (mSetupStep == 2 && mSetupPassLen + 1 < PASS_CAP) {
        mSetupPass[mSetupPassLen++] = Key.UnicodeChar;
        mSetupPass[mSetupPassLen] = 0;
      } else if (mSetupStep == 3 && mSetupPassConfirmLen + 1 < PASS_CAP) {
        mSetupPassConfirm[mSetupPassConfirmLen++] = Key.UnicodeChar;
        mSetupPassConfirm[mSetupPassConfirmLen] = 0;
      }
    }
    return;
  }

  if (mUiMode == UI_LOGIN) {
    if (Key.UnicodeChar == L'\r') {
      LoginAttempt();
      return;
    }
    if (Key.ScanCode == SCAN_UP) {
      if (mLoginUserIndex > 0) {
        mLoginUserIndex--;
      }
      mLoginStatus[0] = 0;
      return;
    }
    if (Key.ScanCode == SCAN_DOWN) {
      if (mLoginUserIndex + 1 < (INTN)mState.UserCount) {
        mLoginUserIndex++;
      }
      mLoginStatus[0] = 0;
      return;
    }
    if (Key.UnicodeChar == 0x08) {
      if (mLoginPassLen > 0) {
        mLoginPass[--mLoginPassLen] = 0;
      }
      return;
    }
    if (Key.UnicodeChar >= 32 && Key.UnicodeChar <= 126 && mLoginPassLen + 1 < PASS_CAP) {
      mLoginPass[mLoginPassLen++] = Key.UnicodeChar;
      mLoginPass[mLoginPassLen] = 0;
      return;
    }
    return;
  }

  if (Key.ScanCode == SCAN_ESC) {
    *Running = FALSE;
    return;
  }

  if (Key.ScanCode == SCAN_LEFT) {
    mCursorX = ClampU((INTN)mCursorX - KB_CURSOR_STEP, 0, mScreenW - 1);
    mKeyboardCursorTicks = 6;
    return;
  }
  if (Key.ScanCode == SCAN_RIGHT) {
    mCursorX = ClampU((INTN)mCursorX + KB_CURSOR_STEP, 0, mScreenW - 1);
    mKeyboardCursorTicks = 6;
    return;
  }
  if (Key.ScanCode == SCAN_UP) {
    mCursorY = ClampU((INTN)mCursorY - KB_CURSOR_STEP, 0, mScreenH - 1);
    mKeyboardCursorTicks = 6;
    return;
  }
  if (Key.ScanCode == SCAN_DOWN) {
    mCursorY = ClampU((INTN)mCursorY + KB_CURSOR_STEP, 0, mScreenH - 1);
    mKeyboardCursorTicks = 6;
    return;
  }

  if (Key.UnicodeChar == L' ' || Key.UnicodeChar == L'\r') {
    *KeyboardClick = TRUE;
    return;
  }

  if (mFocusedApp == APP_NOTES && mApps[APP_NOTES].Open) {
    HandleNotesKey(Key);
    return;
  }

  if (mFocusedApp == APP_FILES && mApps[APP_FILES].Open) {
    if (Key.UnicodeChar == L'o' || Key.UnicodeChar == L'O') {
      FilesOpenSelected();
    } else if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
      FilesClearSelected();
    }
    return;
  }

  if (mFocusedApp == APP_CALC && mApps[APP_CALC].Open) {
    HandleCalcKey(Key);
    return;
  }

  if (mFocusedApp == APP_TERMINAL && mApps[APP_TERMINAL].Open) {
    HandleTerminalKey(Key);
    return;
  }

  if (mFocusedApp == APP_SETTINGS && mApps[APP_SETTINGS].Open) {
    if (Key.UnicodeChar == L'[') {
      AdjustCursorSize(-2);
    } else if (Key.UnicodeChar == L']') {
      AdjustCursorSize(2);
    } else if (Key.UnicodeChar == L'-') {
      AdjustFrameDelay(2000);
    } else if (Key.UnicodeChar == L'=') {
      AdjustFrameDelay(-2000);
    } else if (Key.UnicodeChar == L'\t') {
      mSettingsEditField++;
      if (mSettingsEditField > 2) {
        mSettingsEditField = 0;
      }
    } else if (Key.UnicodeChar == L'\r') {
      WifiConnect();
    } else if (mSettingsEditField == 1) {
      UINTN len = StrLen16(mWifiSsid);
      if (Key.UnicodeChar == 0x08) {
        if (len > 0) {
          mWifiSsid[len - 1] = 0;
        }
      } else if (Key.UnicodeChar >= 32 && Key.UnicodeChar <= 126 && len + 1 < WIFI_SSID_CAP) {
        mWifiSsid[len] = Key.UnicodeChar;
        mWifiSsid[len + 1] = 0;
      }
    } else if (mSettingsEditField == 2) {
      UINTN len2 = StrLen16(mWifiPass);
      if (Key.UnicodeChar == 0x08) {
        if (len2 > 0) {
          mWifiPass[len2 - 1] = 0;
        }
      } else if (Key.UnicodeChar >= 32 && Key.UnicodeChar <= 126 && len2 + 1 < WIFI_PASS_CAP) {
        mWifiPass[len2] = Key.UnicodeChar;
        mWifiPass[len2 + 1] = 0;
      }
    }
    return;
  }

  if (mFocusedApp == APP_TASKS && mApps[APP_TASKS].Open) {
    if (Key.UnicodeChar == L'w' || Key.UnicodeChar == L'W') {
      if (mTaskSelected > 0) {
        mTaskSelected--;
      }
      return;
    }
    if (Key.UnicodeChar == L's' || Key.UnicodeChar == L'S') {
      if (mTaskSelected + 1 < APP_COUNT) {
        mTaskSelected++;
      }
      return;
    }
    if (Key.UnicodeChar == L'o' || Key.UnicodeChar == L'O') {
      TasksOpenSelected();
    } else if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
      TasksCloseSelected();
    }
    return;
  }

  if (mFocusedApp == APP_PHOTO && mApps[APP_PHOTO].Open) {
    if (Key.UnicodeChar == L'n' || Key.UnicodeChar == L'N') {
      if (mPhotoCount > 0) {
        mPhotoSelected = (mPhotoSelected + 1) % mPhotoCount;
      }
    } else if (Key.UnicodeChar == L'p' || Key.UnicodeChar == L'P') {
      if (mPhotoCount > 0) {
        mPhotoSelected = (mPhotoSelected == 0) ? (mPhotoCount - 1) : (mPhotoSelected - 1);
      }
    } else if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
      PhotoCaptureFromPaint();
    }
    return;
  }

  if (mFocusedApp == APP_SNAKE && mApps[APP_SNAKE].Open) {
    if (Key.UnicodeChar == L'r' || Key.UnicodeChar == L'R') {
      SnakeReset();
      return;
    }
    if (!mSnakeAlive) {
      return;
    }
    if ((Key.UnicodeChar == L'w' || Key.UnicodeChar == L'W') && mSnakeDir != 2) {
      mSnakeNextDir = 0;
    } else if ((Key.UnicodeChar == L'd' || Key.UnicodeChar == L'D') && mSnakeDir != 3) {
      mSnakeNextDir = 1;
    } else if ((Key.UnicodeChar == L's' || Key.UnicodeChar == L'S') && mSnakeDir != 0) {
      mSnakeNextDir = 2;
    } else if ((Key.UnicodeChar == L'a' || Key.UnicodeChar == L'A') && mSnakeDir != 1) {
      mSnakeNextDir = 3;
    }
    return;
  }

  if (mFocusedApp == APP_BROWSER && mApps[APP_BROWSER].Open) {
    if (Key.UnicodeChar == L'n' || Key.UnicodeChar == L'N') {
      mBrowserPage = (mBrowserPage + 1) % 3;
    } else if (Key.UnicodeChar == L'p' || Key.UnicodeChar == L'P') {
      mBrowserPage = (mBrowserPage + 2) % 3;
    }
    return;
  }

  if (mFocusedApp == APP_MAIL && mApps[APP_MAIL].Open) {
    if (Key.UnicodeChar == L's' || Key.UnicodeChar == L'S') {
      MailSyncStub();
    } else if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
      MailClear();
    } else if (Key.UnicodeChar == L'w' || Key.UnicodeChar == L'W') {
      if (mMailSelected > 0) {
        mMailSelected--;
      }
    } else if (Key.UnicodeChar == L'x' || Key.UnicodeChar == L'X') {
      if (mMailSelected + 1 < (INTN)MAIL_MAX) {
        mMailSelected++;
      }
    }
  }
}

EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status;
  EFI_INPUT_KEY key;
  BOOLEAN running;
  BOOLEAN keyboardClick;
  UINTN i;

  (VOID)ImageHandle;
  (VOID)SystemTable;

  status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&mGop);
  if (EFI_ERROR(status)) {
    Print(L"GOP unavailable.\r\n");
    return status;
  }

  mScreenW = mGop->Mode->Info->HorizontalResolution;
  mScreenH = mGop->Mode->Info->VerticalResolution;
  if (mScreenW < 320 || mScreenH < 240) {
    Print(L"Resolution too small: %ux%u.\r\n", (UINT32)mScreenW, (UINT32)mScreenH);
    return EFI_UNSUPPORTED;
  }

  status = InitBackBuffer();
  if (EFI_ERROR(status)) {
    Print(L"Backbuffer unavailable; using direct render.\r\n");
  }

  status = gBS->LocateProtocol(&gEfiSimplePointerProtocolGuid, NULL, (VOID **)&mSimplePointer);
  if (!EFI_ERROR(status) && mSimplePointer != NULL) {
    mSimplePointer->Reset(mSimplePointer, TRUE);
  } else {
    mSimplePointer = NULL;
  }

  status =
      gBS->LocateProtocol(&gEfiAbsolutePointerProtocolGuid, NULL, (VOID **)&mAbsolutePointer);
  if (!EFI_ERROR(status) && mAbsolutePointer != NULL) {
    mAbsolutePointer->Reset(mAbsolutePointer, TRUE);
  } else {
    mAbsolutePointer = NULL;
  }
  WifiInitProtocols();
  LoadState();

  ResetPaint();
  ClearNotes();
  mCalcExprLen = 0;
  mCalcExpr[0] = 0;
  CalcSetResultText(L"READY");
  TerminalInit();
  mPhotoCount = 0;
  mPhotoSelected = 0;
  mSnakeAlive = FALSE;
  mSnakeLen = 0;
  mSnakeScore = 0;
  mBrowserPage = 0;
  mSettingsEditField = 0;
  mWifiSsid[0] = 0;
  mWifiPass[0] = 0;
  WifiDisconnect();
  mMailSyncCount = 0;
  MailClear();
  mTaskSelected = APP_NOTES;
  mCursorSize = 12;
  mFrameDelayUs = 16666;
  for (i = 0; i < APP_COUNT; ++i) {
    mApps[i].Open = FALSE;
  }

  ClearSetupInput();
  ClearLoginInput();
  if ((mState.Flags & STATE_FLAG_SETUP_DONE) == 0 || mState.UserCount == 0) {
    mUiMode = UI_SETUP;
  } else {
    mUiMode = UI_LOGIN;
  }

  running = TRUE;
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  Print(L"Desktop ready: double-click icons, drag title bars, resize lower-right.\r\n");
  Print(L"Arrow keys move cursor, Space/Enter click, type in apps, ESC exits.\r\n");
  Print(L"Apps: PHOTO SNAKE MON WEB MAIL. SETTINGS uses WIFI protocols.\r\n");
  if (mSimplePointer == NULL && mAbsolutePointer == NULL) {
    Print(L"No UEFI pointer protocol found; keyboard-mouse mode is active.\r\n");
  }
  if (mWiFi2 != NULL) {
    Print(L"WiFi2 protocol detected.\r\n");
  } else if (mWiFi != NULL) {
    Print(L"WiFi protocol detected (legacy).\r\n");
  } else {
    Print(L"No WiFi protocol detected.\r\n");
  }

  while (running) {
    keyboardClick = FALSE;

    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &key) == EFI_SUCCESS) {
      HandleKeyInput(key, &running, &keyboardClick);
    }

    if (mKeyboardCursorTicks > 0) {
      mKeyboardCursorTicks--;
    }

    ReadPointerState();

    if (!mPrevPointerDown && mPointerDown) {
      HandlePointerDownAction();
    }
    if (keyboardClick) {
      HandlePointerDownAction();
    }

    if (mPointerDown) {
      HandlePointerHold();
    } else {
      mDraggingApp = -1;
      mResizingApp = -1;
      mPainting = FALSE;
    }

    if (mApps[APP_SNAKE].Open) {
      SnakeStep();
    }

    mPrevPointerDown = mPointerDown;
    RenderFrame();
    mTick++;
    gBS->Stall(mFrameDelayUs);
  }

  FreeBackBuffer();
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  return EFI_SUCCESS;
}
