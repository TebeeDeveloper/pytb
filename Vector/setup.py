from setuptools import setup, Extension
from Cython.Build import cythonize

# Định nghĩa extension
ext = Extension(
    "vector",           # Tên module sau khi import
    ["vector.pyx", './source/vector_int.c'],     # File code của bạn
    extra_compile_args=[
        "-Ofast",
        "-fno-plt"
    ],
    py_limited_api=True,   # Đây là chìa khóa để chạy mọi version >= 3.7
)

setup(
    ext_modules=cythonize(ext, compiler_directives={'language_level': "3"}),
)