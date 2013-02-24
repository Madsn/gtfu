from django.db import models

class Sleeper(models.Model):
    name = models.CharField(max_length=20, unique=True)
    password = models.CharField(max_length=20)
    state = models.BooleanField(default=False)
