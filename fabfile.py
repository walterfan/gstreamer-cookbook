import os, sys, subprocess
import http.server
import socketserver
import markdown
import codecs
from datetime import date
from sys import platform
from fabric import task
from fabric import Connection

BASE_PATH = os.path.dirname(__file__)
default_hosts = ["localhost"]


def md_to_html(md_file, html_file ):

    input_file = codecs.open(md_file, mode="r", encoding="utf-8")
    text = input_file.read()

    html = markdown.markdown(text, extensions=['markdown.extensions.tables'])

    output_file = codecs.open(html_file, "w", encoding="utf-8")
    output_file.write(html)

@task(hosts=default_hosts)
def usage(c):
    print("usage: fab make_note|publish_note|md2rst|rst2md")


@task(hosts=['localhost'])
def md2rst(c, src, dest=None):
    if not dest:
        dest = src[:-3] + ".rst";
    cmd = "pandoc --to RST --reference-links {} > {}".format(src, dest)
    c.local(cmd)


@task(hosts=default_hosts)
def rst2md(c, src, dest=None):
    if not dest:
        dest = src[:-4] + ".md";
    cmd = "pandoc {} -f rst -t markdown -o {}".format(src, dest)
    c.local(cmd)

@task(hosts=default_hosts)
def make_note(c):
    build_cmd = 'cd doc && make clean html'
    c.local(build_cmd)



@task(hosts=default_hosts)
def publish_note(c):
    c.local("cd doc && touch ./build/html/.nojekyll")
    c.local("git add doc")
    c.local('git commit -m "update notes"')
    c.local("git subtree push --prefix doc/build/html origin gh-pages")
