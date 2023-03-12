import os, os.path
import urllib.parse

from gi.repository import Nautilus, GObject

QLDD_KEY = '/usr/bin/Qldd'

class OpenQlddExtension(GObject.GObject, Nautilus.MenuProvider):
    def __init__(self):
        pass

    def _open_qldd(self, file):
        filename = urllib.parse.unquote(file.get_uri()[7:])
        qldd = QLDD_KEY
        os.system('%s %s &' % (qldd, filename))

    def menu_activate_cb(self, menu, files):
        for file in files:
            self._open_qldd(file)

    def get_file_items(self, files):
        item = Nautilus.MenuItem(
            name='Qldd',
            label='View dependencies',
            icon='/usr/share/icons/qldd/Qldd.png')
        item.connect('activate', self.menu_activate_cb, files)
        return item,
