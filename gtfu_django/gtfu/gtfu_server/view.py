from django.http import HttpResponse
from gtfu_server.models import Sleeper

def gtfu_handler(request):
    # toggle state
    sleeper = None
    password = None
    cmdstate = None
    try:
        name = request.GET['name']
        password = request.GET['password']
        sleeper = Sleeper.objects.get(name=name)
        # old clients send no cmdstate
        cmdstate = int(request.GET.get('cmdstate', 1))
    except:
        return HttpResponse("Who?? Go fuck yourself.", status=404)
    if sleeper.password == password:
        if cmdstate == 1:
            sleeper.setPwrOn()
            return HttpResponse("Fine, I'll wake the fucker up, now get lost", status=202)#202-accepted
        elif cmdstate == 2:
            sleeper.setSoftOff()
            return HttpResponse("Soft? You pussy.", status=202)
        elif cmdstate == 3:
            sleeper.setPwrOff()
            return HttpResponse("Yeah! Kill that fucker!", status=202)
        elif cmdstate == 4:
            sleeper.setRestart()
            return HttpResponse("Whatever fucker, I'll tell him to reboot from u.", status=202)
        else:
            return HttpResponse("Unknown cmdstate");
    else:
        return HttpResponse("Wrong password shithead, better luck next time", status=403)

def gtfu_poll(request):
    # check state
    sleeper = None
    try:
        name = request.GET['name']
        sleeper = Sleeper.objects.get(name=name)
    except:
        return HttpResponse("9-I don't know that ID, so fuck you.", status=404)
    if sleeper.pwron:
        # time to wake up
        # first reset the state
        sleeper.resetState()
        return HttpResponse("1-Fuck yes! Wake that bitch up", status=202) #202-accepted
    elif sleeper.restart:
        sleeper.resetState()
        return HttpResponse("2-That motherfucker needs a reboot!", status=202)
    elif sleeper.pwroff:
        sleeper.resetState()
        return HttpResponse("3-Let the motherfucker burn!", status=202)
    else:
        return HttpResponse("0-Fuck off!", status=200) #200-OK

def gtfu_add(request):
    name = None
    try:
        name = request.GET['name']
        password = request.GET['password']
    except:
        return HttpResponse("You fucking retard, you forgot to tell me your name or password", status=403)#forbidden
    sleeper = Sleeper(name=name, password=password)
    sleeper.save()
    return HttpResponse("Hey dickhead, here's your motherfucking ID: {0}".format(sleeper.id), status=201)#201-created

def gtfu_restart(request):
    name = None
    try:
        name = request.GET['name']
        password = request.GET['password']
    except:
        return HttpResponse("You fucking retard, you forgot to tell me your name or pass", status=403)#forbidden
    sleeper = Sleeper.objects.get(name=name)
    sleeper.setRestart()
    return HttpResponse("Whatever fucker, I'll tell him to reboot from u.", status=202)

def gtfu_off(request):
    name = None
    try:
        name = request.GET['name']
        password = request.GET['password']
    except:
        return HttpResponse("You fucking retard, you forgot to tell me your name or pass", status=403)#forbidden
    sleeper = Sleeper.objects.get(name=name)
    sleeper.setPwrOff()
    return HttpResponse("Yeah! Kill that fucker!", status=202)

def gtfu_soft_off(request):
    name = None
    try:
        name = request.GET['name']
        password = request.GET['password']
    except:
        return HttpResponse("You fucking retard, you forgot to tell me your name or pass", status=403)#forbidden
    sleeper = Sleeper.objects.get(name=name)
    sleeper.setSoftOff() #TODO, test if different timing needed for soft off and pwron
    return HttpResponse("Soft? You pussy.", status=202)
