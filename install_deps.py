#!/usr/bin/env python3
from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile
import os

def install_zip(url):
    deps = os.path.join(os.path.dirname(__file__), 'deps')
    with urlopen(url) as response:
        with ZipFile(BytesIO(response.read())) as zipfile:
                zipfile.extractall(deps)

def main():
    install_zip('https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip')
    install_zip('https://github.com/zeux/pugixml/releases/download/v1.12/pugixml-1.12.zip')

if __name__ == '__main__':
    main()
