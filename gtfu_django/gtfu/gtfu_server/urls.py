from django.conf.urls import patterns, include, url

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'myproject.views.home', name='home'),
    # url(r'^myproject/', include('myproject.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),

    url(r'^api/gtfu/', 'gtfu_server.view.gtfu_handler'),
    url(r'^api/poll/', 'gtfu_server.view.gtfu_poll'),
    url(r'^api/add/', 'gtfu_server.view.gtfu_add'),
    url(r'^api/restart/', 'gtfu_server.view.gtfu_restart'),
    url(r'^api/stfo/', 'gtfu_server.view.gtfu_off'),
    url(r'^api/stfos/', 'gtfu_server.view.gtfu_soft_off'),
)
