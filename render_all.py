#!/usr/bin/env python3
import os
from platform import platform
import sys
import subprocess

def render(in_file, out_dir, cache_dir):
    base = os.path.basename(in_file)
    stem = os.path.splitext(base)[0]
    out_file = os.path.join(out_dir, stem + '.bmp')
    cache_file = os.path.join(cache_dir, stem + '.bvh')
    print(f'Rendering {in_file}')

    program = os.path.join(os.path.dirname(__file__), 'x64', 'Release', 'PathTracer.exe')
    args = [in_file, out_file, cache_file]
    if sys.platform == 'win32':
        args.insert(0, os.path.join(os.path.dirname(__file__), 'x64', 'Release', 'PathTracer.exe'))
    elif sys.platform == 'linux':
        args.insert(0, os.path.join(os.path.dirname(__file__), 'x64', 'Release', 'PathTracer'))
    subprocess.run(args, stderr=sys.stderr, stdout=sys.stdout)

def main():
    scene_dir = os.path.join(os.path.dirname(__file__), 'scenes')
    scene_files = os.listdir(scene_dir)

    out_dir = os.path.join(os.path.dirname(__file__), 'out')
    if not os.path.isdir(out_dir):
        os.mkdir(out_dir)

    cache_dir = os.path.join(os.path.dirname(__file__), 'cache')
    if not os.path.isdir(cache_dir):
        os.mkdir(cache_dir)

    for scene_file in scene_files:
        in_file = os.path.join(scene_dir, scene_file)
        render(in_file, out_dir, cache_dir)

if __name__ == '__main__':
    main()
