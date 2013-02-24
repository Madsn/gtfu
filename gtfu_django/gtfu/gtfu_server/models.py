from django.db import models

class Sleeper(models.Model):
    name = models.CharField(max_length=20, unique=True)
    password = models.CharField(max_length=20)
    pwron = models.BooleanField(default=False)
    restart = models.BooleanField(default=False)
    pwroff = models.BooleanField(default=False)
