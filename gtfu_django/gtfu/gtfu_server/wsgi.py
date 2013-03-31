import os
import sys

from django.core.handlers.wsgi import WSGIHandler

#basedir = "/home/madsn/webapps/gtfu_django/"
#sys.path = [basedir + 'gtfu/gtfu_server',basedir+'gtfu',basedir+'lib/python2.7'] + sys.path
#sys.path.insert(0, '/home/madsn/webapps/gtfu_django/gtfu/cronjobs')
os.environ['DJANGO_SETTINGS_MODULE'] = 'gtfu_server.settings'
application = WSGIHandler()
