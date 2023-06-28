#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from urllib.parse import quote
import zipfile

EXCLUDE_DIRS = [
    ".git",
    "docs",
    ".vscode",
    "overrides",
    ".github",
    "script",
    "images",
    "zips",
    "site",
]
README_MD = ["README.md", "readme.md", "index.md"]

TXT_EXTS = ["md", "txt"]
TXT_URL_PREFIX = "https://github.com/beiyuouo/hainanu-course-comments/blob/main/"
BIN_URL_PREFIX = "https://github.com/beiyuouo/hainanu-course-comments/raw/main/"
CDN_PREFIX = "https://dl.capoo.xyz/"
CDN_RAW_PREFIX = "https://github.com/beiyuouo/hainanu-course-comments/blob/zips/"


def make_zip(dir_path, zip_path):
    zip = zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED)
    for path, dirnames, filenames in os.walk(dir_path):
        fpath = path.replace(dir_path, "")
        for filename in filenames:
            zip.write(os.path.join(path, filename), os.path.join(fpath, filename))
    zip.close()


def size_format(size: int):
    if size < 1024:
        return "{}B".format(size)
    elif size < 1024 * 1024:
        return "{:.2f}KB".format(size / 1024)
    elif size < 1024 * 1024 * 1024:
        return "{:.2f}MB".format(size / 1024 / 1024)
    else:
        return "{:.2f}GB".format(size / 1024 / 1024 / 1024)


def get_file_size(path: str):
    return size_format(os.path.getsize(path))


def list_files(course: str):
    filelist_texts = "## 文件列表\n\n"
    filelist_texts_cdn = "### 一键下载（CDN加速）\n\n"
    zip_path = os.path.join("zips", "{}.zip".format(course))
    print(course, get_file_size(zip_path))
    filelist_texts_cdn += f"- [{os.path.basename(course)}.zip({get_file_size(zip_path)})]({CDN_PREFIX}/{CDN_RAW_PREFIX}/{course}.zip)\n\n"

    filelist_texts_org = "### GitHub原始链接\n\n"
    filelist_texts_org += f"- [{os.path.basename(course)}.zip({get_file_size(zip_path)})]({CDN_RAW_PREFIX}{course}.zip)\n\n"

    readme_path = ""
    for root, dirs, files in os.walk(course):
        files.sort()
        level = root.replace(course, "").count(os.sep)
        indent = " " * 4 * level
        filelist_texts_org += "{}- {}\n".format(indent, os.path.basename(root))
        subindent = " " * 4 * (level + 1)
        for f in files:
            if f not in README_MD:
                if f.split(".")[-1] in TXT_EXTS:
                    filelist_texts_org += "{}- [{}]({})\n".format(
                        subindent, f, TXT_URL_PREFIX + quote("{}/{}".format(root, f))
                    )
                else:
                    filelist_texts_org += "{}- [{}]({})\n".format(
                        subindent, f, BIN_URL_PREFIX + quote("{}/{}".format(root, f))
                    )
            elif root == course and readme_path == "":
                readme_path = "{}/{}".format(root, f)
    return filelist_texts + filelist_texts_cdn + filelist_texts_org, readme_path


def generate_md(course: str, filelist_texts: str, readme_path: str, topic: str):
    final_texts = ["\n\n".encode(), filelist_texts.encode()]
    if readme_path:
        with open(readme_path, "rb") as file:
            final_texts = file.readlines() + final_texts
    topic_path = os.path.join("docs", topic)
    if not os.path.isdir(topic_path):
        os.mkdir(topic_path)
    with open(os.path.join(topic_path, "{}.md".format(course)), "wb") as file:
        file.writelines(final_texts)


if __name__ == "__main__":
    if not os.path.isdir("docs"):
        os.mkdir("docs")

    if not os.path.isdir("zips"):
        os.mkdir("zips")

    global PROJECT_PATH
    PROJECT_PATH = os.path.abspath(__file__)

    topics = list(
        filter(lambda x: os.path.isdir(x) and (x not in EXCLUDE_DIRS), os.listdir("."))
    )  # list topics

    for topic in topics:
        topic_path = os.path.join(".", topic)
        if not os.path.isdir(os.path.join("zips", topic)):
            os.mkdir(os.path.join("zips", topic))

        courses = list(
            filter(
                lambda x: os.path.isdir(os.path.join(topic_path, x))
                and (x not in EXCLUDE_DIRS),
                os.listdir(topic_path),
            )
        )  # list courses

        for course in courses:
            course_path = os.path.join(".", topic, course)

            make_zip(course_path, os.path.join("zips", topic, "{}.zip".format(course)))

            # split zip files limit 100MB
            if (
                os.path.getsize(os.path.join("zips", topic, "{}.zip".format(course)))
                > 100 * 1024 * 1024
            ):
                os.system(
                    "split -b 100M {} {}".format(
                        os.path.join("zips", topic, "{}.zip".format(course)),
                        os.path.join("zips", topic, "{}.zip.".format(course)),
                    )
                )
                os.remove(os.path.join("zips", topic, "{}.zip".format(course)))
                os.remove(os.path.join("zips", topic, "{}.zip.aa".format(course)))

            filelist_texts, readme_path = list_files(course_path)

            generate_md(course, filelist_texts, readme_path, topic)

    with open("README.md", "rb") as file:
        mainreadme_lines = file.readlines()

    with open("docs/index.md", "wb") as file:
        file.writelines(mainreadme_lines)
