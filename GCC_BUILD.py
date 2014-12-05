import sys
import os
import shutil
import urllib
import zipfile

make_prog = ''
dir_morglodsrender = ''
dir_gcc = ''
dir_glew_src = ''

def url_report(blocknr, blocksize, size):
    current = blocknr*blocksize
    sys.stdout.write("\r{0:.2f}%".format(100.0*current/size))

def url_downloadFile(url, saveTo):
    print "\n", url, "\n"
    urllib.urlretrieve(url, saveTo, url_report)

def zip_unzip(what, where):
    if(False == os.path.isdir(where)):
        os.makedirs(where)
    with zipfile.ZipFile(what, "r") as z:
        z.extractall(where)

def SetupPaths(MRdir):
    global make_prog, dir_morglodsrender, dir_gcc, dir_glew_src
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
        print 'gcc found "' + dir_gcc + '"'
    else:
        dir_gcc = raw_input('gcc dir: ')

    if(make_prog != ''):
        print 'make found "' + make_prog + '"'
    else:
        make_prog = raw_input('make: ')

    if(raw_input('download glew sources? (yes/no): ') == 'yes'):
        where_to_download = raw_input('download to dir: ')
        url_downloadFile(url = 'http://downloads.sourceforge.net/project/glew/glew/1.11.0/glew-1.11.0.zip?r=http%3A%2F%2Fglew.sourceforge.net%2F&ts=1417792203&use_mirror=netcologne', saveTo = os.path.join(where_to_download, 'glew-1.11.0.zip'))
        zip_unzip(what = os.path.join(where_to_download, 'glew-1.11.0.zip'), where = where_to_download)
        dir_glew_src = os.path.join(where_to_download, 'glew-1.11.0')
    else:
        dir_glew_src = raw_input('glew source dir: ')


def InstallLib(libDir, libName):
    global dir_gcc
    shutil.copyfile(os.path.join(libDir, libName), os.path.join(os.path.join(dir_gcc, '/lib/'), libName))

def BuildGlew():
    global dir_glew_src, make_prog
    _localdir = os.getcwd()
    os.chdir(dir_glew_src)
    os.system(make_prog)
    InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32.a')
    InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32.dll.a')
    InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32mx.a')
    InstallLib(os.path.join(dir_glew_src, 'lib'), 'libglew32mx.dll.a')
    os.chdir(_localdir)

def Build():
    SetupPaths(MRdir = os.getcwd())
    BuildGlew()


if __name__ == "__main__":
    Build()
