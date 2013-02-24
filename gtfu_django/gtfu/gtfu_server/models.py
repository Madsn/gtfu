from django.db import models
from django.utils.encoding import smart_unicode

class Sleeper(models.Model):
    name = models.CharField(max_length=20, unique=True)
    password = models.CharField(max_length=20)
    pwron = models.BooleanField(default=False)
    restart = models.BooleanField(default=False)
    pwroff = models.BooleanField(default=False)

    def __unicode__(self):
        return smart_unicode(self.name)

    def resetState(self):
        self.pwron = False
        self.restart = False
        self.pwroff = False
        self.save()

    def setPwrOn(self):
        self.pwron = True
        self.restart = False
        self.pwroff = False
        self.save()

    def setPwrOff(self):
        self.pwron = False
        self.restart = False
        self.pwroff = True
        self.save()


    def setRestart(self):
        self.pwron = False
        self.restart = True
        self.pwroff = False
        self.save()
