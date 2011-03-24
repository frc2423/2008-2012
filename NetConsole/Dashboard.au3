
#Include <Constants.au3>
#Include <WinAPI.au3>

Run( "NetConsole.exe", "", @SW_MAXIMIZE )

WinWait( "NetConsole.vi" )
;WinMove( "NetConsole.vi", "", 0, 0, 1024, 300 )

$h = 450
$w = 734

$hWnd = WinGetHandle( "NetConsole.vi" )
_WinAPI_SetWindowPos( $hWnd, $HWND_BOTTOM, 0, 0, $w, $h, $SWP_NOZORDER )

Sleep(1000)

MouseClickDrag( "left", $w-1, $h/2, 1024, $h/2 )

MouseClick( "right", 1024/2, 500 )

