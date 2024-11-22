import os
import shutil
import multiprocessing

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))

def build_dir(dir):
    os.system(f"cmake --build {dir} --config Debug")

def test_clang():
    build_clang = os.path.join(PROJECT_DIR, "build-clang")
    os.system(
        f"cmake -S {os.path.join(PROJECT_DIR, 'test')} -B {build_clang} -Wno-dev "
        f"-G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang "
        f"> {os.devnull}"
    )
    build_dir(build_clang)

def test_gcc():
    build_gcc = os.path.join(PROJECT_DIR, "build-gcc")
    os.system(
        f"cmake -S {os.path.join(PROJECT_DIR, 'test')} -B {build_gcc} -Wno-dev "
        f"-G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc "
        f"> {os.devnull}"
    )
    build_dir(build_gcc)

def test_msvc():
    build_msvc = os.path.join(PROJECT_DIR, "build-msvc")
    os.system(
        f"cmake -S {os.path.join(PROJECT_DIR, 'test')} -B {build_msvc} -G Ninja "
        f"> {os.devnull}"
    )
    build_dir(build_msvc)

if __name__ == '__main__':
    for dir in os.listdir(PROJECT_DIR):
        if dir.startswith('build'):
            shutil.rmtree(os.path.join(PROJECT_DIR, dir))

    tests = [
        multiprocessing.Process(target=test_gcc),
        multiprocessing.Process(target=test_clang),
        multiprocessing.Process(target=test_msvc),
    ]
    for t in tests:
        t.start()

    for t in tests:
        t.join()


