Import("env")
import shutil

print("Copying hardware.h")
shutil.copyfile(
    env["PROJECT_DIR"] + '/include/hardware.h',
    env["PROJECT_LIBDEPS_DIR"] + '/' + env["PIOENV"] + '/OGM-Common/include/hardware.h')
print("Copying knxprod.h")
shutil.copyfile(
    env["PROJECT_DIR"] + '/include/knxprod.h',
    env["PROJECT_LIBDEPS_DIR"] + '/' + env["PIOENV"] + '/OGM-Common/include/knxprod.h')