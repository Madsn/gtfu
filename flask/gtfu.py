from flask import Flask, request, make_response
from datetime import datetime, timedelta
app = Flask(__name__)

class User:

    def __init__(self):
        self.users = {
            "balls": {"user": "balls", "password": "jabadoo", "pwron": 0, "restart": 0, "pwroff": 0, "timestamp": datetime.now()},
            "odin": {"user": "odin", "password": "gtfu", "pwron": 0, "restart": 0, "pwroff": 0, "timestamp": datetime.now()},
            "thor": {"user": "thor", "password": "gtfu", "pwron": 0, "restart": 0, "pwroff": 0, "timestamp": datetime.now()}}

    def login(self, name, password):
        print(self.users)
        if name in self.users:
            print("found user")
            if self.users[name]["password"] != password:
                return None
            if self.timestamp_expired(name):
                self.reset_state(name)
            return self.users[name]
        else:
            print("user not found")
            return None

    def timestamp_expired(self, name):
        time_since_last_check = self.users[name]["timestamp"] - datetime.now()
        command_expires_after = timedelta(minutes=5)
        if time_since_last_check < command_expires_after:
            return False
        return True

    def reset_state(self, name):
        user = self.users[name]
        user["pwron"] = 0
        user["pwroff"] = 0
        user["restart"] = 0
        user["timestamp"] = datetime.now()
        self.users[name] = user

    def update_cmdstate_and_get_response(self, userinfo, cmdstate):
        user = self.users[userinfo["user"]]
        if cmdstate == 1:
            user["pwron"] = 1
            return make_response("Fine, I'll wake the fucker up, now get lost", 202) #202-accepted
        elif cmdstate == 2:
            user["pwron"] = 1
            return make_response("Soft? You pussy.", status=202)
        elif cmdstate == 3:
            user["pwroff"] = 1
            return make_response("Yeah! Kill that fucker!", status=202)
        elif cmdstate == 4:
            user["restart"] = 1
            return make_response("Whatever fucker, I'll tell him to reboot from u.", status=202)
        else:
            return make_response("Unknown cmdstate");


def get_poll_reply(userinfo):
    global user
    resp = ""
    if userinfo["pwron"]:
        # time to wake up
        # first reset the state
        resp = make_response("1-Fuck yes! Wake that bitch up", 202) #202-accepted
    elif userinfo["restart"]:
        resp = make_response("2-That motherfucker needs a reboot!", 202)
    elif userinfo["pwroff"]:
        resp = make_response("3-Let the motherfucker burn!", 202)
    else:
        return make_response("0-Fuck off!", 200) #200-OK
    user.reset_state(userinfo["user"])
    return resp


@app.route("/api/poll/") # polling entry point, for arduino clients
def embedded_entry_point():
    global user
    name = request.args.get("name", None)
    password = request.args.get("password", None)
    userinfo = user.login(name, password)
    if userinfo is not None:
        return get_poll_reply(userinfo)
    else:
        return make_response("Who?? Go fuck yourself.", 404)

@app.route("/api/gtfu/") # entry point for android and other controller clients
def controller_entry_point():
    global user
    name = request.args.get("name", None)
    password = request.args.get("password", None)
    userinfo = user.login(name, password)
    if userinfo is not None:
        cmdstate = request.args.get("cmdstate", 1) # default to 1, first version of clients don't send a cmdstate
        return user.update_cmdstate_and_get_response(userinfo, cmdstate)
    else:
        return make_response("Who?? Go fuck yourself.", 404)


user = User()
if __name__ == "__main__":
    app.run()
