from django.http import HttpResponse
from gtfu_server.models import Sleeper

def gtfu_handler(request):
    # toggle state
    sleeper = None
    password = None
    try:    
        name = request.GET['name']
        password = request.GET['password']
        sleeper = Sleeper.objects.get(name=name)
    except:
        return HttpResponse("Who?? Go fuck yourself.", status=404)
    if sleeper.password == password:    
        sleeper.state = True
        sleeper.save()
        return HttpResponse("Fine, I'll wake the fucker up, now get lost", status=202)#202-accepted
    else:
        return HttpResponse("Wrong password shithead, better luck next time", status=403)

def gtfu_poll(request):
    # check state
    sleeper = None
    try:    
        name = request.GET['name']
        sleeper = Sleeper.objects.get(name=name)
    except:
        return HttpResponse("I don't know that ID, so fuck you.", status=404)
    if sleeper.state:
        # time to wake up
        # first reset the state
        sleeper.state = False
        sleeper.save()
        return HttpResponse("Fuck yes! Wake that bitch up", status=202) #202-accepted
    else:
        return HttpResponse("Fuck off!", status=200) #200-OK

def gtfu_add(request):
    name = None
    try:
        name = request.GET['name']
        password = request.GET['password']
    except:
        return HttpResponse("You fucking retard, you forgot to tell me your name", status=403)#forbidden
    sleeper = Sleeper(name=name, password=password)
    sleeper.save()
    return HttpResponse("Hey dickhead, here's your motherfucking ID: {0}".format(sleeper.id), status=201)#201-created
