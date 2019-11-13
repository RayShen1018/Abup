@echo off 
setlocal EnableDelayedExpansion

set bin_patch=%1
set zip_patch=%2
set def_path=%3
set version=version
set fota=IOT5.0_LUSUN11_R50426
set algorithm=0
set base=0
set bl=0
set app=0
set user=0
set addr=0
set block=0
set wosun=0
set addr1=0
set size=0
set lusun_ver=0
set wosun_ver=0
set app_ver=0
set abup=abup.bin
set fromelf=fromelf.exe

if not "x!bin_patch:%fromelf%=!"=="x%bin_patch%" (
	%bin_patch% --bin -o %4\%abup% %4\%4.axf
) else (
    echo N
)
for /f "tokens=2 delims==" %%a in ('wmic path win32_operatingsystem get LocalDateTime /value') do (
  set datetime=%%a
)
set d=%datetime:~0,4%%datetime:~4,2%%datetime:~6,2%
set h=%time:~0,2%
set h=%h: =0%
set t=%h%%time:~3,2%%time:~6,2%

call:findstr1 ABUP_FIRMWARE_VERSION %def_path% version
call:findstr1 ADUPS_FOTA_LUSUN_VERSION %def_path% lusun_ver
call:findstr1 ADUPS_FOTA_WOSUN_VERSION %def_path% wosun_ver
call:findstr2 ABUP_FOTA_ALGORITHM %def_path% algorithm
call:findstr2 ABUP_FLASH_BASE_ADDR %def_path% base
call:findstr2 ABUP_BL_SIZE %def_path% bl
call:findstr2 ABUP_APP_SIZE %def_path% app
call:findstr2 ABUP_USER_DATA_SIZE %def_path% user
call:findstr2 ABUP_DEFAULT_SECTOR_SIZE %def_path% block
call:findstr2 ABUP_UPDATE_SIZE %def_path% size
call:findstr2 ABUP_WOSUN_STREAM_TYPE %def_path% wosun
	
for  %%s in ( %def_path%) do (
    findstr  "ADUPS_FOTA_SERVICE" %%s >adups_info.txt
    set /p line=<adups_info.txt
) 

if "%algorithm%"=="0" (
	echo #define ADUPS_FOTA_SDK_VER	"%lusun_ver%" >> adups_info.txt
) else (
	echo #define ADUPS_FOTA_SDK_VER	"%wosun_ver%" >> adups_info.txt
)

if "%5"=="RTT" (
call:findstr1 ADUPS_FOTA_RTT_VERSION %def_path% app_ver
echo RTT
) else (
call:findstr1 ADUPS_FOTA_APP_VERSION %def_path% app_ver
echo not RTT
)
echo %app_ver%
echo #define ADUPS_FOTA_APP_VER	"%app_ver%" >> adups_info.txt
set /A addr=base+bl
call:hexfun %addr% addr1
set "addr1=00000000%addr1%"
set "addr1=%addr1:~-8,8%"
set /A size=size/1024
set /A block=block/1024
echo abup fota used size=%size%
set /A size=size-2*block
echo abup fota delta size=%size%
	
if "%algorithm%"=="0" (echo lusun) else (
	if "%wosun%"=="0" (echo wosun bz) else (echo wosun lzma)
    echo #define ADUPS_FOTA_SDK_MAX_MCU_NUM	"" >> adups_info.txt
    echo #define ADUPS_FOTA_FILENAME1 "%abup%-D" >> adups_info.txt
    echo #define ADUPS_FOTA_ADDRESS1   "0x%addr1%" >> adups_info.txt
    echo #define ADUPS_FOTA_REGION_LEN   "%size%" >> adups_info.txt
    echo #define ADUPS_FOTA_BLOCK_LEN "%block%" >> adups_info.txt
    echo #define ADUPS_FOTA_PATCH_FORMAT   "%wosun%" >> adups_info.txt
    echo #define ADUPS_BUILD_TIME   "%d%_%t%" >> adups_info.txt
)
echo version=%version% >> adups_info.txt

echo+%zip_patch%|findstr "7z.exe" 
if %errorlevel% equ 0 (
copy %4\%abup% %abup%
%zip_patch% a -tzip %version%_%d%_%t%.zip adups_info.txt %abup%
del %abup% /s
) else (
%zip_patch% a -ep1 -o+ -inul -r  -iback %version%_%d%_%t%.zip adups_info.txt %4\%abup%
) 
del adups_info.txt /s
echo complete
GOTO:EOF

:findstr1
	for /f tokens^=^2^ delims^=^"^= %%i in ('findstr "%1" %2') do set "%3=%%i"
GOTO:EOF
:findstr2
	for /f "tokens=2,3" %%i in ('findstr "%1" %2') do if %%i==%1 set /A "%3=%%j"
GOTO:EOF

:hexfun
	set str=
	set code=0123456789ABCDEF
    set "var=%1"
	:again
	set /a tra=%var%%%16
	call,set tra=%%code:~%tra%,1%%
	set /a var/=16
	call:set_YU %var%
	set str=%tra%%str%
	if %var% geq 16 goto again
    set "%2=%ret%%str%"

GOTO:EOF

:set_YU
set ret=
if "%1" == "10" set ret=A
if "%1" == "11" set ret=B
if "%1" == "12" set ret=C
if "%1" == "13" set ret=D
if "%1" == "14" set ret=E
if "%1" == "15" set ret=F
if %1 lss 10 set ret=%1
GOTO:EOF