
import os, os.path
import urllib

from gi.repository import Nautilus, GObject, GConf

QLDD_KEY = '/usr/bin/Qldd'

class OpenQlddExtension(Nautilus.MenuProvider, GObject.GObject):
    def __init__(self):
        pass
        
    def _open_qldd(self, file):
        filename = urllib.unquote(file.get_uri()[7:])
        qldd = QLDD_KEY
        os.system('%s %s &' % (qldd, filename))

    def menu_activate_cb(self, menu, file):
        self._open_qldd(file)

    def get_file_items(self, window, files):
        if len(files) != 1:
            return
        
        file = files[0]
        if file.is_directory() or file.get_uri_scheme() != 'file':
            return
        
        item = Nautilus.MenuItem(name='NautilusPython::openqldd_file_item',
                                 label='View dependecy' ,
                                 tip='View dependecy of %s' % file.get_name())
        item.connect('activate', self.menu_activate_cb, file)
        return item,
