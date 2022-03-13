#!/usr/bin/env python3
import os
import sys
import subprocess

def render(in_file, out_dir):
    base = os.path.basename(in_file)
    stem = os.path.splitext(base)[0]
    out_file = os.path.join(out_dir, stem + '.bmp')
    print(f'Rendering {in_file}')

    program = os.path.join(os.path.dirname(__file__), 'x64', 'Release', 'PathTracer.exe')
    subprocess.run([program, in_file, out_file], stderr=sys.stderr, stdout=sys.stdout)

def main():
    scene_dir = os.path.join(os.path.dirname(__file__), 'scenes')
    scene_files = os.listdir(scene_dir)

    out_dir = os.path.join(os.path.dirname(__file__), 'out')
    if not os.path.isdir(out_dir):
        os.mkdir(out_dir)

    for scene_file in scene_files:
        in_file = os.path.join(scene_dir, scene_file)
        render(in_file, out_dir)

if __name__ == '__main__':
    main()
