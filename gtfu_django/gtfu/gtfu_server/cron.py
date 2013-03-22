import cronjobs
from models import clear_old_flags

@cronjobs.register
def clear_old_flags_job():
    clear_old_flags()
