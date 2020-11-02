from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("worker",
                         sources=["worker.pyx"],
                         language="C++",
                         include_dirs=["include", "/usr/include"],
                         library_dirs=["lib"],
                         libraries=["iex", ],
                         extra_compile_args=['-lstdc++', '-std=c++17', '-v'],
                         extra_link_args=['-lstdc++', '-lboost_iostreams', '-v'],
                         )]

setup(
    name="worker",
    cmdclass={"build_ext": build_ext},
    ext_modules=ext_modules
)
