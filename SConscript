from building import *

cwd = GetCurrentDir()
path = [cwd + '/inc']
path += [cwd + '/inc/App']


if GetDepend(['PKG_USING_ABUP_FOTA']):
    src  = Glob('src/abup_config.c')
    src  += Glob('src/abup_hal_uart.c')
    src  += Glob('src/abup_hal.c')
    src  += Glob('src/abup_client.c')
    src  += Glob('src/abup_res.c')

CPPDEFINES = ['ABUP_RTTHREAD']
LIBPATH = [cwd + '/Lib/App']
LIBS = ['arm_cortexM4_AbupFotaApp']
src1 = Split('''
''')

group = DefineGroup('Abup', src, depend = ['PKG_USING_ABUP_FOTA'], CPPPATH = path, CPPDEFINES = CPPDEFINES)
group = DefineGroup('Abup/Lib', src1, depend = ['PKG_USING_ABUP_FOTA'], LIBS = LIBS, LIBPATH=LIBPATH)

Return('group')
