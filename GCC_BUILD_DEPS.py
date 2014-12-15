import sys
import os
import shutil
import urllib
import zipfile

make_prog = ''
dir_morglodsrender = ''
dir_gcc = ''
dir_glew_src = ''
dir_soil_src = ''


def url_report(blocknr, blocksize, size):
    current = blocknr*blocksize
    sys.stdout.write("\r{0:.2f}%".format(100.0*current/size))

def url_downloadFile(url, saveTo):
    print "\n", "Downloading ", url, "\n"
    if(not os.path.isdir(os.path.dirname(saveTo))):
        os.makedirs(os.path.dirname(saveTo))
    urllib.urlretrieve(url, saveTo, url_report)
    print "\n"

def zip_unzip(what, where):
    print "Unpacking ", what, "\n"
    if(not os.path.isdir(where)):
        os.makedirs(where)
    with zipfile.ZipFile(what, "r") as z:
        z.extractall(where)

def SetupPaths(MRdir):
    global make_prog, dir_morglodsrender, dir_gcc, dir_glew_src, dir_soil_src
    dir_morglodsrender = MRdir

    for p in os.environ["PATH"].split(os.pathsep):
        if(os.path.exists(os.path.join(p, 'gcc.exe'))):
            dir_gcc = os.path.dirname(p)
            if(os.path.join(p, 'mingw32-make.exe')):
                make_prog = 'mingw32-make'
            else:
                if(os.path.join(p, 'make.exe')):
                    make_prog = 'make'
            break

    if(dir_gcc != ''):
        print 'gcc found "' + os.path.abspath(dir_gcc).replace('\\', '/') + '"'
    else:
        dir_gcc = os.path.abspath(raw_input('gcc dir: ')).replace('\\', '/')

    if(make_prog != ''):
        print 'make found "' + make_prog + '"'
    else:
        make_prog = raw_input('make: ')

    print "\n"
    where_to_download = ''
    if(raw_input('download glew sources? (yes/no): ') == 'yes'):
        if(where_to_download == ''):
            where_to_download = os.path.abspath(raw_input('download to dir: '))
        url_downloadFile(url = 'http://downloads.sourceforge.net/project/glew/glew/1.11.0/glew-1.11.0.zip?r=http%3A%2F%2Fglew.sourceforge.net%2F&ts=1417792203&use_mirror=netcologne', saveTo = os.path.join(where_to_download, 'glew-1.11.0.zip'))
        zip_unzip(what = os.path.join(where_to_download, 'glew-1.11.0.zip'), where = where_to_download)
        dir_glew_src = os.path.abspath(os.path.join(where_to_download, 'glew-1.11.0')).replace('\\', '/')
    else:
        dir_glew_src = os.path.abspath(raw_input('glew source dir: ')).replace('\\', '/')

    print "\n"
    if(raw_input('download SOIL sources? (yes/no): ') == 'yes'):
        if(where_to_download == ''):
            where_to_download = os.path.abspath(raw_input('download to dir: '))
        url_downloadFile(url = 'http://www.lonesock.net/files/soil.zip', saveTo = os.path.join(where_to_download, 'soil.zip'))
        zip_unzip(what = os.path.join(where_to_download, 'soil.zip'), where = where_to_download)
        os.rename(os.path.abspath(os.path.join(where_to_download, 'Simple OpenGL Image Library')), os.path.abspath(os.path.join(where_to_download, 'soil')))
        dir_soil_src = os.path.abspath(os.path.join(where_to_download, 'soil')).replace('\\', '/')
    else:
        dir_soil_src = os.path.abspath(raw_input('SOIL source dir: ')).replace('\\', '/')

    print "\n"
    if(raw_input('download Assimp sources? (yes/no): ') == 'yes'):
        if(where_to_download == ''):
            where_to_download = os.path.abspath(raw_input('download to dir: '))
        url_downloadFile(url = 'http://downloads.sourceforge.net/project/assimp/assimp-3.1/assimp-3.1.1.zip?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fassimp%2Ffiles%2Fassimp-3.1%2F&ts=1418654905&use_mirror=heanet', saveTo = os.path.join(where_to_download, 'assimp.zip'))
        zip_unzip(what = os.path.join(where_to_download, 'assimp.zip'), where = where_to_download)

    print "\n"

def InstallLib(libDir, libName):
    global dir_gcc
    print 'installing ', libName
    shutil.copyfile(os.path.join(libDir, libName).replace('\\', '/'), os.path.join(os.path.join(dir_gcc, 'lib'), libName))

def BuildGlew():
    global dir_glew_src, make_prog, dir_gcc

    print "Building glew"
    _localdir = os.getcwd()
    os.chdir(dir_glew_src)

    if(not os.path.exists(os.path.abspath(os.path.join(dir_glew_src, 'lib/libglew32.a')))):
        os.system(make_prog)

    if(not os.path.exists(os.path.join(os.path.join(dir_gcc, 'lib'), 'libglew32.a'))):
        InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32.a')
        InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32.dll.a')
        InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32mx.a')
        InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32mx.dll.a')

    os.chdir(_localdir)
    print "\n"

def BuildSOIL():
    global dir_soil_src, make_prog, dir_gcc

    print "Building SOIL"
    _localdir = os.getcwd()
    os.chdir(dir_soil_src)

    if(not os.path.exists(os.path.join(os.path.join(dir_gcc, 'lib'), 'libSOIL.a'))):
        InstallLib(os.path.join(dir_soil_src, 'lib'), 'libSOIL.a')

    os.chdir(_localdir)
    print "\n"

def Build():
    SetupPaths(MRdir = os.getcwd())
    BuildGlew()
    BuildSOIL()
    print "\n\n", 'FINISHED, dont forget to build Assimp manually lol'
    print "\nWhen building assimp, don't forget to fix IFC importer 'add_definitions(-DASSIMP_BUILD_NO_IFC_IMPORTER)'"


if __name__ == "__main__":
    Build()
