import os
import urllib
import io
import zipfile
import sys

temp_dir = './'
libs = [] # Add lib with addLib function
download_all = False # if False, check if files exists

def _url_report(blocknr, blocksize, size):
    current = blocknr * blocksize
    sys.stdout.write("\r{0:.2f}%".format(100.0*current/size))

def _url_download_file(url, saveAs):
    print("\nDownloading " + url + "\n")
    if(not os.path.isdir(os.path.dirname(saveAs))):
        os.makedirs(os.path.dirname(saveAs))
    urllib.urlretrieve(url, saveAs, _url_report)
    print("\n")
    
def _unzip(what, where):
    print("Unpacking "+what+"\n")
    if(not os.path.isdir(where)):
        os.makedirs(where)
    with zipfile.ZipFile(what, "r") as z:
        z.extractall(where)
        """
#glm example
glm_url = 'https://github.com/g-truc/glm/archive/master.zip'
glm_file = 'glm_master.zip'
glm_target_file = os.path.join(temp_dir, glm_file)
_url_download_file(glm_url, glm_target_file)
_unzip(glm_target_file, temp_dir)

zip.write(urllib.request.urlopen(url).read())
"""

def addLib(url, temp_file):
    global libs
    libs += [{
        'url' : url,
        'temp_file' : temp_file
        }]

def processNextLib():
    global libs, temp_dir, download_all
    lib = libs[0]
    target_file = os.path.join(temp_dir, lib['temp_file'])
    if(download_all or (not os.path.exists(target_file))):
        _url_download_file(lib['url'], target_file)
        _unzip(target_file, temp_dir)
    libs.pop(0)

addLib('https://github.com/g-truc/glm/archive/master.zip', 'glm_master.zip')

def main():
    while(len(libs) != 0):
        processNextLib()

if __name__ == '__main__':
    main()