from setuptools import setup, find_packages

setup(
    name="Arm_Lib",
    version="0.0.5",
    author="Yahboom Team",
    packages=find_packages(),
    install_requires=["pyserial>=3.0"],
    python_requires=">=3.7",
)
