#!/usr/bin/env python3

import os, subprocess


def list_files(directory, *allowed_extensions):
    directory = "/".join(directory.split("/"))
    files = []
    for file in os.listdir(directory):
        full_path = "/".join([directory, file])
        if os.path.isdir(full_path):
            files.extend(list_files(full_path, *allowed_extensions))
        elif not allowed_extensions or full_path.split(".")[-1] in allowed_extensions:
            files.append(full_path)
    return files


def format_py_files():
    file_extensions = ["py"]
    files_to_format = list_files("scripts", *file_extensions)
    files_to_format += list_files("src", *file_extensions)
    for file_to_format in files_to_format:
        if __file__ not in file_to_format:
            subprocess.run(["black", file_to_format])


if __name__ == "__main__":
    format_py_files()
