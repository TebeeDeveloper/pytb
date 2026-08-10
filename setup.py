from setuptools import Extension, setup
from Cython.Build import cythonize
import os

ext_modules = cythonize(
    Extension(
        name="pytb.Vector",
        sources=[
            "Vector/vector.pyx",
            "Vector/source/*.c"
        ],
        include_dirs=["Vector/include"],
        extra_compile_args=["-O3", "-march=native"], # Tối ưu tốc độ
    ),
    language_level="3"
)

setup(
    ext_modules=ext_modules,
    packages=["pytb.Vector"],
    package_dir={"pytb.Vector": "Vector"}, # Nối thư mục Vector thành gói pytb.Vector
    package_data={"pytb.Vector": ["*.pyi"]}, # Gom stub vào gói
    zip_safe=False
)