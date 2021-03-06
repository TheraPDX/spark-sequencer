#!/usr/bin/env python

PROJECT = 'sequencer'
VERSION = '0.1'

from setuptools import setup, find_packages


long_description = ''

setup(
    name=PROJECT,
    version=VERSION,
    description='flask webapp for communicating with spark-core light sequencer',
    long_description=long_description,
    author='Kyle Marsh',
    author_email='kyle.marsh@quixoticflame.net',
    url='...',
    download_url='...',
    scripts=[],
    provides=[],
    install_requires=['flask', 'requests'],
    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,
    )
