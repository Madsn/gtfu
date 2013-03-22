from django.db import models
from django.utils.encoding import smart_unicode
from datetime import datetime
from django.utils.timezone import utc

class Sleeper(models.Model):
    name = models.CharField(max_length=20, unique=True)
    password = models.CharField(max_length=20)
    pwron = models.BooleanField(default=False)
    restart = models.BooleanField(default=False)
    pwroff = models.BooleanField(default=False)
    timestamp = models.DateTimeField(auto_now=True, auto_now_add=True)
    clear_after_minutes = models.IntegerField(default=15)

    def __unicode__(self):
        return smart_unicode(self.name)

    def resetState(self):
        self.set_flags(False, False, False)
        self.save()

    def setPwrOn(self):
        self.set_flags(True, False, False)
        self.save()

    def setSoftOff(self): #TODO: soft off flag needed?
                          #or is timing acceptable
        self.setPwrOn()

    def setRestart(self):
        self.set_flags(False, True, False)
        self.save()

    def setPwrOff(self):
        self.set_flags(False, False, True)
        self.save()

    def clear_old(self):
        now = datetime.utcnow().replace(tzinfo=utc)
        if (self.pwron or self.restart or self.pwroff) and (
                        (now-self.timestamp).seconds > self.clear_after_minutes*60):
            self.resetState()

    def set_flags(self, pwron, restart, pwroff):
        self.pwron = pwron
        self.restart = restart
        self.pwroff = pwroff


def clear_old_flags():
    sleepers = Sleeper.objects.all()
    for sleeper in sleepers:
        sleeper.clear_old()
