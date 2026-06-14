import os
import urllib.parse

from gi.repository import Nautilus, GObject

QLDD_KEY = '/usr/bin/Qldd'


class OpenQlddExtension(GObject.GObject, Nautilus.MenuProvider):
    def __init__(self):
        pass

    def _open_qldd(self, file):
        filename = urllib.parse.unquote(file.get_uri()[7:])
        os.system('%s %s &' % (QLDD_KEY, filename))

    def menu_activate_cb(self, menu, files):
        for file in files:
            self._open_qldd(file)

    # Nautilus 43+ removed the `window` parameter from get_file_items.
    # Using *args makes this extension work on both old and new API:
    #   Nautilus <43 calls get_file_items(window, files)  -> args = (window, files)
    #   Nautilus 43+ calls get_file_items(files)          -> args = (files,)
    def get_file_items(self, *args):
        files = args[-1]
        item = Nautilus.MenuItem(
            name='Qldd',
            label='View dependencies',
            icon='/usr/share/icons/qldd/Qldd.png')
        item.connect('activate', self.menu_activate_cb, files)
        return item,
